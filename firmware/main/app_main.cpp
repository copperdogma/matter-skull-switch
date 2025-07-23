/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_ota.h>
#include <nvs_flash.h>

// Include the sdkconfig.h file to access Kconfig values
#include "sdkconfig.h"

#include <app_openthread_config.h>
#include "app_reset.h"
#include "utils/common_macros.h"

// Button component direct include (for factory reset only)
#include "iot_button.h"
#include "button_gpio.h"

// For VID/PID and Onboarding Codes (official example method)
#include <app/server/OnboardingCodesUtil.h>

// Attempting to include ESP32 specific config to resolve GetVendorId issue
#include <platform/ConfigurationManager.h> // Base interface
#include <platform/ESP32/ESP32Config.h>    // ESP32 specific implementations

// drivers implemented by this example


#include <esp_matter_event.h>
#include <esp_console.h>
#include <esp_vfs_dev.h>
#include <driver/gpio.h>
#include <esp_timer.h>

static const char *TAG = "app_main";

// Global variables
static uint16_t g_switch_endpoint_id = 0;
static uint16_t g_ui_endpoint_id = 0; // On/Off endpoint for Home UI
static bool g_pulse_active = false;    // Track if pulse is currently active

// Use the Kconfig value directly

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

// Define GPIO pins
#define BUTTON_GPIO CONFIG_BSP_BUTTON_GPIO  // GPIO 9 on ESP32-C3 SuperMini
#define BSP_BUTTON_NUM 0
#define SIGNAL_GPIO (gpio_num_t)4            // GPIO 4 for signal output
#define PULSE_DURATION_MS 500               // 500ms pulse duration

static void open_commissioning_window_if_necessary()
{
    VerifyOrReturn(chip::Server::GetInstance().GetFabricTable().FabricCount() == 0);

    chip::CommissioningWindowManager & commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
    VerifyOrReturn(commissionMgr.IsCommissioningWindowOpen() == false);

    // After removing last fabric, this example does not remove the Wi-Fi credentials
    // and still has IP connectivity so, only advertising on DNS-SD.
    CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(chip::System::Clock::Seconds16(300),
                                    chip::CommissioningWindowAdvertisement::kDnssdOnly);
    if (err != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
    }
}

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
        ESP_LOGI(TAG, "Fabric removed successfully");
        open_commissioning_window_if_necessary();
        break;

    case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:
        ESP_LOGI(TAG, "BLE deinitialized and memory reclaimed");
        break;

    default:
        break;
    }
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing a light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

// GPIO control functions (defined before they're used)
static esp_err_t init_signal_gpio()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SIGNAL_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO %d: %s", SIGNAL_GPIO, esp_err_to_name(err));
        return err;
    }
    
    // Initialize to LOW
    gpio_set_level(SIGNAL_GPIO, 0);
    ESP_LOGI(TAG, "Signal GPIO %d initialized", SIGNAL_GPIO);
    return ESP_OK;
}

static void start_pulse()
{
    if (g_pulse_active) {
        ESP_LOGW(TAG, "Pulse already active, ignoring");
        return;
    }
    
    g_pulse_active = true;
    gpio_set_level(SIGNAL_GPIO, 1);
    ESP_LOGI(TAG, "Pulse started - GPIO %d HIGH", SIGNAL_GPIO);
    
    // Schedule pulse end
    esp_timer_create_args_t timer_args = {
        .callback = [](void* arg) {
            gpio_set_level(SIGNAL_GPIO, 0);
            g_pulse_active = false;
            ESP_LOGI(TAG, "Pulse ended - GPIO %d LOW", SIGNAL_GPIO);
        },
        .arg = nullptr,
        .name = "pulse_timer"
    };
    
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_once(timer, PULSE_DURATION_MS * 1000); // Convert to microseconds
}

static void stop_pulse()
{
    gpio_set_level(SIGNAL_GPIO, 0);
    g_pulse_active = false;
    ESP_LOGI(TAG, "Pulse stopped - GPIO %d LOW", SIGNAL_GPIO);
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE) {
        // Handle On/Off cluster commands
        if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
            bool new_state = val->val.b;
            ESP_LOGI(TAG, "On/Off command received: %s", new_state ? "ON" : "OFF");
            
            if (new_state) {
                // Matter "ON" command - start pulse
                start_pulse();
            } else {
                // Matter "OFF" command - stop pulse immediately
                stop_pulse();
            }
        }
    }
    return ESP_OK;
}

static esp_err_t factory_reset_button_register()
{
    // Create button configurations
    button_config_t button_config = {
        .long_press_time = 5000,     // 5 seconds for long press
        .short_press_time = 50,      // 50ms for short press
    };
    
    button_gpio_config_t gpio_config = {
        .gpio_num = 9,               // GPIO 9 (BOOT button on ESP32-C3 SuperMini)
        .active_level = 0,           // Active low
        .enable_power_save = false,  // No power save
        .disable_pull = false,       // Use internal pull-up
    };
    
    button_handle_t push_button = NULL;
    
    // Create the GPIO button device
    esp_err_t err = iot_button_new_gpio_device(&button_config, &gpio_config, &push_button);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create button device: %s", esp_err_to_name(err));
        return err;
    }
    
    return app_reset_button_register(push_button);
}

// AFTER factory_reset_button_register(), add switch button callback registration

/* -------------------------------------------------------------------------- */
/* Generic Switch button callback                                             */
/* -------------------------------------------------------------------------- */





// Physical button handling removed - GPIO control is now via Matter commands only

// Button registration removed - GPIO control is now via Matter commands only

// Simple factory reset trigger - will reset after 10 seconds
static void trigger_factory_reset_timer(void)
{
    ESP_LOGW(TAG, "=== FACTORY RESET TRIGGERED ===");
    ESP_LOGW(TAG, "Device will reset in 10 seconds...");
    ESP_LOGW(TAG, "Unplug power now if you want to cancel!");
    
    vTaskDelay(pdMS_TO_TICKS(10000)); // Wait 10 seconds
    
    ESP_LOGI(TAG, "Starting factory reset NOW");
    esp_matter::factory_reset();
}

// Console command for factory reset
static int factory_reset_cmd(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "confirm") == 0) {
        // Start the reset in a new task
        xTaskCreate([](void*){ trigger_factory_reset_timer(); vTaskDelete(NULL); }, 
                   "factory_reset", 4096, NULL, 5, NULL);
        return 0;
    } else {
        printf("Usage: factory_reset confirm\n");
        printf("WARNING: This will erase all pairing data!\n");
        return 1;
    }
}

static void register_factory_reset_console_cmd()
{
    esp_console_cmd_t cmd = {
        .command = "factory_reset",
        .help = "Perform factory reset (use 'factory_reset confirm')",
        .hint = NULL,
        .func = &factory_reset_cmd,
    };
    esp_console_cmd_register(&cmd);
}

extern "C" void app_main()
{
    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize console for factory reset command */
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    esp_console_new_repl_uart(&uart_config, &repl_config, &repl);
    register_factory_reset_console_cmd();
    esp_console_start_repl(repl);

    /* Initialize push button on the dev-kit to reset the device */
    esp_err_t err = factory_reset_button_register();
    ABORT_APP_ON_FAILURE(ESP_OK == err, ESP_LOGE(TAG, "Failed to initialize reset button, err:%d", err));

    /* Initialize signal GPIO */
    err = init_signal_gpio();
    ABORT_APP_ON_FAILURE(ESP_OK == err, ESP_LOGE(TAG, "Failed to initialize signal GPIO, err:%d", err));



    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config{}; // Explicitly zero-initialize

    // --- BEGIN CUSTOM DEVICE INFO CONFIGURATION ---
    // When 'Device Info Provider' is 'Custom' (via menuconfig),
    // core device identity (VID, PID, names, versions) is primarily expected
    // from the factory NVS partition.
    // Kconfig settings under "Device Basic Information" provide other fields.
    // We will only set the node_label here if we want to override the Kconfig value at runtime.

    // Basic Information Cluster Configuration for Root Node
    // Set a node_label (user-visible device name for this node).
    // This overrides any node_label set via Kconfig.
    // If not set here, the Kconfig value (if any) will be used.
    // strncpy(node_config.root_node.basic_information.node_label, "MyOccupancySensorRuntime", sizeof(node_config.root_node.basic_information.node_label) - 1);
    // node_config.root_node.basic_information.node_label[sizeof(node_config.root_node.basic_information.node_label) - 1] = '\0'; // Ensure null termination

    // Identify Cluster on Root Node is mandatory and typically initialized by default by the SDK.

    // --- END CUSTOM DEVICE INFO CONFIGURATION ---

    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    // ------------------------------------------------------------------
    // Create On/Off Switch endpoint
    // ------------------------------------------------------------------

    endpoint::on_off_switch::config_t switch_cfg; // default config
    endpoint_t *switch_ep = endpoint::on_off_switch::create(node, &switch_cfg, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(switch_ep != nullptr, ESP_LOGE(TAG, "Failed to create on_off_switch endpoint"));

    g_switch_endpoint_id = endpoint::get_id(switch_ep);

    // ------------------------------------------------------------------
    // Create On/Off Light endpoint for UI representation (stateful tile)
    // ------------------------------------------------------------------

    endpoint::on_off_light::config_t light_cfg; // default
    endpoint_t *ui_ep = endpoint::on_off_light::create(node, &light_cfg, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(ui_ep != nullptr, ESP_LOGE(TAG, "Failed to create on_off_light endpoint"));
    g_ui_endpoint_id = endpoint::get_id(ui_ep);

    // Ensure OnOff starts at false (off)
    {
        esp_matter_attr_val_t off_val = esp_matter_bool(false);
        attribute::update(g_ui_endpoint_id, chip::app::Clusters::OnOff::Id,
                          chip::app::Clusters::OnOff::Attributes::OnOff::Id, &off_val);
    }

    // GPIO control is now handled via Matter commands only

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    // PrintOnboardingCodes will log the necessary VID/PID and commissioning info
    chip::DeviceLayer::StackLock lock; // RAII lock for Matter stack
    PrintOnboardingCodes(chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE).Set(chip::RendezvousInformationFlag::kOnNetwork));
}
