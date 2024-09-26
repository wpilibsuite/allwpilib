// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/CTREPCM.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/CTREPCMData.h"

namespace hal {

std::string gTestSolenoidCallbackName;
HAL_Value gTestSolenoidCallbackValue;

void TestSolenoidInitializationCallback(const char* name, void* param,
                                        const struct HAL_Value* value) {
  gTestSolenoidCallbackName = name;
  gTestSolenoidCallbackValue = *value;
}

TEST(PCMDataTest, PCMInitialization) {
  const int MODULE_TO_TEST = 2;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterCTREPCMInitializedCallback(
      MODULE_TO_TEST, &TestSolenoidInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;
  int32_t module;
  HAL_CTREPCMHandle pcmHandle;

  // Use out of range index
  module = 8000;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(module, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pcmHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestSolenoidCallbackName.c_str());

  // Successful setup
  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(module, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pcmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestSolenoidCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(module, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pcmHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestSolenoidCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetCTREPCMData(MODULE_TO_TEST);
  callbackId = HALSIM_RegisterCTREPCMInitializedCallback(
      MODULE_TO_TEST, &TestSolenoidInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  status = 0;
  module = MODULE_TO_TEST;
  gTestSolenoidCallbackName = "Unset";
  pcmHandle = HAL_InitializeCTREPCM(module, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pcmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestSolenoidCallbackName.c_str());
}

}  // namespace hal
