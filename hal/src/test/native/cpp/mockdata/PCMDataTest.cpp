// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/CTREPCM.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/hal/simulation/CTREPCMData.h"

namespace wpi::hal {

std::string gTestSolenoidCallbackName;
HAL_Value gTestSolenoidCallbackValue;

void TestSolenoidInitializationCallback(const char* name, void* param,
                                        const struct HAL_Value* value) {
  gTestSolenoidCallbackName = name;
  gTestSolenoidCallbackValue = *value;
}

TEST_CASE("PCMDataTest PCMInitialization", "[hal][mockdata]") {
  const int MODULE_TO_TEST = 2;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterCTREPCMInitializedCallback(
      MODULE_TO_TEST, &TestSolenoidInitializationCallback, &callbackParam,
      false);
  REQUIRE(0 != callbackId);

  int32_t status = 0;
  int32_t module;
  HAL_CTREPCMHandle pcmHandle;

  // Use out of range index
  module = 8000;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(0, module, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE == pcmHandle);
  CHECK(HAL_USE_LAST_ERROR == status);
  HAL_GetLastError(&status);
  CHECK(HAL_RESOURCE_OUT_OF_RANGE == status);
  CHECK("Unset" == gTestSolenoidCallbackName);

  // Successful setup
  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(0, module, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE != pcmHandle);
  CHECK(0 == status);
  CHECK("Initialized" == gTestSolenoidCallbackName);

  // Double initialize... should fail
  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(0, module, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE == pcmHandle);
  CHECK(HAL_USE_LAST_ERROR == status);
  HAL_GetLastError(&status);
  CHECK(HAL_RESOURCE_IS_ALLOCATED == status);
  CHECK("Unset" == gTestSolenoidCallbackName);

  // Reset, should allow you to re-register
  wpi::hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetCTREPCMData(MODULE_TO_TEST);
  callbackId = HALSIM_RegisterCTREPCMInitializedCallback(
      MODULE_TO_TEST, &TestSolenoidInitializationCallback, &callbackParam,
      false);
  REQUIRE(0 != callbackId);

  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(0, module, nullptr, &status);
  CHECK(HAL_INVALID_HANDLE != pcmHandle);
  CHECK(0 == status);
  CHECK("Initialized" == gTestSolenoidCallbackName);
}

}  // namespace wpi::hal
