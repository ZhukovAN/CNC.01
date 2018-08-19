/*
 * i2c_utils.c
 *
 *  Created on: 9 июн. 2018 г.
 *      Author: Alexey N. Zhukov
 */

#include <stm32f1xx_hal.h>
#include <i2c.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <task.h>
#include "i2c_utils.h"
#include "debug_utils.h"

I2C_HandleTypeDef *hMasterI2C = &hi2c1;
I2C_HandleTypeDef *hSlaveI2C = &hi2c2;

uint8_t intBuffer;

#define I2C_MASTER_QUEUE_SIZE 	64
static i2cMasterMessageBuffer   i2cMasterQueueData[I2C_MASTER_QUEUE_SIZE];
static uint8_t 					i2cMasterQueueDataIdx = 0;
static QueueHandle_t       		i2cMasterQueueHandle;

#define I2C_SLAVE_QUEUE_SIZE 	16
static i2cSlaveMessageBuffer    i2cSlaveQueueData[I2C_SLAVE_QUEUE_SIZE];
static uint8_t 					i2cSlaveQueueDataIdx = 0;
static QueueHandle_t       		i2cSlaveQueueHandle;
void vI2CMasterTask(void *theArg);

static SemaphoreHandle_t   		i2cMasterMutexHandle;
static SemaphoreHandle_t   		i2cSlaveMutexHandle;

void vInitI2CMaster(I2C_HandleTypeDef *theMaster) {
    vSemaphoreCreateBinary(i2cMasterMutexHandle);
    i2cMasterQueueHandle = xQueueCreate(I2C_MASTER_QUEUE_SIZE, sizeof(uint8_t));
}

void vInitI2CSlave(I2C_HandleTypeDef *theSlave) {
    i2cSlaveQueueHandle = xQueueCreate(I2C_SLAVE_QUEUE_SIZE, sizeof(uint8_t));
}

void vStartI2CMaster(I2C_HandleTypeDef *theMaster) {
	// Создаем задачу, которая обрабатывает данные, поступающие для передачи через I2C
    xTaskCreate(vI2CMasterTask, "MasterI2C", configMINIMAL_STACK_SIZE, &hi2c1, configMAX_PRIORITIES - 2, NULL);
}

void vI2CMasterTask(void *theArg) {
    for (;;) {
        // Ждем освобождения шины
        xSemaphoreTake(i2cMasterMutexHandle, portMAX_DELAY);
        // Шина свободна, ждем данные из очереди
        xQueueReceive(i2cMasterQueueHandle, &i2cMasterQueueDataIdx, portMAX_DELAY);
        i2cMasterMessageBuffer *l_ptrData = &i2cMasterQueueData[i2cMasterQueueDataIdx];
        uint8_t *l_ptrBuffer = l_ptrData->data.d;
        if (l_ptrData->staticData)
        	l_ptrBuffer = &(l_ptrData->data.s[0]);
        do {
            if (HAL_OK == HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef*)theArg, l_ptrData->address, l_ptrBuffer, l_ptrData->size))
                break;
            vTaskDelay(1);
        } while (true);
    }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    /* Отдать семафор задаче-обработчику - освобождаем шину */
    // Обработчик один, I2C может быть несколько
    if (hi2c->Instance == hMasterI2C->Instance) {
        if (!i2cMasterQueueData[i2cMasterQueueDataIdx].staticData)
            vGCFreeFromISR(i2cMasterQueueData[i2cMasterQueueDataIdx].data.d);
        xSemaphoreGiveFromISR(i2cMasterMutexHandle, &xHigherPriorityTaskWoken);
    }
}

void vStartI2CSlave() {
	HAL_I2C_Slave_Receive_IT(hSlaveI2C, (uint8_t*)(&i2cSlaveQueueData[i2cSlaveQueueDataIdx]), sizeof(i2cSlaveMessageBuffer));
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == hSlaveI2C->Instance) {
		// Данные от GRBL-мастера получены и записаны в кольцевой буфер.
		// Поместим в очередь указатель на эти данные
		xQueueSend(i2cSlaveQueueHandle, &i2cSlaveQueueDataIdx, portMAX_DELAY);
		// Работа с кольцевым буфером
		i2cSlaveQueueDataIdx++;
		if (i2cSlaveQueueDataIdx == I2C_SLAVE_QUEUE_SIZE)
			i2cSlaveQueueDataIdx = 0;
		// Ждем следующую порцию данных от GRBL-мастера
		HAL_I2C_Slave_Receive_IT(hSlaveI2C, (uint8_t*)(&i2cSlaveQueueData[i2cSlaveQueueDataIdx]), sizeof(i2cSlaveMessageBuffer));
		//
		// hi2c->pBuffPtr
		/*
		if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(1);
		} else if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(2);
		} else if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(3);
		}
		*/
	}
}
