/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/AnalogInput.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/AnalogInData.h"

namespace hal {

std::string gTestAnalogInCallbackName;
HAL_Value gTestAnalogInCallbackValue;

void TestAnalogInInitializationCallback(const char* name, void* param,
                                        const struct HAL_Value* value) {
  gTestAnalogInCallbackName = name;
  gTestAnalogInCallbackValue = *value;
}

TEST(AnalogInSimTests, TestAnalogInInitialization) {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterAnalogInInitializedCallback(
      INDEX_TO_TEST, &TestAnalogInInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle analogInHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogInHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestAnalogInCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogInHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogInCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogInHandle);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestAnalogInCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetAnalogInData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterAnalogInInitializedCallback(
      INDEX_TO_TEST, &TestAnalogInInitializationCallback, &callbackParam,
      false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogInCallbackName = "Unset";
  analogInHandle = HAL_InitializeAnalogInputPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogInHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogInCallbackName.c_str());
}
}  // namespace hal
