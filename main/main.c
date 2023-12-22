#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_sleep.h"

#define TAG "[MAIN]"
#define SEC_TO_US 1000000

void foo_task(void *pvParameters)
{
    // Enables WDT monitorament on this task
    esp_task_wdt_add(NULL);
 
    while(true)
    {
        // Feeds the WDT each 1 sec
        esp_task_wdt_reset();

        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("Task", "OK");
    }
}

void start_deep_sleep(uint16_t sleep_seconds)
{
    esp_sleep_enable_timer_wakeup(sleep_seconds * SEC_TO_US);
    esp_deep_sleep_start();
}

void app_main(void)
{
    esp_err_t err = 0;
    esp_task_wdt_config_t wdt_config={0};

    // Sets the timeout to 2 sec
    wdt_config.timeout_ms = 2000;
    wdt_config.trigger_panic = false;

    err = esp_task_wdt_init(&wdt_config);
    if(err == ESP_ERR_INVALID_STATE) // Already initialized
    {
        err = esp_task_wdt_reconfigure(&wdt_config);
        ESP_LOGW(TAG, "Reconfigure %s", (err == ESP_OK)?"success":"fail");
    }
    if(esp_reset_reason() == ESP_RST_DEEPSLEEP)
    {
        ESP_LOGI(TAG, "Wake up from Deep-Sleep");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    // Create task
    ESP_LOGI(TAG, "Foo task created");
    xTaskCreate(foo_task, "Foo task", 2048, NULL, 1, NULL);

    // Works normally for 10 seconds
    vTaskDelay(pdMS_TO_TICKS(10000));
    // start_deep_sleep(10);
}
