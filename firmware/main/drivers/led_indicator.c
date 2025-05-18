#include "drivers/include/led_indicator.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_timer.h"

static const char *TAG = "led_indicator";

// LEDC configuration
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT  // 10-bit resolution, 0-1023 values
#define LEDC_FREQUENCY          5000               // 5kHz PWM frequency

// PWM duty cycle values
#define DUTY_CYCLE_DIM          102                // ~10% brightness when dim
#define DUTY_CYCLE_BRIGHT       1023               // 100% brightness when blinking

// Blinking configuration
#define BLINK_PERIOD_MS         150                // Fast blink every 150ms
#define BLINK_COUNT             3                  // Flash 3 times

static int s_led_gpio = -1;
static bool s_is_blinking = false;
static esp_timer_handle_t s_blink_timer = NULL;
static int s_blink_counter = 0;

// Function prototypes
static void blink_timer_callback(void *arg);

esp_err_t pir_led_indicator_init(int gpio_num) {
    if (s_led_gpio != -1) {
        ESP_LOGW(TAG, "LED indicator already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    s_led_gpio = gpio_num;

    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure LEDC channel
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = s_led_gpio,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // Create blink timer but don't start it yet
    const esp_timer_create_args_t blink_timer_args = {
        .callback = &blink_timer_callback,
        .name = "led_blink_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&blink_timer_args, &s_blink_timer));

    // Initialize with dim brightness
    ESP_ERROR_CHECK(pir_led_indicator_set_dim());

    ESP_LOGI(TAG, "LED indicator initialized on GPIO %d", s_led_gpio);
    return ESP_OK;
}

esp_err_t pir_led_indicator_set_dim(void) {
    if (s_led_gpio == -1) {
        return ESP_ERR_INVALID_STATE;
    }

    // If blinking, stop the timer
    if (s_is_blinking) {
        esp_timer_stop(s_blink_timer);
        s_is_blinking = false;
    }

    // Set to dim brightness
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, DUTY_CYCLE_DIM));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    ESP_LOGD(TAG, "LED indicator set to dim");
    return ESP_OK;
}

esp_err_t pir_led_indicator_set_bright(void) {
    if (s_led_gpio == -1) {
        return ESP_ERR_INVALID_STATE;
    }

    // If blinking, stop the timer
    if (s_is_blinking) {
        esp_timer_stop(s_blink_timer);
        s_is_blinking = false;
    }

    // Set to full brightness
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, DUTY_CYCLE_BRIGHT));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    ESP_LOGD(TAG, "LED indicator set to bright");
    return ESP_OK;
}

esp_err_t pir_led_indicator_set_blink(void) {
    if (s_led_gpio == -1) {
        return ESP_ERR_INVALID_STATE;
    }

    // If already blinking, restart the sequence
    if (s_is_blinking) {
        esp_timer_stop(s_blink_timer);
        s_blink_counter = 0;
    }

    // Set to bright initially
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, DUTY_CYCLE_BRIGHT));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    // Start blinking timer for rapid flashing
    s_blink_counter = 0;
    ESP_ERROR_CHECK(esp_timer_start_periodic(s_blink_timer, BLINK_PERIOD_MS * 1000));
    s_is_blinking = true;

    ESP_LOGD(TAG, "LED indicator set to flash 3 times");
    return ESP_OK;
}

esp_err_t pir_led_indicator_deinit(void) {
    if (s_led_gpio == -1) {
        return ESP_ERR_INVALID_STATE;
    }

    // Stop blinking if active
    if (s_is_blinking) {
        esp_timer_stop(s_blink_timer);
        s_is_blinking = false;
    }

    // Delete the timer
    if (s_blink_timer) {
        esp_timer_delete(s_blink_timer);
        s_blink_timer = NULL;
    }

    // Turn off the LED
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    s_led_gpio = -1;
    ESP_LOGI(TAG, "LED indicator deinitialized");
    return ESP_OK;
}

static void blink_timer_callback(void *arg) {
    static bool led_state = true;
    
    // Toggle LED state
    if (led_state) {
        // Turn off
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    } else {
        // Turn on bright
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, DUTY_CYCLE_BRIGHT);
        
        // Count completed blink cycles (one on-off cycle)
        s_blink_counter++;
        
        // After BLINK_COUNT cycles, switch to steady bright
        if (s_blink_counter >= BLINK_COUNT) {
            // Stop the timer
            esp_timer_stop(s_blink_timer);
            s_is_blinking = false;
            
            // Set to steady bright
            pir_led_indicator_set_bright();
            return;
        }
    }
    
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    led_state = !led_state;
} 