#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

#include "net/wifi_sniffer.h"
#include "processing/frame_analyzer.h"

#define RING_BUF_SIZE (64 * 1024)

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    RingbufHandle_t rx_ringbuf = xRingbufferCreate(RING_BUF_SIZE, RINGBUF_TYPE_NOSPLIT);
    assert(rx_ringbuf != NULL);

    // Core 1 allocate for decoding and business logic.
    xTaskCreatePinnedToCore(
        frame_analyzer_task,
        "analyzer_tsk",
        4096,
        (void *)rx_ringbuf,
        tskIDLE_PRIORITY + 3,
        NULL,
        1
    );

    // Wi-Fi radio interface launches on Core 0
    wifi_sniffer_init(rx_ringbuf);
    wifi_sniffer_set_channel(1);
}
