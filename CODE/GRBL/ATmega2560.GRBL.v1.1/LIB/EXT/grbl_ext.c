/*
 * grbl_ext.c
 *
 * Created: 13.06.2018 18:54:57
 *  Author: Alexey N. Zhukov
 */ 

#include "grbl_ext.h"
#include "IIC_ultimate.h"

#define GRBL_EXT_I2C_ADDRESS  0x10
#define GRBL_EXT_CMD_MAX_SIZE (I2C_BUFFER_SIZE - 2)
#define GRBL_EXT_CMD_SPINDLE  0x01

typedef struct _grbl_ext_cmd_t {
    uint8_t cmd;
    uint8_t size;
    uint8_t data[GRBL_EXT_CMD_MAX_SIZE];
} grbl_ext_cmd_t;

uint8_t grblExtRetries = 0;

void grbl_ext_init() {
    GRBL_EXT_ERROR_DDR |= _BV(GRBL_EXT_ERROR_BIT);
    grblExtRetries = 0;
    grbl_ext_error_off();
    Init_I2C();
    I2C_SlaveAddress = GRBL_EXT_I2C_ADDRESS;
}

inline void grbl_ext_error_on() {
    GRBL_EXT_ERROR_PORT |= (1<<GRBL_EXT_ERROR_BIT);  // Set pin to high
}

inline void grbl_ext_error_off() {
    GRBL_EXT_ERROR_PORT &= ~(1 << GRBL_EXT_ERROR_BIT); // Set pin to low
}

void OnSendComplete() {
    // Освобождаем шину
    I2C_State &= I2C_FREE;
}

void OnSendError() {
    // Освобождаем шину
    I2C_State &= I2C_FREE;
}

void spindle_sync_ext(uint8_t state, float rpm) {
    bool l_boolRes = false;
    do {
        do {
            if (I2C_State & I2C_BUSY) break;
            
            grbl_ext_cmd_t l_objCmd;
            memset(l_objCmd.data, 0, GRBL_EXT_CMD_MAX_SIZE);
            l_objCmd.cmd = GRBL_EXT_CMD_SPINDLE;
            l_objCmd.size = sizeof(state) + sizeof(rpm);
            // C analog of C++'s *reinterpret_cast<uint8_t*>(l_objCmd.data) = state;
            *(uint8_t*)(l_objCmd.data) = state;
            // C analog of C++'s *reinterpret_cast<uint32_t*>(l_objCmd.data + sizeof(state)) = *reinterpret_cast<uint32_t*>(&rpm);
            *(uint32_t*)(l_objCmd.data + sizeof(state)) = *(uint32_t*)(&rpm);
            
            I2C_Index = 0;
            I2C_ByteCount = sizeof(grbl_ext_cmd_t);
            I2C_State = I2C_SAWP;
	        MasterOutFunc = &OnSendComplete;
	        ErrorOutFunc = &OnSendError;
            
            Start_I2C();
            
            
            
        } while (true);        
        if (l_boolRes) {
            grblExtRetries = 0;
            grbl_ext_error_off();
            break;
        } else {
            grblExtRetries++;
            if (grblExtRetries >= GRBL_EXT_MAX_RETRIES) {
                grbl_ext_error_on();
                break;
            }
        }                        
        delay_ms(10);
    } while (true);
    
    /*
    if (SPINDLE_DISABLE == state) {
        
    } else if (SPINDLE_ENABLE_CW == state) {
        
    } else if (SPINDLE_ENABLE_CCW == state) {
    } 
    */       
}
