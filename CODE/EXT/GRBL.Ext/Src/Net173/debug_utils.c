/*
 * debug_utils.c
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: alzhukov
 */
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
#include "stm32f1xx_hal.h"
#include "debug_utils.h"

#define STATUS_QUEUE_SIZE 64
static QueueHandle_t hStatusQueue;

void vDebugInfoTask(void *theParams);
void vShowStatusTask(void *theParams);

void vInitDebug() {
	hStatusQueue = xQueueCreate(STATUS_QUEUE_SIZE, sizeof(uint8_t));
	HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ERROR_GPIO_Port, ERROR_Pin, GPIO_PIN_RESET);
}

void vStartDebug() {
	xTaskCreate(vDebugInfoTask, "DBG", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vShowStatusTask, "STATUS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	vShowStatus(3);
}

void vDebugInfoTask(void *theParams) {
	for (;;) {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		vTaskDelay(500);
	}
}

void vShowStatus(uint8_t theStatus) {
	xQueueSend(hStatusQueue, &theStatus, portMAX_DELAY);
}

void vShowStatusTask(void *theParams) {
	for (;;) {
		uint8_t l_intStatus;
		xQueueReceive(hStatusQueue, &l_intStatus, portMAX_DELAY);
		for (uint8_t i = 0 ; i < l_intStatus ; i++) {
			for (uint8_t j = 0 ; j < 2 ; j++){
				HAL_GPIO_TogglePin(STATUS_GPIO_Port, STATUS_Pin);
				vTaskDelay(500);
			}
		}
		vTaskDelay(1000);
	}
}
