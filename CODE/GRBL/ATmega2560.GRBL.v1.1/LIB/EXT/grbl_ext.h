/*
 * grbl_ext.h
 *
 * Created: 13.06.2018 18:54:42
 *  Author: Alexey N. Zhukov
 */ 


#ifndef GRBL_EXT_H_
#define GRBL_EXT_H_

#include <avr/io.h>
#include <stdbool.h>

#define GRBL_EXT_I2C_MAX_RETRIES   2

#define GRBL_EXT_ERROR_DDR     DDRB
#define GRBL_EXT_ERROR_PORT    PORTB
#define GRBL_EXT_ERROR_PIN     PINB
#define GRBL_EXT_ERROR_BIT   1 // MEGA2560 Digital Pin 52

void grbl_ext_init();
void grbl_ext_error_on();
void grbl_ext_error_off();

void grbl_ext_send_spindle_speed(uint8_t state, float rpm);

#endif /* GRBL_EXT_H_ */