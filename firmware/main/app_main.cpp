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

// Button component direct include
#include "iot_button.h"
#include "button_gpio.h"

// For VID/PID and Onboarding Codes (official example method)
#include <app/server/OnboardingCodesUtil.h>

// Attempting to include ESP32 specific config to resolve GetVendorId issue
#include <platform/ConfigurationManager.h> // Base interface
#include <platform/ESP32/ESP32Config.h>    // ESP32 specific implementations

// drivers implemented by this example
#include "drivers/include/led_indicator.h"  // Still using LED helper

#include <app/InteractionModelEngine.h>

static const char *TAG = "app_main";

// Use the Kconfig value directly

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

// Define reset button GPIO
#define BUTTON_GPIO CONFIG_BSP_BUTTON_GPIO
#define BSP_BUTTON_NUM 0

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
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE) { // Or POST_UPDATE, depending on when you want to see the value
        if (cluster_id == OccupancySensing::Id && attribute_id == OccupancySensing::Attributes::PIROccupiedToUnoccupiedDelay::Id) {
            ESP_LOGW(TAG, "app_attribute_update_cb: PIROccupiedToUnoccupiedDelay update received for ep %u. New value: %u (type: %d)", 
                     endpoint_id, val->val.u16, val->type);
        } else if (cluster_id == OccupancySensing::Id) {
             ESP_LOGI(TAG, "app_attribute_update_cb: OccupancySensing cluster (ID: %lu) attribute (ID: %lu) update for ep %u. Value type: %d", 
                     cluster_id, attribute_id, endpoint_id, val->type);
        }
    }
    // Since this is just a sensor and we don't expect any writes on our temperature sensor,
    // so, return success.
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
        .gpio_num = 0,               // Default GPIO 0 (Boot button)
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

static uint16_t g_switch_endpoint_id = 0;

static void switch_button_event(void *btn_handle, void *usr_data)
{
    ESP_LOGI(TAG, "Generic Switch: Button pressed (GPIO 3)");

    // Optional: flash LED three times to give feedback
    pir_led_indicator_set_blink();

    // Update Switch cluster CurrentPosition attribute (toggle 0/1)
    attribute_t * attr = attribute::get(g_switch_endpoint_id, chip::app::Clusters::Switch::Id,
                                        chip::app::Clusters::Switch::Attributes::CurrentPosition::Id);
    if (attr) {
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        attribute::get_val(attr, &val);
        val.val.u8 = (val.val.u8 == 0) ? 1 : 0;
        attribute::update(g_switch_endpoint_id, chip::app::Clusters::Switch::Id,
                          chip::app::Clusters::Switch::Attributes::CurrentPosition::Id, &val);
    }
}

static esp_err_t register_switch_button()
{
    button_config_t cfg = {
        .long_press_time = 1000,
        .short_press_time = 50,
    };

    button_gpio_config_t gpio_cfg = {
        .gpio_num = 3,          // GPIO 3 wired to push-button
        .active_level = 0,      // Active-low (internal pull-up)
        .enable_power_save = false,
        .disable_pull = false,
    };

    button_handle_t hbtn = NULL;
    if (iot_button_new_gpio_device(&cfg, &gpio_cfg, &hbtn) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create generic-switch button device");
        return ESP_FAIL;
    }

    // Register callbacks mapped to Matter Switch Events

    // Helper lambda to bind specific event type
    auto reg = [&](button_event_t ev, button_cb_t fn) {
        iot_button_register_cb(hbtn, ev, NULL, fn, NULL);
    };

    // PRESS DOWN -> InitialPress
    reg(BUTTON_PRESS_DOWN, [](void*, void*) {
        chip::app::Clusters::Switch::Events::InitialPress::Type e;
        e.newPosition = 1;
        chip::app::InteractionModelEngine::GetInstance()->SendEvent(g_switch_endpoint_id, e);
    });

    // SHORT RELEASE -> ShortRelease and toggle attribute
    reg(BUTTON_PRESS_UP, [](void*, void*) {
        ESP_LOGI(TAG, "Generic Switch: Short press");
        chip::app::Clusters::Switch::Events::ShortRelease::Type e;
        e.previousPosition = 1;
        chip::app::InteractionModelEngine::GetInstance()->SendEvent(g_switch_endpoint_id, e);

        // Toggle CurrentPosition attribute (0/1)
        attribute_t * attr = attribute::get(g_switch_endpoint_id, chip::app::Clusters::Switch::Id,
                                            chip::app::Clusters::Switch::Attributes::CurrentPosition::Id);
        if (attr) {
            esp_matter_attr_val_t val = esp_matter_invalid(NULL);
            attribute::get_val(attr, &val);
            val.val.u8 = (val.val.u8 == 0) ? 1 : 0;
            attribute::update(g_switch_endpoint_id, chip::app::Clusters::Switch::Id,
                              chip::app::Clusters::Switch::Attributes::CurrentPosition::Id, &val);
        }
        pir_led_indicator_set_blink();
    });

    // LONG PRESS START -> LongPress
    reg(BUTTON_LONG_PRESS_START, [](void*, void*) {
        ESP_LOGI(TAG, "Generic Switch: Long press start");
        chip::app::Clusters::Switch::Events::LongPress::Type e;
        e.newPosition = 1;
        chip::app::InteractionModelEngine::GetInstance()->SendEvent(g_switch_endpoint_id, e);
    });

    // LONG PRESS UP -> LongRelease
    reg(BUTTON_LONG_PRESS_UP, [](void*, void*) {
        chip::app::Clusters::Switch::Events::LongRelease::Type e;
        e.previousPosition = 1;
        chip::app::InteractionModelEngine::GetInstance()->SendEvent(g_switch_endpoint_id, e);
    });

    // DOUBLE CLICK complete -> MultiPressComplete (2 presses)
    reg(BUTTON_DOUBLE_CLICK, [](void*, void*) {
        ESP_LOGI(TAG, "Generic Switch: Double click");
        chip::app::Clusters::Switch::Events::MultiPressComplete::Type e;
        e.newPosition = 1;
        e.currentNumberOfPresses = 2;
        chip::app::InteractionModelEngine::GetInstance()->SendEvent(g_switch_endpoint_id, e);
    });

    // Set MultiPressMax attribute =2
    attribute::update(g_switch_endpoint_id, chip::app::Clusters::Switch::Id,
                      chip::app::Clusters::Switch::Attributes::MultiPressMax::Id,
                      esp_matter_uint8(2));

    return ESP_OK;
}

extern "C" void app_main()
{
    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize push button on the dev-kit to reset the device */
    esp_err_t err = factory_reset_button_register();
    ABORT_APP_ON_FAILURE(ESP_OK == err, ESP_LOGE(TAG, "Failed to initialize reset button, err:%d", err));

    // Initialize LED indicator
    err = pir_led_indicator_init(CONFIG_LED_INDICATOR_GPIO_NUM);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialize LED indicator, err:%d", err));

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
    // Create Generic Switch endpoint
    // ------------------------------------------------------------------

    endpoint::generic_switch::config_t switch_cfg; // default config (momentary, 2 positions)
    endpoint_t *switch_ep = endpoint::generic_switch::create(node, &switch_cfg, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(switch_ep != nullptr, ESP_LOGE(TAG, "Failed to create generic_switch endpoint"));

    g_switch_endpoint_id = endpoint::get_id(switch_ep);

    // Register button on GPIO3 to drive the switch events/attributes
    err = register_switch_button();
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to register switch button"));

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
