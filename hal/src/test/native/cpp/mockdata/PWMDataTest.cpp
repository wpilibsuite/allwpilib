// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/HAL.h"
#include "hal/PWM.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/PWMData.h"

namespace hal {

std::string gTestPwmCallbackName;
HAL_Value gTestPwmCallbackValue;

void TestPwmInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestPwmCallbackName = name;
  gTestPwmCallbackValue = *value;
}

TEST(PWMSimTest, PwmInitialization) {
  const int INDEX_TO_TEST = 7;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPWMInitializedCallback(
      INDEX_TO_TEST, &TestPwmInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle pwmHandle;

  // Use out of range index
  portHandle = 8000;
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pwmHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestPwmCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pwmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPwmCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pwmHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestPwmCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetPWMData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterPWMInitializedCallback(
      INDEX_TO_TEST, &TestPwmInitializationCallback, &callbackParam, false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pwmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPwmCallbackName.c_str());
  HALSIM_CancelPWMInitializedCallback(INDEX_TO_TEST, callbackId);
}
}  // namespace hal
