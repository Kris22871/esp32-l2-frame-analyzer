#include "wifi_sniffer.h"
#include "ieee80211_types.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "wifi_sniffer";
static RingbufHandle_t s_rx_buf = NULL;

static void IRAM_ATTR wifi_promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) {
        return;
    }

    const wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    uint16_t len = pkt->rx_ctrl.sig_len;

    // allocating memory in a circular buffer: metadata + payload
    size_t total_size = sizeof(rx_packet_t) + len;
    rx_packet_t *item = NULL;
    
    // attention: exiting an ISR without waiting
    BaseType_t res = xRingbufferSendAcquire(s_rx_buf, (void **)&item, total_size, 0);
    if (res == pdTRUE && item != NULL) {
        item->rssi = pkt->rx_ctrl.rssi;
        item->channel = pkt->rx_ctrl.channel;
        item->payload_len = len;
        memcpy(item->payload, pkt->payload, len);
        xRingbufferSendComplete(s_rx_buf, item);
    }
}

esp_err_t wifi_sniffer_init(RingbufHandle_t ringbuf) {
    s_rx_buf = ringbuf;

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_promiscuous_filter_t filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
    };
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&filter));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_rx_cb));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));

    ESP_LOGI(TAG, "Promiscuous mode enabled");
    return ESP_OK;
}

esp_err_t wifi_sniffer_set_channel(uint8_t ch) {
    return esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
}