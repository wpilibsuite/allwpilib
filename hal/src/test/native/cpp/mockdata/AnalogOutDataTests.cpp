/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/AnalogOutput.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/AnalogOutData.h"

namespace hal {

std::string gTestAnalogOutCallbackName;
HAL_Value gTestAnalogOutCallbackValue;

void TestAnalogOutInitializationCallback(const char* name, void* param,
                                         const struct HAL_Value* value) {
  gTestAnalogOutCallbackName = name;
  gTestAnalogOutCallbackValue = *value;
}

TEST(AnalogOutSimTests, TestAnalogOutInitialization) {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterAnalogOutInitializedCallback(
      INDEX_TO_TEST, &TestAnalogOutInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle analogOutHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle = HAL_InitializeAnalogOutputPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogOutHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestAnalogOutCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle = HAL_InitializeAnalogOutputPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogOutHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogOutCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestAnalogOutCallbackName = "Unset";
  analogOutHandle = HAL_InitializeAnalogOutputPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, analogOutHandle);
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
  analogOutHandle = HAL_InitializeAnalogOutputPort(portHandle, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != analogOutHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestAnalogOutCallbackName.c_str());
}
}  // namespace hal
