// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * @defgroup hal_errors Error Defines
 * @ingroup hal_capi
 * @{
 */

#define HAL_ERR_CANSessionMux_InvalidBuffer_MESSAGE "CAN: Invalid Buffer"
#define HAL_ERR_CANSessionMux_MessageNotFound_MESSAGE "CAN: Message not found"
#define HAL_WARN_CANSessionMux_NoToken_MESSAGE "CAN: No token"
#define HAL_ERR_CANSessionMux_NotAllowed_MESSAGE "CAN: Not allowed"
#define HAL_ERR_CANSessionMux_NotInitialized_MESSAGE "CAN: Not initialized"
#define HAL_WARN_CANSessionMux_TxQueueFull_MESSAGE \
  "CAN: TX Queue full. Generally caused by a disconnected bus."
#define HAL_WARN_CANSessionMux_SocketBufferFull_MESSAGE \
  "CAN: Socket Buffer full. Generally caused by sending too many packets."

#define HAL_SUCCESS 0

#define HAL_VOLTAGE_OUT_OF_RANGE 1002
#define HAL_VOLTAGE_OUT_OF_RANGE_MESSAGE \
  "HAL: Voltage to convert to raw value is out of range [0; 5]"
#define HAL_INCOMPATIBLE_STATE 1015
#define HAL_INCOMPATIBLE_STATE_MESSAGE \
  "HAL: Incompatible State: The operation cannot be completed"
#define HAL_NO_AVAILABLE_RESOURCES -1004
#define HAL_NO_AVAILABLE_RESOURCES_MESSAGE \
  "HAL: No available resources to allocate"
#define HAL_NULL_PARAMETER -1005
#define HAL_NULL_PARAMETER_MESSAGE \
  "HAL: A pointer parameter to a method is NULL"
#define HAL_PARAMETER_OUT_OF_RANGE -1028
#define HAL_PARAMETER_OUT_OF_RANGE_MESSAGE "HAL: A parameter is out of range."
#define HAL_RESOURCE_IS_ALLOCATED -1029
#define HAL_RESOURCE_IS_ALLOCATED_MESSAGE "HAL: Resource already allocated"
#define HAL_RESOURCE_OUT_OF_RANGE -1030
#define HAL_RESOURCE_OUT_OF_RANGE_MESSAGE \
  "HAL: The requested resource is out of range."
#define HAL_COUNTER_NOT_SUPPORTED -1058
#define HAL_COUNTER_NOT_SUPPORTED_MESSAGE \
  "HAL: Counter mode not supported for encoder method"
#define HAL_HANDLE_ERROR -1098
#define HAL_HANDLE_ERROR_MESSAGE \
  "HAL: A handle parameter was passed incorrectly"

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

#define HAL_CAN_BUFFER_OVERRUN -35007
#define HAL_CAN_BUFFER_OVERRUN_MESSAGE \
  "HAL: CAN Output Buffer Full. Ensure a device is attached"

/** @} */
