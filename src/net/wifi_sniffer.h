#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "esp_err.h"

esp_err_t wifi_sniffer_init(RingbufHandle_t ringbuf);
esp_err_t wifi_sniffer_set_channel(uint8_t ch);