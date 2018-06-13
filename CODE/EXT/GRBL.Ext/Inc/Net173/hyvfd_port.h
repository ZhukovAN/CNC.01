/*
 * hyvfd_port.h
 *
 *  Created on: 12 θών. 2018 γ.
 *      Author: Alexey N. Zhukov
 */

#ifndef NET173_HYVFD_PORT_H_
#define NET173_HYVFD_PORT_H_

#include <stdint.h>

// Ported from linuxcnc-master\src\hal\user_comps\huanyang-vfd

/* Local */
#define COMM_TIME_OUT           -0x0C
#define PORT_SOCKET_FAILURE     -0x0D
#define SELECT_FAILURE          -0x0E
#define TOO_MANY_DATAS          -0x0F
#define INVALID_CRC             -0x10
#define INVALID_EXCEPTION_CODE  -0x11

// Huanyang Function Codes
#define	FUNCTION_READ			0x01
#define	FUNCTION_WRITE			0x02
#define	WRITE_CONTROL_DATA		0x03
#define	READ_CONTROL_STATUS		0x04
#define	WRITE_FREQ_DATA			0x05
#define	LOOP_TEST				0x08

typedef struct _hycomm_data_t {
	unsigned char slave;			/* slave address */
	unsigned char function;			/* function code */
	unsigned char parameter;		/* PDxxx parameter */
	int data;						/* Data to send */
	unsigned char ret_length;		/* length of data returned from slave */
	unsigned char ret_parameter;	/* parameter returned from slave */
	int ret_data;					/* Data returned from slave */
} hycomm_data_t;

#endif /* NET173_HYVFD_PORT_H_ */
