/*
 * grbl_ext_cmd.h
 *
 *  Created on: 18 авг. 2018 г.
 *      Author: Alexey N. Zhukov
 */

#ifndef NET173_GRBL_EXT_CMD_H_
#define NET173_GRBL_EXT_CMD_H_

#include "misc_utils.h"
#include "i2c_utils.h"

// Modal Group M7: Spindle control
#define GRBL_EXT_CMD_SPINDLE        	0x01
#define PL_COND_FLAG_SPINDLE_CW        	bit(4)
#define PL_COND_FLAG_SPINDLE_CCW       	bit(5)
// M5 (Default: Must be zero)
#define SPINDLE_DISABLE 0
// M3 (NOTE: Uses planner condition bit flag)
#define SPINDLE_ENABLE_CW   			PL_COND_FLAG_SPINDLE_CW
// M4 (NOTE: Uses planner condition bit flag)
#define SPINDLE_ENABLE_CCW  			PL_COND_FLAG_SPINDLE_CCW
// Максимальный размер буфера данных, получаемого от GRBL-мастера
#define GRBL_EXT_CMD_MAX_SIZE       	8
// Формат структуры данных, получаемой от GRBL-мастера
typedef struct _grbl_ext_cmd_t {
    uint8_t cmd;
    uint8_t size;
    uint8_t data[GRBL_EXT_CMD_MAX_SIZE];
} grbl_ext_cmd_t;
// Latest GRBL extension command received from GRBL master board
grbl_ext_cmd_t cmdBuffer;

#endif /* NET173_GRBL_EXT_CMD_H_ */
