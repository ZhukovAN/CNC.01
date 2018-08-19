/*
 * i2c_utils.h
 *
 *  Created on: 9 ���. 2018 �.
 *      Author: Alexey N. Zhukov
 */

#ifndef NET173_I2C_UTILS_H_
#define NET173_I2C_UTILS_H_

#include <stdbool.h>
#include "grbl_ext_cmd.h"

// ������������ ����� ������ ������, ������������� ����������� ����������� � �������.
// ������ ������� ����� ���������� �������� ���������� ����������� ����������� � ������� ���������
// �� ����� ������, ���������� � ����
#define I2C_MASTER_STATIC_BUFFER_LENGTH   8

// ����� ������� �������� ������ ����������� I2C ����������� � ���� �������:
// 1: ����������� ���������������� �������� ��������. ����� ���������� ��� ����,
//    ����� �������� ������������� ��������� � ������������ ������������� ������
//    ���� ��� ������ ���������� �������
// 2: ����������� ���������. ����� ���������� �������� ������ ������������� ��������������
typedef struct {
    // ����� I2C-����������, � ������� ���������� ������
    uint16_t address;
    // ��������� �� ������������� ������ �� ������ data.d
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
// ����������� ���������� ���������� ������ ������ �� I2C � ������ slave
void vStartI2CSlave();

#endif /* NET173_I2C_UTILS_H_ */
