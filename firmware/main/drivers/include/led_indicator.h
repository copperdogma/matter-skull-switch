#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the LED indicator
 * 
 * @param gpio_num GPIO pin number for the LED
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_init(int gpio_num);

/**
 * @brief Set the LED indicator to dim state (device is on, no motion detected)
 * 
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_set_dim(void);

/**
 * @brief Set the LED indicator to flash 3 times rapidly (motion detected)
 * and then automatically change to bright state
 * 
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_set_blink(void);

/**
 * @brief Set the LED indicator to full brightness (occupancy active)
 * 
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_set_bright(void);

/**
 * @brief Deinitialize the LED indicator
 * 
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t led_indicator_deinit(void);

#ifdef __cplusplus
}
#endif 