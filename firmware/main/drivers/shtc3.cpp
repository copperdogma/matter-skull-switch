/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <driver/i2c_master.h>
#include <hal/gpio_types.h>

#include <lib/support/CodeUtils.h>

#include <drivers/shtc3.h>

static const char * TAG = "shtc3";

#define I2C_MASTER_SCL_IO CONFIG_SHTC3_I2C_SCL_PIN
#define I2C_MASTER_SDA_IO CONFIG_SHTC3_I2C_SDA_PIN
#define I2C_MASTER_NUM I2C_NUM_0    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

#define SHTC3_SENSOR_ADDR 0x70      /*!< I2C address of SHTC3 sensor */

typedef struct {
    shtc3_sensor_config_t *config;
    esp_timer_handle_t timer;
    bool is_initialized = false;
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
} shtc3_sensor_ctx_t;

static shtc3_sensor_ctx_t s_ctx;

static esp_err_t shtc3_init_i2c()
{
    i2c_master_bus_config_t bus_config = {};
    
    bus_config.sda_io_num = static_cast<gpio_num_t>(I2C_MASTER_SDA_IO);
    bus_config.scl_io_num = static_cast<gpio_num_t>(I2C_MASTER_SCL_IO);
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.i2c_port = I2C_MASTER_NUM;
    bus_config.flags.enable_internal_pullup = 1;

    esp_err_t err = i2c_new_master_bus(&bus_config, &s_ctx.bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C master bus, err:%d", err);
        return err;
    }

    i2c_device_config_t dev_config = {};
    dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_config.device_address = SHTC3_SENSOR_ADDR;
    dev_config.scl_speed_hz = I2C_MASTER_FREQ_HZ;

    err = i2c_master_bus_add_device(s_ctx.bus_handle, &dev_config, &s_ctx.dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add I2C device, err:%d", err);
        i2c_del_master_bus(s_ctx.bus_handle);
        return err;
    }

    return ESP_OK;
}

static esp_err_t shtc3_read(uint8_t *data, size_t size)
{
    uint8_t cmd[2] = {0x7C, 0xA2};
    
    esp_err_t err = i2c_master_transmit(s_ctx.dev_handle, cmd, sizeof(cmd), -1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send SHTC3 measurement command, err:%d", err);
        return err;
    }

    vTaskDelay(pdMS_TO_TICKS(15));

    err = i2c_master_receive(s_ctx.dev_handle, data, size, -1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to receive data from SHTC3, err:%d", err);
        return err;
    }

    return ESP_OK;
}

// Temperature in degree Celsius
static float shtc3_get_temp(uint16_t raw_temp)
{
    return 175.0f * (static_cast<float>(raw_temp) / 65535.0f) - 45.0f;
}

// Humidity in percentage
static float shtc3_get_humidity(uint16_t raw_humidity)
{
    return 100.0f * (static_cast<float>(raw_humidity) / 65535.0f);
}

static esp_err_t shtc3_get_read_temp_and_humidity(float & temp, float & humidity)
{
    // foreach temperature and humidity: two bytes data, one byte for checksum
    uint8_t data[6] = {0};

    esp_err_t err = shtc3_read(data, sizeof(data));
    if (err != ESP_OK) {
        return err;
    }

    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_humidity = (data[3] << 8) | data[4];

    temp = shtc3_get_temp(raw_temp);
    humidity = shtc3_get_humidity(raw_humidity);

    return ESP_OK;
}

static void timer_cb_internal(void *arg)
{
    auto *ctx = (shtc3_sensor_ctx_t *) arg;
    if (!(ctx && ctx->config)) {
        return;
    }

    float temp, humidity;
    esp_err_t err = shtc3_get_read_temp_and_humidity(temp, humidity);
    if (err != ESP_OK) {
        return;
    }
    if (ctx->config->temperature.cb) {
        ctx->config->temperature.cb(ctx->config->temperature.endpoint_id, temp, ctx->config->user_data);
    }
    if (ctx->config->humidity.cb) {
        ctx->config->humidity.cb(ctx->config->humidity.endpoint_id, humidity, ctx->config->user_data);
    }
}

esp_err_t shtc3_sensor_init(shtc3_sensor_config_t *config)
{
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // we need at least one callback so that we can start notifying application layer
    if (config->temperature.cb == NULL || config->humidity.cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_ctx.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = shtc3_init_i2c();
    if (err != ESP_OK) {
        return err;
    }

    // keep the pointer to config
    s_ctx.config = config;

    esp_timer_create_args_t args = {
        .callback = timer_cb_internal,
        .arg = &s_ctx,
    };

    err = esp_timer_create(&args, &s_ctx.timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create failed, err:%d", err);
        i2c_master_bus_rm_device(s_ctx.dev_handle);
        i2c_del_master_bus(s_ctx.bus_handle);
        return err;
    }

    err = esp_timer_start_periodic(s_ctx.timer, config->interval_ms * 1000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_start_periodic failed: %d", err);
        i2c_master_bus_rm_device(s_ctx.dev_handle);
        i2c_del_master_bus(s_ctx.bus_handle);
        return err;
    }

    s_ctx.is_initialized = true;
    ESP_LOGI(TAG, "shtc3 initialized successfully");

    return ESP_OK;
}
