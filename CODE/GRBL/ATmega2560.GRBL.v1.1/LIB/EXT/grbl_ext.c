/*
 * grbl_ext.c
 *
 * Created: 13.06.2018 18:54:57
 *  Author: Alexey N. Zhukov
 */ 

#include <string.h>

#include "grbl_ext.h"
#include "IIC_ultimate.h"
#include "nuts_bolts.h"

#define GRBL_EXT_I2C_SLAVE_ADDRESS  0x10
#define GRBL_EXT_CMD_MAX_SIZE       (I2C_BUFFER_SIZE - 2)
#define GRBL_EXT_CMD_SPINDLE        0x01

typedef struct _grbl_ext_cmd_t {
    uint8_t cmd;
    uint8_t size;
    uint8_t data[GRBL_EXT_CMD_MAX_SIZE];
} grbl_ext_cmd_t;

uint8_t grblExtRetries = 0;

void OnSendComplete() {
    // Освобождаем шину
    I2C_State &= I2C_FREE;
}

void OnSendError() {
    // Освобождаем шину
    I2C_State &= I2C_FREE;
}

void grbl_ext_init() {
    GRBL_EXT_ERROR_DDR |= _BV(GRBL_EXT_ERROR_BIT);
    grbl_ext_error_off();

    grblExtRetries = 0;

    I2C_SlaveAddress = GRBL_EXT_I2C_SLAVE_ADDRESS;
    Init_I2C();
    I2C_MasterOutFunc = &OnSendComplete;
    I2C_ErrorOutFunc = &OnSendError;
}

inline void grbl_ext_error_on() {
    GRBL_EXT_ERROR_PORT |= (1<<GRBL_EXT_ERROR_BIT);  // Set pin to high
}

inline void grbl_ext_error_off() {
    GRBL_EXT_ERROR_PORT &= ~(1 << GRBL_EXT_ERROR_BIT); // Set pin to low
}

void grbl_ext_send(grbl_ext_cmd_t theCmd) {
    // Wait for I2C bus to be available
    bool l_boolRes = false;
    do {
        if (I2C_State & I2C_BUSY) {
            grblExtRetries++;
            delay_ms(10);
        } else {
            l_boolRes = true;
            break;
        }            
    } while (!l_boolRes || (grblExtRetries < GRBL_EXT_I2C_MAX_RETRIES));
    if (!l_boolRes) {
        grbl_ext_error_on();
        return;
    }           
    grbl_ext_error_off();
    memset(I2C_Buffer, 0, I2C_BUFFER_SIZE);
    memcpy(I2C_Buffer, &theCmd, sizeof(grbl_ext_cmd_t));
    I2C_Index = 0;
    I2C_ByteCount = sizeof(grbl_ext_cmd_t);
    I2C_State = I2C_SAWP;
    Start_I2C();
}

void grbl_ext_send_spindle_speed(uint8_t state, float rpm) {
    grbl_ext_cmd_t l_objCmd;
    memset(l_objCmd.data, 0, GRBL_EXT_CMD_MAX_SIZE);
    l_objCmd.cmd = GRBL_EXT_CMD_SPINDLE;
    l_objCmd.size = sizeof(state) + sizeof(rpm);
    // C analog of C++'s *reinterpret_cast<uint8_t*>(l_objCmd.data) = state;
    *(uint8_t*)(l_objCmd.data) = state;
    // C analog of C++'s *reinterpret_cast<uint32_t*>(l_objCmd.data + sizeof(state)) = *reinterpret_cast<uint32_t*>(&rpm);
    *(uint32_t*)(l_objCmd.data + sizeof(state)) = *(uint32_t*)(&rpm);
    grbl_ext_send(l_objCmd);
    /*
    if (SPINDLE_DISABLE == state) {
        
    } else if (SPINDLE_ENABLE_CW == state) {
        
    } else if (SPINDLE_ENABLE_CCW == state) {
    } 
    */       
}
