// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/CAN.h"
#include "wpi/hal/Errors.h"

namespace wpi {

/** REVLib-compatible error categories returned by high-level A301 commands. */
enum class A301Error {
  /** No error occurred. */
  kOk = 0,
  /** A general controller error occurred. */
  kError = 1,
  /** The operation timed out. */
  kTimeout = 2,
  /** The requested operation is not implemented. */
  kNotImplemented = 3,
  /** The HAL returned an error. */
  kHALError = 4,
  /** The controller firmware could not be read. */
  kCantFindFirmware = 5,
  /** The controller firmware is too old. */
  kFirmwareTooOld = 6,
  /** The controller firmware is newer than this driver supports. */
  kFirmwareTooNew = 7,
  /** The parameter ID is invalid. */
  kParamInvalidID = 8,
  /** The parameter type does not match the expected type. */
  kParamMismatchType = 9,
  /** The parameter does not support the requested access mode. */
  kParamAccessMode = 10,
  /** The parameter value is invalid. */
  kParamInvalid = 11,
  /** The parameter is deprecated or not implemented. */
  kParamNotImplementedDeprecated = 12,
  /** The follower configuration does not match its leader. */
  kFollowConfigMismatch = 13,
  /** The returned error value is invalid. */
  kInvalid = 14,
  /** The requested setpoint is outside the accepted range. */
  kSetpointOutOfRange = 15,
  /** An unknown controller error occurred. */
  kUnknown = 16,
  /** The controller is disconnected from the CAN bus. */
  kCANDisconnected = 17,
  /** Another device is using the requested CAN ID. */
  kDuplicateCANId = 18,
  /** The requested CAN ID is invalid. */
  kInvalidCANId = 19,
};

namespace detail {

/**
 * Converts a HAL status code to its REVLib-compatible A301 error category.
 *
 * @param status HAL status code
 * @return corresponding A301 error category
 */
constexpr A301Error A301ErrorFromHalStatus(int32_t status) {
  switch (status) {
    case 0:
      return A301Error::kOk;
    case HAL_CAN_TIMEOUT:
    case HAL_ERR_CANSessionMux_MessageNotFound:
      return A301Error::kTimeout;
    case HAL_PARAMETER_OUT_OF_RANGE:
      return A301Error::kParamInvalid;
    case HAL_RESOURCE_IS_ALLOCATED:
      return A301Error::kDuplicateCANId;
    case HAL_RESOURCE_OUT_OF_RANGE:
      return A301Error::kInvalidCANId;
    case HAL_INCOMPATIBLE_STATE:
      return A301Error::kError;
    case HAL_CAN_BUFFER_OVERRUN:
      return A301Error::kCANDisconnected;
    default:
      return A301Error::kHALError;
  }
}

}  // namespace detail
}  // namespace wpi
