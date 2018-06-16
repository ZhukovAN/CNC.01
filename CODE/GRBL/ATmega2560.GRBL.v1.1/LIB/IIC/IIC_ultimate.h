#ifndef IICULTIMATE_H
#define IICULTIMATE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stddef.h>

#define I2C_PORT	PORTD				// Порт где сидит нога TWI
#define I2C_DDR		DDRD
#define I2C_SCL		0					// Биты соответствующих выводов
#define I2C_SDA		1

#ifdef I2C_MASTER
#define I2C_I_AM_SLAVE		0			// Если мы еще и слейвом работаем то 1. А то не услышит!
#define I2C_BUFFER_SIZE		8			// Величина буфера Master режима RX-TX. Зависит от того какой длины строки мы будем гонять
#define I2C_MAX_PAGE_ADDR_SIZE	2			// Максимальная величина адреса страницы. Обычно адрес страницы это один или два байта.
                                        // Зависит от типа ЕЕПРОМ или другой микросхемы.
#endif

#ifdef I2C_SLAVE
#define I2C_ADDRESS 	0x32	    	// Адрес на который будем отзываться
#define I2C_I_AM_SLAVE		1			// Если мы еще и слейвом работаем то 1. А то не услышит!
#define I2C_BUFFER_RX_SIZE	1			// Величина принимающего буфера режима Slave, т.е. сколько байт жрем.
#define I2C_BUFFER_TX_SIZE	1			// Величина Передающего буфера режима Slave , т.е. сколько байт отдаем за сессию.
#endif


#define I2C_TYPE_MSK	0b00001100		// Маска режима
#define I2C_SARP		0b00000000		// Start-Addr_R-Read-Stop  							Это режим простого чтения. Например из слейва или из епрома с текущего адреса
#define I2C_SAWP		0b00000100		// Start-Addr_W-Write-Stop 							Это режим простой записи. В том числе и запись с адресом страницы. 
#define I2C_SAWSARP		0b00001000		// Start-Addr_W-WrPageAdr-rStart-Addr_R-Read-Stop 	Это режим с предварительной записью текущего адреса страницы

#define I2C_ERR_MSK		0b00110011		// Маска кода ошибок
#define I2C_ERR_OK		0b00000000		// All Right! 						-- Все окей, передача успешна. 
#define I2C_ERR_NA		0b00010000		// Device No Answer 				-- Слейв не отвечает. Т.к. либо занят, либо его нет на линии.
#define I2C_ERR_LP		0b00100000		// Low Priority 					-- нас перехватили собственным адресом, либо мы проиграли арбитраж
#define I2C_ERR_NK		0b00000010		// Received NACK. End Transmittion. -- Был получен NACK. Бывает и так.
#define I2C_ERR_BF		0b00000001		// BUS FAIL 						-- Автобус сломался. И этим все сказано. Можно попробовать сделать переинициализацию шины

#define I2C_INTERRUPTED		0b10000000	// Transmiting Interrupted		Битмаска установки флага занятости
#define I2C_NO_INTERRUPTED 	0b01111111  // Transmiting No Interrupted	Битмаска снятия флага занятости

#define I2C_BUSY		0b01000000  	// Trans is Busy				Битмаска флага "Передатчик занят, руками не трогать". 
#define I2C_FREE		0b10111111  	// Trans is Free				Битмаска снятия флага занятости.

#ifdef I2C_MASTER
#define MACRO_i2c_WhatDo_MasterOut 	(MasterOutFunc)();		// Макросы для режимо выхода. Пока тут функция, но может быть что угодно
#endif
#ifdef I2C_SLAVE
#define MACRO_i2c_WhatDo_SlaveOut   (SlaveOutFunc)();
#endif
#define MACRO_i2c_WhatDo_ErrorOut   (ErrorOutFunc)();


typedef void (*IIC_F)(void);								// Тип указателя на функцию
// Мои доработки
// Необходимость в них возникла при написании модуля взаимодействия с HD44780 
// посредством I2C-расширителя PCF8574. Максимальная частота работы PCF8574 составляет 100kHz, 
// поэтому на передачу одного байта уходит порядка 100µS. Типовой тайминг HD44780 составляет 37µS,
// при этом на передачу одной порции данных уходит три байта (см. рис. 9 на стр. 22 описания HD44780):
// 1: Rs - low, D4-D7 - data	 
// 2: Rs - high, D4-D7 - data
// 3: Rs - low, D4-D7 - data
// Таким образом, на одну порцию данных затрачивается не менее 300µS, соответственно, вносить 
// дополнительные задержки не требуется. Это позволяет при передаче буфера отправлять данные не 
// байт за байтом (что приводит к дополнительным расходам в виде отправки I2C-адресного байта при
// каждой посылке), а подготавливать буфер и отправлять его целиком.
// Однако, решение этой задачи "в лоб" будет приводить к тому, что подготавливаемый буфер должен 
// будет содержать в шесть раз больше данных, нежели передаваемая строка: три байта на одну 
// четырехбитную посылку умножаются на два для передачи байта. Такой подход слишком расточителен,
// поэтому код DI HALT'а был доработан с тем, чтобы обеспечивать возможность вычисления передаваемых 
// значений в callback-функции, вызываемой непосредственно из кода обработчика прерывания. Указанная 
// функция принимает на вход номер передаваемого байта и возвращает значение, которое должно быть 
// отправлено
#ifdef I2C_MASTER
typedef uint8_t (*IIC_EXT)(uint16_t theIdx);
extern IIC_EXT GetDataFunc;
extern IIC_F MasterOutFunc;
#endif
#ifdef I2C_SLAVE									// Подрбрости в сишнике. 
extern IIC_F SlaveOutFunc;
#endif
extern IIC_F ErrorOutFunc;

extern uint8_t I2C_State;	

#ifdef I2C_SLAVE
extern uint8_t I2C_RxBuffer[I2C_BUFFER_RX_SIZE];
extern uint8_t I2C_TxBuffer[I2C_BUFFER_TX_SIZE];
extern uint8_t I2C_SlaveIndex;
extern void Init_Slave_I2C(IIC_F Addr);
#endif

#ifdef I2C_MASTER
extern uint8_t I2C_Buffer[I2C_BUFFER_SIZE];
extern uint8_t I2C_Index;
extern uint8_t I2C_ByteCount;

extern uint8_t I2C_SlaveAddress;
extern uint8_t I2C_PageAddress[I2C_MAX_PAGE_ADDR_SIZE];

extern uint8_t I2C_PageAddrIndex;
extern uint8_t I2C_PageAddrCount;
extern void Init_I2C(void);
extern void Start_I2C();
#endif

#endif
