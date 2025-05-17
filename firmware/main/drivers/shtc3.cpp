/*----------------------------------------------------------------------------
 *
 * Copyright (c) Unspecified LLC. All rights reserved.
 *
 * Provided under end user license agreement.
 *
 * THIS FILE IS PROVIDED AS IS AND WITHOUT WARRANTY OF ANY KIND, AND LINUX FOUNDATION PROJECT AND ITS CONTRIBUTORS EXPRESSLY DISCLAIM ALL WARRANTIES (WHETHER EXPRESS OR IMPLIED) OF INNOVATION FITNESS FOR ANY PARTICULAR PURPOSE, MERCHANTABILITY, COUNTER INFRINGEMENT, INTEROPERABILITY OR OTHERWISE. THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY DISCLAIMED.
 *
 *--------------------------------------------------------------------------*/

#include <esp_check.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <driver/i2c.h> // Use legacy I2C driver
#include <cmath> // For NAN

#include <lib/support/CodeUtils.h>
#include <esp_matter.h> // For chip::app::Clusters and esp_matter_invalid
#include <app/ConcreteAttributePath.h> // For chip::app::Clusters definitions
#include "common_macros.h" // For ABORT_APP_ON_FAILURE

#include "shtc3.h"

#define I2C_MASTER_SCL_IO           CONFIG_SHTC3_I2C_SCL_PIN      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_SHTC3_I2C_SDA_PIN      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                     /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          400000                        /*!< I2C master clock frequency */
#define SHTC3_SENSOR_ADDR           0x70                          /*!< slave address for SHTC3 sensor */

static const char *TAG = "shtc3";

typedef struct {
    shtc3_sensor_config_t *config;
    esp_timer_handle_t timer;
    bool is_initialized = false;
    // No bus_handle or dev_handle needed for legacy driver in this context
} shtc3_sensor_ctx_t;

static shtc3_sensor_ctx_t s_ctx;

static esp_err_t shtc3_init_i2c()
{
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
         // .clk_flags = 0, // Only for new driver
    };

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &i2c_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2C driver, err:%d", err);
        return err;
    }

    err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2C driver, err: %d", err);
        return err;
    }
    return ESP_OK;
}

static esp_err_t shtc3_read(uint8_t *data, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true /* enable_ack */);
    // Read temperature first then humidity, with clock stretching enabled
    // SHTC3 specific command: Wakeup (0x3517) then Read Temp/RH High Precision with Clock Stretching (0x7CA2)
    // However, simpler approach is to send measurement command, wait, then read.
    // Command: Measure T & RH, clock stretching, high precision (0x7CA2)
    i2c_master_write_byte(cmd, 0x7C, true /* enable_ack */); // SHTC3_CMD_MEASURE_HPM
    i2c_master_write_byte(cmd, 0xA2, true /* enable_ack */); // 
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SHTC3: Failed to send measurement command: %s", esp_err_to_name(err));
        return err;
    }
    cmd = NULL;

    // Wait for measurement to complete. SHTC3 datasheet specifies max 12.1 ms for HPM.
    vTaskDelay(pdMS_TO_TICKS(15)); 

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | I2C_MASTER_READ, true /* enable_ack */);
    // Read 6 bytes: Temp_MSB, Temp_LSB, Temp_CRC, RH_MSB, RH_LSB, RH_CRC
    err = i2c_master_read(cmd, data, size, I2C_MASTER_LAST_NACK);
     if (err != ESP_OK) { // Check error after read setup
        ESP_LOGE(TAG, "SHTC3: Failed to setup read: %s", esp_err_to_name(err));
        i2c_cmd_link_delete(cmd);
        return err;
    }
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to receive data from SHTC3, err:%d (%s)", err, esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

// SHTC3 CRC checksum function
static uint8_t shtc3_crc8(const uint8_t *data, int len)
{
    uint8_t crc = 0xFF;
    for (int j = 0; j < len; j++) {
        crc ^= data[j];
        for (int i = 0; i < 8; i++) {
            if ((crc & 0x80) != 0) {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}


static void shtc3_sensor_report_task(void *pvParameters)
{
    uint8_t data_rd[6]; // Temp MSB, LSB, CRC, RH MSB, LSB, CRC
    esp_err_t err = shtc3_read(data_rd, sizeof(data_rd));

    if (err == ESP_OK) {
        uint16_t temp_raw = (data_rd[0] << 8) | data_rd[1];
        uint8_t temp_crc = data_rd[2];
        uint16_t humidity_raw = (data_rd[3] << 8) | data_rd[4];
        uint8_t humidity_crc = data_rd[5];

        if (shtc3_crc8(data_rd, 2) != temp_crc) {
            ESP_LOGE(TAG, "Temperature CRC check failed");
        } else {
            float temperature = -45.0f + 175.0f * (temp_raw / 65535.0f);
            ESP_LOGI(TAG, "Temperature: %.2f C", temperature);
            if (s_ctx.config && s_ctx.config->temperature.cb) {
                s_ctx.config->temperature.cb(s_ctx.config->temperature.endpoint_id, temperature, s_ctx.config->user_data);
            }
        }

        if (shtc3_crc8(data_rd + 3, 2) != humidity_crc) {
            ESP_LOGE(TAG, "Humidity CRC check failed");
        } else {
            float humidity = 100.0f * (humidity_raw / 65535.0f);
            // Ensure humidity is within 0-100%
            humidity = (humidity < 0.0f) ? 0.0f : humidity;
            humidity = (humidity > 100.0f) ? 100.0f : humidity;
            ESP_LOGI(TAG, "Humidity: %.2f %%", humidity);
            if (s_ctx.config && s_ctx.config->humidity.cb) {
                s_ctx.config->humidity.cb(s_ctx.config->humidity.endpoint_id, humidity, s_ctx.config->user_data);
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to read from SHTC3 sensor");
        // Optionally, report a default/error value or NaN
        if (s_ctx.config && s_ctx.config->temperature.cb) {
             s_ctx.config->temperature.cb(s_ctx.config->temperature.endpoint_id, NAN, s_ctx.config->user_data);
        }
        if (s_ctx.config && s_ctx.config->humidity.cb) {
            s_ctx.config->humidity.cb(s_ctx.config->humidity.endpoint_id, NAN, s_ctx.config->user_data);
        }
    }
    vTaskDelete(NULL);
}


static void shtc3_sensor_timer_cb(void *arg)
{
    // Create a task to handle sensor reading and reporting
    // This avoids blocking the timer callback, which should be short
    xTaskCreate(shtc3_sensor_report_task, "shtc3_report", 2048, NULL, 5, NULL);
}

esp_err_t shtc3_sensor_init(shtc3_sensor_config_t *config)
{
    ESP_LOGI(TAG, "Initializing SHTC3 sensor");
    if (s_ctx.is_initialized) {
        ESP_LOGI(TAG, "SHTC3 sensor already initialized");
        return ESP_OK;
    }

    s_ctx.config = config;

    esp_err_t err = shtc3_init_i2c();
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialize I2C for SHTC3, err: %d", err));

    s_ctx.is_initialized = true;

    esp_timer_create_args_t args = {
        .callback = shtc3_sensor_timer_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "shtc3_sensor_timer",
    };

    err = esp_timer_create(&args, &s_ctx.timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create failed, err:%d", err);
        // i2c_driver_delete(I2C_MASTER_NUM); // Clean up I2C driver if timer fails
        return err;
    }

    err = esp_timer_start_periodic(s_ctx.timer, config->interval_ms * 1000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_start_periodic failed: %d", err);
        // i2c_driver_delete(I2C_MASTER_NUM); // Clean up I2C driver
        return err;
    }

    ESP_LOGI(TAG, "SHTC3 sensor initialized and timer started");
    return ESP_OK;
}
