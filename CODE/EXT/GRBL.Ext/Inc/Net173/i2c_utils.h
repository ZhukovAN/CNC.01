/*
 * i2c_utils.h
 *
 *  Created on: 9 июн. 2018 г.
 *      Author: Alexey N. Zhukov
 */

#ifndef NET173_I2C_UTILS_H_
#define NET173_I2C_UTILS_H_

#include <stdbool.h>
#include "grbl_ext_cmd.h"

// Максимальная длина буфера данных, передаваемого посредством копирования в очередь.
// Данные объемом свыше указанного значения передаются посредством копирования в очередь указателя
// на буфер данных, выделенный в куче
#define I2C_MASTER_STATIC_BUFFER_LENGTH   8

// Вызов функции передачи данных посредством I2C реализуется в двух режимах:
// 1: Посредством непосредственной передачи значения. Режим реализован для того,
//    чтобы избежать необходимости выделения и последующего высвобождения памяти
//    кучи для данных небольшого размера
// 2: Посредством указателя. После завершения передачи память автоматически высвобождается
typedef struct {
    // Адрес I2C-устройства, в которое передаются данные
    uint16_t address;
    // Требуется ли высвобождение памяти по адресу data.d
    bool staticData;
    union {
        uint8_t s[I2C_MASTER_STATIC_BUFFER_LENGTH];
        uint8_t *d;
    } data;
    uint16_t size;
} i2cMasterMessageBuffer;

typedef grbl_ext_cmd_t i2cSlaveMessageBuffer;

void vInitI2CMaster();
void vStartI2Master();

void vInitI2CSlave();
// Запускается обработчик прерываний приема данных по I2C в режиме slave
void vStartI2CSlave();

#endif /* NET173_I2C_UTILS_H_ */
