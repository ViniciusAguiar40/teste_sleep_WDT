#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/FreeRTOSConfig.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#define TAG "[MAIN]"

void foo_task(void *pvParameters)
{
    // Habilita o monitoramento nessa task
    esp_task_wdt_add(NULL);
 
    while(true)
    {
        // Realimenta o WDT
        esp_task_wdt_reset();

        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("Task", "OK");
    }
}

void app_main(void)
{
    esp_err_t err;
    esp_task_wdt_config_t wdt_config={0};

    // Seta o timeout para 3 segundos
    wdt_config.timeout_ms = 3000;
    // Monitorando idle tasks de todos os nucleos
    wdt_config.idle_core_mask = (1 << portNUM_PROCESSORS) - 1;
    // Gatilho do panic ativado
    wdt_config.trigger_panic = true; 

    // Verifica se TWDT ainda nao foi inicializado
#if !CONFIG_ESP_TASK_WDT_INIT
    err = esp_task_wdt_init(&wdt_config);
    ESP_LOGI(TAG, "Initialization %s", (err == ESP_OK)?"success":"fail");
#else
    // Se ja foi inicializado, apenas reconfigura
    err = esp_task_wdt_reconfigure(&wdt_config);
    ESP_LOGI(TAG, "Reconfigure %s", (err == ESP_OK)?"success":"fail");
#endif
    // Verifica se foi resetado pelo TWDT
    if(esp_reset_reason() == ESP_RST_TASK_WDT)
    {
        ESP_LOGI(TAG, "Reset caused by Task WDT");
    }

    // Inicializa a Task
    ESP_LOGI(TAG, "Foo task created");
    xTaskCreate(foo_task, "Foo task", 2048, NULL, 1, NULL); 
}
