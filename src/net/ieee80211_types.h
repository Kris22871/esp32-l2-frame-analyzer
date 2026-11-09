#pragma once
#include <stdint.h>

#define WIFI_MGMT_TYPE_PROBE_REQ  0x0040
#define WIFI_MGMT_TYPE_BEACON     0x0080
#define WIFI_MGMT_TYPE_DEAUTH     0x00C0

typedef struct __attribute__((packed)) {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t  addr1[6]; // Receiver / DA
    uint8_t  addr2[6]; // Transmitter / SA
    uint8_t  addr3[6]; // BSSID
    uint16_t seq_ctrl;
} wifi_mgmt_hdr_t;

typedef struct {
    int8_t   rssi;
    uint8_t  channel;
    uint16_t payload_len;
    uint8_t  payload[];
} rx_packet_t;