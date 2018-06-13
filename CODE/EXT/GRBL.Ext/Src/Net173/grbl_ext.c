/*
 * init_utils.c
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#include <FreeRTOS.h>
#include <task.h>

#include "grbl_ext.h"

#include "debug_utils.h"
#include "i2c_utils.h"

void vStart() {
	vInitI2C();
	vInitDebug();

	vStartI2C();
	vStartDebug();
}
