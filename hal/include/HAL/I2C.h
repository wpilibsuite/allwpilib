#pragma once
#include <stdint.h>

extern "C" {
void i2CInitialize(uint8_t port, int32_t* status);
int32_t i2CTransaction(uint8_t port, uint8_t deviceAddress, uint8_t* dataToSend,
                       uint8_t sendSize, uint8_t* dataReceived,
                       uint8_t receiveSize);
int32_t i2CWrite(uint8_t port, uint8_t deviceAddress, uint8_t* dataToSend,
                 uint8_t sendSize);
int32_t i2CRead(uint8_t port, uint8_t deviceAddress, uint8_t* buffer,
                uint8_t count);
void i2CClose(uint8_t port);
}
