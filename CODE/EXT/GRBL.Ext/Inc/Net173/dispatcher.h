/*
 * dispatcher.h
 *
 *  Created on: 12 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#ifndef NET173_DISPATCHER_H_
#define NET173_DISPATCHER_H_

#include <stdint.h>

typedef enum _data_source_t {I2C, UART} data_source_t;

#define MAX_DATA_SIZE 32

typedef struct _data_packet_t {
	data_source_t 	source;
	uint8_t 		size;
	uint8_t	 		data[MAX_DATA_SIZE];
} data_packet_t;

void vInitDispatcher();
void vStartDispatcher();

#endif /* NET173_DISPATCHER_H_ */
