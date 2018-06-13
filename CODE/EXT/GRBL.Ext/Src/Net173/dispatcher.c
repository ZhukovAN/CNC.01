/*
 * dispatcher.c
 *
 *  Created on: 12 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#include <FreeRTOS.h>
#include <queue.h>
#include "dispatcher.h"

#define DATA_QUEUE_SIZE 8
static QueueHandle_t hDataQueue;

void vInitDispatcher() {
	hDataQueue = xQueueCreate(DATA_QUEUE_SIZE, sizeof(data_packet_t));
}

void vStartDispatcher() {

}
