/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>
#include <bsp/esp-bsp.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_ota.h>
#include <nvs_flash.h>

#include <app_openthread_config.h>
#include "app_reset.h"
#include "utils/common_macros.h"

// For VID/PID and Onboarding Codes (official example method)
#include <app/server/OnboardingCodesUtil.h>

// Attempting to include ESP32 specific config to resolve GetVendorId issue
#include <platform/ConfigurationManager.h> // Base interface
#include <platform/ESP32/ESP32Config.h>    // ESP32 specific implementations

// drivers implemented by this example
#include <drivers/shtc3.h>
#include "drivers/include/pir_sensor.h"

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static void occupancy_sensor_notification(uint16_t endpoint_id, bool occupancy, void *user_data)
{
    // schedule the attribute update so that we can report it from matter thread
    chip::DeviceLayer::SystemLayer().ScheduleLambda([endpoint_id, occupancy]() {
        attribute_t * attribute = attribute::get(endpoint_id,
                                                 OccupancySensing::Id,
                                                 OccupancySensing::Attributes::Occupancy::Id);

        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        attribute::get_val(attribute, &val);
        val.val.b = occupancy;

        attribute::update(endpoint_id, OccupancySensing::Id, OccupancySensing::Attributes::Occupancy::Id, &val);
    });
}

static esp_err_t factory_reset_button_register()
{
    button_handle_t push_button;
    esp_err_t err = bsp_iot_button_create(&push_button, NULL, BSP_BUTTON_NUM);
    VerifyOrReturnError(err == ESP_OK, err);
    return app_reset_button_register(push_button);
}

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
    // Since this is just a sensor and we don't expect any writes on our temperature sensor,
    // so, return success.
    return ESP_OK;
}

extern "C" void app_main()
{
    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize push button on the dev-kit to reset the device */
    esp_err_t err = factory_reset_button_register();
    ABORT_APP_ON_FAILURE(ESP_OK == err, ESP_LOGE(TAG, "Failed to initialize reset button, err:%d", err));

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

    // add the occupancy sensor
    occupancy_sensor::config_t occupancy_sensor_config;
    occupancy_sensor_config.occupancy_sensing.occupancy_sensor_type =
        chip::to_underlying(OccupancySensing::OccupancySensorTypeEnum::kPir);
    occupancy_sensor_config.occupancy_sensing.occupancy_sensor_type_bitmap =
        chip::to_underlying(OccupancySensing::OccupancySensorTypeBitmap::kPir);

    endpoint_t * occupancy_sensor_ep = occupancy_sensor::create(node, &occupancy_sensor_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(occupancy_sensor_ep != nullptr, ESP_LOGE(TAG, "Failed to create occupancy_sensor endpoint"));

    // Configure PIROccupiedToUnoccupiedDelay attribute
    if (occupancy_sensor_ep) {
        cluster_t *occupancy_cluster = cluster::get(occupancy_sensor_ep, OccupancySensing::Id);
        if (occupancy_cluster) {
            uint32_t delay_attr_id = OccupancySensing::Attributes::PIROccupiedToUnoccupiedDelay::Id;
            esp_matter_attr_val_t default_val = esp_matter_uint16(900); // 15 minutes = 900 seconds
            attribute_t *delay_attribute = attribute::get(occupancy_cluster, delay_attr_id);

            if (!delay_attribute) {
                ESP_LOGI(TAG, "Creating PIROccupiedToUnoccupiedDelay attribute with default %d s", default_val.val.u16);
                attribute::create(occupancy_cluster, delay_attr_id,
                                  ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                  default_val);
            } else {
                ESP_LOGI(TAG, "PIROccupiedToUnoccupiedDelay attribute exists. Ensuring default if 0.");
                esp_matter_attr_val_t current_val;
                if (attribute::get_val(delay_attribute, &current_val) == ESP_OK) {
                    if (current_val.val.u16 == 0) { // Matter spec default is 0, override with our default.
                        ESP_LOGI(TAG, "Setting PIROccupiedToUnoccupiedDelay to default %d s", default_val.val.u16);
                        attribute::update(endpoint::get_id(occupancy_sensor_ep), OccupancySensing::Id, delay_attr_id, &default_val);
                    } else {
                        ESP_LOGI(TAG, "PIROccupiedToUnoccupiedDelay already has a value: %d s", current_val.val.u16);
                    }
                }
                // Ensuring flags are set (Note: esp-matter might not support changing flags post-creation easily.
                // If flags are incorrect, attribute might need to be re-created or SDK modified.
                // For now, we assume flags set at initial creation (if SDK does so) or by our creation are sufficient.)
                 ESP_LOGI(TAG, "Assuming PIROccupiedToUnoccupiedDelay flags are correctly set (Writable, Non-Volatile).");
            }
        } else {
            ESP_LOGE(TAG, "Failed to get OccupancySensing cluster from occupancy_sensor_ep for delay attribute setup");
        }
    }

    // initialize occupancy sensor driver (pir)
    static pir_sensor_config_t pir_config = {
        .cb = occupancy_sensor_notification,
        .endpoint_id = endpoint::get_id(occupancy_sensor_ep),
        .user_data = nullptr, // Explicitly initialize to suppress warning
    };
    err = pir_sensor_init(&pir_config);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialize occupancy sensor driver"));

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

// Helper function to be called from C code (e.g., pir_sensor.c)
extern "C" uint16_t get_pir_unoccupied_delay_seconds(uint16_t endpoint_id)
{
    cluster_t *occupancy_cluster = cluster::get(endpoint_id, OccupancySensing::Id);
    if (occupancy_cluster) {
        uint32_t delay_attr_id = OccupancySensing::Attributes::PIROccupiedToUnoccupiedDelay::Id;
        attribute_t *delay_attribute = attribute::get(occupancy_cluster, delay_attr_id);
        if (delay_attribute) {
            esp_matter_attr_val_t current_val;
            if (attribute::get_val(delay_attribute, &current_val) == ESP_OK) {
                if (current_val.val.u16 > 0) { // Ensure a valid delay is returned
                    return current_val.val.u16;
                }
            }
        }
    }
    ESP_LOGW(TAG, "Failed to get PIROccupiedToUnoccupiedDelay for ep %d, returning default 15s", endpoint_id);
    return 900; // Default to 15 minutes (900 seconds) if attribute not found or value is 0
}
