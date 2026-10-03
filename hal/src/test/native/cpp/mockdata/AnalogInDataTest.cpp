// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/AnalogInData.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/handles/HandlesInternal.hpp"

namespace wpi::hal {

std::string gTestAnalogInCallbackName;
HAL_Value gTestAnalogInCallbackValue;

void TestAnalogInInitializationCallback(const char* name, void* param,
                                        const struct HAL_Value* value) {
  gTestAnalogInCallbackName = name;
  gTestAnalogInCallbackValue = *value;
}

TEST_CASE("AnalogInSimTest AnalogInInitialization", "[hal][mockdata]") {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterAnalogInInitializedCallback(
      INDEX_TO_TEST, &TestAnalogInInitializationCallback, &callbackParam,
      false);
  REQUIRE(0 != callbackId);

  int32_t status = 0;
  int32_t channel = 0;
  HAL_DigitalHandle analogInHandle;

  // Use out of range index
  channel = 8000;
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(channel, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE == analogInHandle);
  CHECK(HAL_USE_LAST_ERROR == status);
  HAL_GetLastError(&status);
  CHECK(HAL_RESOURCE_OUT_OF_RANGE == status);
  CHECK("Unset" == gTestAnalogInCallbackName);

  // Successful setup
  status = 0;
  channel = INDEX_TO_TEST;
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(channel, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE != analogInHandle);
  CHECK(0 == status);
  CHECK("Initialized" == gTestAnalogInCallbackName);

  // Double initialize... should fail
  status = 0;
  channel = INDEX_TO_TEST;
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(channel, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE == analogInHandle);
  CHECK(HAL_USE_LAST_ERROR == status);
  HAL_GetLastError(&status);
  CHECK(HAL_RESOURCE_IS_ALLOCATED == status);
  CHECK("Unset" == gTestAnalogInCallbackName);

  // Reset, should allow you to re-register
  wpi::hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetAnalogInData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterAnalogInInitializedCallback(
      INDEX_TO_TEST, &TestAnalogInInitializationCallback, &callbackParam,
      false);

  status = 0;
  channel = INDEX_TO_TEST;
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(channel, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE != analogInHandle);
  CHECK(0 == status);
  CHECK("Initialized" == gTestAnalogInCallbackName);
  HALSIM_CancelAnalogInInitializedCallback(INDEX_TO_TEST, callbackId);
}
}  // namespace wpi::hal
