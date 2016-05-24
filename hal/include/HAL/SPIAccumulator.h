#pragma once
#include <stdint.h>

extern "C" {
void spiInitAccumulator(uint8_t port, uint32_t period, uint32_t cmd,
                        uint8_t xfer_size, uint32_t valid_mask,
                        uint32_t valid_value, uint8_t data_shift,
                        uint8_t data_size, bool is_signed, bool big_endian,
                        int32_t* status);
void spiFreeAccumulator(uint8_t port, int32_t* status);
void spiResetAccumulator(uint8_t port, int32_t* status);
void spiSetAccumulatorCenter(uint8_t port, int32_t center, int32_t* status);
void spiSetAccumulatorDeadband(uint8_t port, int32_t deadband, int32_t* status);
int32_t spiGetAccumulatorLastValue(uint8_t port, int32_t* status);
int64_t spiGetAccumulatorValue(uint8_t port, int32_t* status);
uint32_t spiGetAccumulatorCount(uint8_t port, int32_t* status);
double spiGetAccumulatorAverage(uint8_t port, int32_t* status);
void spiGetAccumulatorOutput(uint8_t port, int64_t* value, uint32_t* count,
                             int32_t* status);
}
