/*
 * i2c_utils.c
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#include <stm32f1xx_hal.h>
#include <i2c.h>
#include "i2c_utils.h"
#include "debug_utils.h"

I2C_HandleTypeDef *hMasterI2C = &hi2c1;
I2C_HandleTypeDef *hSlaveI2C = &hi2c2;

uint8_t intBuffer;

#define I2C_BUFFER_SIZE				8
#define GRBL_EXT_CMD_MAX_SIZE       (I2C_BUFFER_SIZE - 2)
#define GRBL_EXT_CMD_SPINDLE        0x01

typedef struct _grbl_ext_cmd_t {
    uint8_t cmd;
    uint8_t size;
    uint8_t data[GRBL_EXT_CMD_MAX_SIZE];
} grbl_ext_cmd_t;
grbl_ext_cmd_t cmdBuffer;

void vInitI2C() {

}

void vStartI2C() {
	HAL_I2C_Slave_Receive_IT(hSlaveI2C, (uint8_t*)(&cmdBuffer), sizeof(grbl_ext_cmd_t));
}

#define bit(n) (1 << n)
#define PL_COND_FLAG_SPINDLE_CW        bit(4)
#define PL_COND_FLAG_SPINDLE_CCW       bit(5)
// Modal Group M7: Spindle control
#define SPINDLE_DISABLE 0 // M5 (Default: Must be zero)
#define SPINDLE_ENABLE_CW   PL_COND_FLAG_SPINDLE_CW // M3 (NOTE: Uses planner condition bit flag)
#define SPINDLE_ENABLE_CCW  PL_COND_FLAG_SPINDLE_CCW // M4 (NOTE: Uses planner condition bit flag)

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == hSlaveI2C->Instance) {
		// hi2c->pBuffPtr
		if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(1);
		} else if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(2);
		} else if (SPINDLE_DISABLE == cmdBuffer.data[0]) {
			vShowStatus(3);
		}
		HAL_I2C_Slave_Receive_IT(hSlaveI2C, (uint8_t*)(&cmdBuffer), sizeof(grbl_ext_cmd_t));
	}
}
