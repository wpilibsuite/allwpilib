// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/DIO.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/DIOData.h"

namespace hal {

std::string gTestDigitalIoCallbackName;
HAL_Value gTestDigitalIoCallbackValue;

void TestDigitalIoInitializationCallback(const char* name, void* param,
                                         const struct HAL_Value* value) {
  gTestDigitalIoCallbackName = name;
  gTestDigitalIoCallbackValue = *value;
}

TEST(DigitalIoSimTest, DigitalIoInitialization) {
  const int INDEX_TO_TEST = 3;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterDIOInitializedCallback(
      INDEX_TO_TEST, &TestDigitalIoInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle digitalIoHandle;

  // Use out of range index
  portHandle = 8000;
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, digitalIoHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestDigitalIoCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != digitalIoHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestDigitalIoCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, digitalIoHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestDigitalIoCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetDIOData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterDIOInitializedCallback(
      INDEX_TO_TEST, &TestDigitalIoInitializationCallback, &callbackParam,
      false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != digitalIoHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestDigitalIoCallbackName.c_str());
  HALSIM_CancelDIOInitializedCallback(INDEX_TO_TEST, callbackId);
}

}  // namespace hal
