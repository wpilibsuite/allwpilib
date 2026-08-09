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
  kOk = 0,
  kError = 1,
  kTimeout = 2,
  kNotImplemented = 3,
  kHALError = 4,
  kCantFindFirmware = 5,
  kFirmwareTooOld = 6,
  kFirmwareTooNew = 7,
  kParamInvalidID = 8,
  kParamMismatchType = 9,
  kParamAccessMode = 10,
  kParamInvalid = 11,
  kParamNotImplementedDeprecated = 12,
  kFollowConfigMismatch = 13,
  kInvalid = 14,
  kSetpointOutOfRange = 15,
  kUnknown = 16,
  kCANDisconnected = 17,
  kDuplicateCANId = 18,
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
