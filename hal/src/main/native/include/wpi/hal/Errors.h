// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * @defgroup hal_errors Error Defines
 * @ingroup hal_capi
 * @{
 */

#define CTR_RxTimeout_MESSAGE "CTRE CAN Receive Timeout"
#define CTR_TxTimeout_MESSAGE "CTRE CAN Transmit Timeout"
#define CTR_InvalidParamValue_MESSAGE "CTRE CAN Invalid Parameter"
#define CTR_UnexpectedArbId_MESSAGE \
  "CTRE Unexpected Arbitration ID (CAN Node ID)"
#define CTR_TxFailed_MESSAGE "CTRE CAN Transmit Error"
#define CTR_SigNotUpdated_MESSAGE "CTRE CAN Signal Not Updated"

#define NiFpga_Status_FifoTimeout_MESSAGE "NIFPGA: FIFO timeout error"
#define NiFpga_Status_TransferAborted_MESSAGE "NIFPGA: Transfer aborted error"
#define NiFpga_Status_MemoryFull_MESSAGE \
  "NIFPGA: Memory Allocation failed, memory full"
#define NiFpga_Status_SoftwareFault_MESSAGE "NIFPGA: Unexpected software error"
#define NiFpga_Status_InvalidParameter_MESSAGE "NIFPGA: Invalid Parameter"
#define NiFpga_Status_ResourceNotFound_MESSAGE "NIFPGA: Resource not found"
#define NiFpga_Status_ResourceNotInitialized_MESSAGE \
  "NIFPGA: Resource not initialized"
#define NiFpga_Status_HardwareFault_MESSAGE "NIFPGA: Hardware Fault"
#define NiFpga_Status_IrqTimeout_MESSAGE "NIFPGA: Interrupt timeout"

#define ERR_CANSessionMux_InvalidBuffer_MESSAGE "CAN: Invalid Buffer"
#define ERR_CANSessionMux_MessageNotFound_MESSAGE "CAN: Message not found"
#define WARN_CANSessionMux_NoToken_MESSAGE "CAN: No token"
#define ERR_CANSessionMux_NotAllowed_MESSAGE "CAN: Not allowed"
#define ERR_CANSessionMux_NotInitialized_MESSAGE "CAN: Not initialized"
#define HAL_WARN_CANSessionMux_TxQueueFull_MESSAGE \
  "CAN: TX Queue full. Generally caused by a disconnected bus."
#define HAL_WARN_CANSessionMux_SocketBufferFull_MESSAGE \
  "CAN: Socket Buffer full. Generally caused by sending too many packets."

#define ERR_FRCSystem_NetCommNotResponding_MESSAGE \
  "FRCSystem: NetComm not responding"
#define ERR_FRCSystem_NoDSConnection_MESSAGE \
  "FRCSystem: No driver station connected"

#define HAL_SUCCESS 0

#define SAMPLE_RATE_TOO_HIGH 1001
#define SAMPLE_RATE_TOO_HIGH_MESSAGE \
  "HAL: Analog module sample rate is too high"
#define VOLTAGE_OUT_OF_RANGE 1002
#define VOLTAGE_OUT_OF_RANGE_MESSAGE \
  "HAL: Voltage to convert to raw value is out of range [0; 5]"
#define LOOP_TIMING_ERROR 1004
#define LOOP_TIMING_ERROR_MESSAGE \
  "HAL: Digital module loop timing is not the expected value"
#define SPI_WRITE_NO_MOSI 1012
#define SPI_WRITE_NO_MOSI_MESSAGE \
  "HAL: Cannot write to SPI port with no MOSI output"
#define SPI_READ_NO_MISO 1013
#define SPI_READ_NO_MISO_MESSAGE \
  "HAL: Cannot read from SPI port with no MISO input"
#define SPI_READ_NO_DATA 1014
#define SPI_READ_NO_DATA_MESSAGE "HAL: No data available to read from SPI"
#define INCOMPATIBLE_STATE 1015
#define INCOMPATIBLE_STATE_MESSAGE \
  "HAL: Incompatible State: The operation cannot be completed"
#define NO_AVAILABLE_RESOURCES -1004
#define NO_AVAILABLE_RESOURCES_MESSAGE "HAL: No available resources to allocate"
#define NULL_PARAMETER -1005
#define NULL_PARAMETER_MESSAGE "HAL: A pointer parameter to a method is NULL"
#define ANALOG_TRIGGER_LIMIT_ORDER_ERROR -1010
#define ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE \
  "HAL: AnalogTrigger limits error.  Lower limit > Upper Limit"
#define ANALOG_TRIGGER_PULSE_OUTPUT_ERROR -1011
#define ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE \
  "HAL: Attempted to read AnalogTrigger pulse output."
#define PARAMETER_OUT_OF_RANGE -1028
#define PARAMETER_OUT_OF_RANGE_MESSAGE "HAL: A parameter is out of range."
#define RESOURCE_IS_ALLOCATED -1029
#define RESOURCE_IS_ALLOCATED_MESSAGE "HAL: Resource already allocated"
#define RESOURCE_OUT_OF_RANGE -1030
#define RESOURCE_OUT_OF_RANGE_MESSAGE \
  "HAL: The requested resource is out of range."
#define HAL_INVALID_ACCUMULATOR_CHANNEL -1035
#define HAL_INVALID_ACCUMULATOR_CHANNEL_MESSAGE \
  "HAL: The requested input is not an accumulator channel"
#define HAL_COUNTER_NOT_SUPPORTED -1058
#define HAL_COUNTER_NOT_SUPPORTED_MESSAGE \
  "HAL: Counter mode not supported for encoder method"
#define HAL_PWM_SCALE_ERROR -1072
#define HAL_PWM_SCALE_ERROR_MESSAGE \
  "HAL: The PWM Scale Factors are out of range"
#define HAL_HANDLE_ERROR -1098
#define HAL_HANDLE_ERROR_MESSAGE \
  "HAL: A handle parameter was passed incorrectly"

#define HAL_LED_CHANNEL_ERROR -1099
#define HAL_LED_CHANNEL_ERROR_MESSAGE \
  "HAL: Addressable LEDs only supported on PWM Headers, not MXP or DIO"

#define HAL_INVALID_DMA_ADDITION -1102
#define HAL_INVALID_DMA_ADDITION_MESSAGE \
  "HAL_AddDMA() only works before HAL_StartDMA()"

#define HAL_INVALID_DMA_STATE -1103
#define HAL_INVALID_DMA_STATE_MESSAGE \
  "HAL_SetPause() only works before HAL_StartDMA()"

#define HAL_SERIAL_PORT_NOT_FOUND -1123
#define HAL_SERIAL_PORT_NOT_FOUND_MESSAGE \
  "HAL: The specified serial port device was not found"

#define HAL_SERIAL_PORT_OPEN_ERROR -1124
#define HAL_SERIAL_PORT_OPEN_ERROR_MESSAGE \
  "HAL: The serial port could not be opened"

#define HAL_SERIAL_PORT_ERROR -1125
#define HAL_SERIAL_PORT_ERROR_MESSAGE \
  "HAL: There was an error on the serial port"

#define HAL_THREAD_PRIORITY_ERROR -1152
#define HAL_THREAD_PRIORITY_ERROR_MESSAGE \
  "HAL: Getting or setting the priority of a thread has failed"

#define HAL_THREAD_PRIORITY_RANGE_ERROR -1153
#define HAL_THREAD_PRIORITY_RANGE_ERROR_MESSAGE \
  "HAL: The priority requested to be set is invalid"

#define HAL_CAN_TIMEOUT -1154
#define HAL_CAN_TIMEOUT_MESSAGE "HAL: CAN Receive has Timed Out"

#define HAL_SIM_NOT_SUPPORTED -1155
#define HAL_SIM_NOT_SUPPORTED_MESSAGE "HAL: Method not supported in sim"

#define HAL_USE_LAST_ERROR -1156
#define HAL_USE_LAST_ERROR_MESSAGE \
  "HAL: Use HAL_GetLastError(status) to get last error"

#define HAL_CONSOLE_OUT_ENABLED_ERROR -1157
#define HAL_CONSOLE_OUT_ENABLED_ERROR_MESSAGE                           \
  "HAL: Onboard serial port is requested, but Console Out is enabled. " \
  "Disable Console Out using imaging tool"

#define HAL_CAN_BUFFER_OVERRUN -35007
#define HAL_CAN_BUFFER_OVERRUN_MESSAGE \
  "HAL: CAN Output Buffer Full. Ensure a device is attached"

#define VI_ERROR_SYSTEM_ERROR_MESSAGE "HAL - VISA: System Error"
#define VI_ERROR_INV_OBJECT_MESSAGE "HAL - VISA: Invalid Object"
#define VI_ERROR_RSRC_LOCKED_MESSAGE "HAL - VISA: Resource Locked"
#define VI_ERROR_RSRC_NFOUND_MESSAGE "HAL - VISA: Resource Not Found"
#define VI_ERROR_INV_RSRC_NAME_MESSAGE "HAL - VISA: Invalid Resource Name"
#define VI_ERROR_QUEUE_OVERFLOW_MESSAGE "HAL - VISA: Queue Overflow"
#define VI_ERROR_IO_MESSAGE "HAL - VISA: General IO Error"
#define VI_ERROR_ASRL_PARITY_MESSAGE "HAL - VISA: Parity Error"
#define VI_ERROR_ASRL_FRAMING_MESSAGE "HAL - VISA: Framing Error"
#define VI_ERROR_ASRL_OVERRUN_MESSAGE "HAL - VISA: Buffer Overrun Error"
#define VI_ERROR_RSRC_BUSY_MESSAGE "HAL - VISA: Resource Busy"
#define VI_ERROR_INV_PARAMETER_MESSAGE "HAL - VISA: Invalid Parameter"
/** @} */
