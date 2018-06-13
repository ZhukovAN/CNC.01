/*
 * i2c_utils.c
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#include <stm32f1xx_hal.h>
#include <i2c.h>
#include "i2c_utils.h"

I2C_HandleTypeDef *hMasterI2C = &hi2c1;
I2C_HandleTypeDef *hSlaveI2C = &hi2c2;

uint8_t intBuffer;

void vInitI2C() {

}

void vStartI2C() {
	HAL_I2C_Slave_Receive_IT(hSlaveI2C, &intBuffer, 1);
}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_Slave_Receive_IT(hSlaveI2C, &intBuffer, 1);
}
