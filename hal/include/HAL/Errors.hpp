#pragma once

#define SAMPLE_RATE_TOO_HIGH 1
#define SAMPLE_RATE_TOO_HIGH_MESSAGE "Analog module sample rate is too high"
#define VOLTAGE_OUT_OF_RANGE 2
#define VOLTAGE_OUT_OF_RANGE_MESSAGE "Voltage to convert to raw value is out of range [0; 5]"
#define LOOP_TIMING_ERROR 4
#define LOOP_TIMING_ERROR_MESSAGE "Digital module loop timing is not the expected value"
#define SPI_WRITE_NO_MOSI 12
#define SPI_WRITE_NO_MOSI_MESSAGE "Cannot write to SPI port with no MOSI output"
#define SPI_READ_NO_MISO 13
#define SPI_READ_NO_MISO_MESSAGE "Cannot read from SPI port with no MISO input"
#define SPI_READ_NO_DATA 14
#define SPI_READ_NO_DATA_MESSAGE "No data available to read from SPI"
#define INCOMPATIBLE_STATE 15
#define INCOMPATIBLE_STATE_MESSAGE "Incompatible State: The operation cannot be completed"
#define NO_AVAILABLE_RESOURCES -4
#define NO_AVAILABLE_RESOURCES_MESSAGE "No available resources to allocate"
#define NULL_PARAMETER -5
#define NULL_PARAMETER_MESSAGE "A pointer parameter to a method is NULL"
#define ANALOG_TRIGGER_LIMIT_ORDER_ERROR -10
#define ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE "AnalogTrigger limits error.  Lower limit > Upper Limit"
#define ANALOG_TRIGGER_PULSE_OUTPUT_ERROR -11
#define ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE "Attempted to read AnalogTrigger pulse output."
#define PARAMETER_OUT_OF_RANGE -28
#define PARAMETER_OUT_OF_RANGE_MESSAGE "A parameter is out of range."
