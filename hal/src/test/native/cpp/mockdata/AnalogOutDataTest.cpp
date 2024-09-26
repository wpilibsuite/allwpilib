// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/AnalogOutput.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/AnalogOutData.h"

namespace hal {

std::string gTestAnalogOutCallbackName;
HAL_Value gTestAnalogOutCallbackValue;

void TestAnalogOutInitializationCallback(const char* name, void* param,
                                         const struct HAL_Value* value) {
  gTestAnalogOutCallbackName = name;
  gTestAnalogOutCallbackValue = *value;
}

TEST(AnalogOutSimTest, AnalogOutInitialization) {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterAnalogOutInitializedCallback(
      INDEX_TO_TEST, &TestAnalogOutInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle analogOutHandle;

  // Use out of range index
  portHandle = 8000;
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle =
      HAL_InitializeAnalogOutputPort(portHandle, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogOutHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestAnalogOutCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle =
      HAL_InitializeAnalogOutputPort(portHandle, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogOutHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogOutCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle =
      HAL_InitializeAnalogOutputPort(portHandle, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogOutHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestAnalogOutCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetAnalogOutData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterAnalogOutInitializedCallback(
      INDEX_TO_TEST, &TestAnalogOutInitializationCallback, &callbackParam,
      false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle =
      HAL_InitializeAnalogOutputPort(portHandle, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogOutHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogOutCallbackName.c_str());
  HALSIM_CancelAnalogOutInitializedCallback(INDEX_TO_TEST, callbackId);
}
}  // namespace hal
