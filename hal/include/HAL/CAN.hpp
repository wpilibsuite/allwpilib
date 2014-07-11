#pragma once

#include <stdint.h>
#include "NetworkCommunication/CANSessionMux.h"

void canTxSend(uint32_t arbID, uint8_t length, int32_t period = CAN_SEND_PERIOD_NO_REPEAT);

void canTxPackInt8 (uint32_t arbID, uint8_t offset, uint8_t  value);
void canTxPackInt16(uint32_t arbID, uint8_t offset, uint16_t value);
void canTxPackInt32(uint32_t arbID, uint8_t offset, uint32_t value);
void canTxPackFXP16(uint32_t arbID, uint8_t offset, double   value);
void canTxPackFXP32(uint32_t arbID, uint8_t offset, double   value);

uint8_t  canTxUnpackInt8 (uint32_t arbID, uint8_t offset);
uint32_t canTxUnpackInt32(uint32_t arbID, uint8_t offset);
uint16_t canTxUnpackInt16(uint32_t arbID, uint8_t offset);
double   canTxUnpackFXP16(uint32_t arbID, uint8_t offset);
double   canTxUnpackFXP32(uint32_t arbID, uint8_t offset);

bool canRxReceive(uint32_t arbID);

uint8_t  canRxUnpackInt8 (uint32_t arbID, uint8_t offset);
uint16_t canRxUnpackInt16(uint32_t arbID, uint8_t offset);
uint32_t canRxUnpackInt32(uint32_t arbID, uint8_t offset);
double   canRxUnpackFXP16(uint32_t arbID, uint8_t offset);
double   canRxUnpackFXP32(uint32_t arbID, uint8_t offset);
