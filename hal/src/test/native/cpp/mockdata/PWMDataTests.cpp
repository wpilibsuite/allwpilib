/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/PWM.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/PWMData.h"

namespace hal {

std::string gTestPwmCallbackName;
HAL_Value gTestPwmCallbackValue;

void TestPwmInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestPwmCallbackName = name;
  gTestPwmCallbackValue = *value;
}

TEST(PWMSimTests, TestPwmInitialization) {
  const int INDEX_TO_TEST = 7;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPWMInitializedCallback(
      INDEX_TO_TEST, &TestPwmInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle pwmHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pwmHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestPwmCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pwmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPwmCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPwmCallbackName = "Unset";
  pwmHandle = HAL_InitializePWMPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pwmHandle);
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
  pwmHandle = HAL_InitializePWMPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pwmHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPwmCallbackName.c_str());
}
}  // namespace hal
