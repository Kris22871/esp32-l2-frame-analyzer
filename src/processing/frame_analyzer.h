// src/processing/frame_analyzer.h
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FreeRTOS task for background parsing and analysis of 802.11 frames
 * 
 * Reads raw packets (rx_packet_t) from the passed RingBuffer,
 * parses L2 headers and detects anomalies (Deauth, Probe, Beacon).
 *
 * @param pvParameters Ring buffer handle (RingbufHandle_t).
 */
void frame_analyzer_task(void *pvParameters);

#ifdef __cplusplus
}
#endif