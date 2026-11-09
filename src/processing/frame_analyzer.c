#include "frame_analyzer.h"
#include "net/ieee80211_types.h"
#include "esp_log.h"
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"

static const char *TAG = "analyzer";

void frame_analyzer_task(void *pvParameters) {
    RingbufHandle_t rx_buf = (RingbufHandle_t)pvParameters;
    size_t item_size = 0;

    ESP_LOGI(TAG, "Frame analyzer task started on core %d", xPortGetCoreID());

    while (1) {
        rx_packet_t *pkt = (rx_packet_t *)xRingbufferReceive(rx_buf, &item_size, portMAX_DELAY);
        if (pkt == NULL) continue;

        if (pkt->payload_len >= sizeof(wifi_mgmt_hdr_t)) {
            const wifi_mgmt_hdr_t *hdr = (const wifi_mgmt_hdr_t *)pkt->payload;
            uint16_t fc = hdr->frame_control;

            switch (fc & 0x00FC) {
                case WIFI_MGMT_TYPE_DEAUTH:
                    ESP_LOGW(TAG, "[DEAUTH DETECTED] Src: %02X:%02X:%02X:%02X:%02X:%02X, RSSI: %d dBm",
                             hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
                             hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
                             pkt->rssi);
                    break;

                case WIFI_MGMT_TYPE_BEACON:
                    // Пример чтения Beacon-фреймов
                    break;

                default:
                    break;
            }
        }

        vRingbufferReturnItem(rx_buf, (void *)pkt);
    }
}