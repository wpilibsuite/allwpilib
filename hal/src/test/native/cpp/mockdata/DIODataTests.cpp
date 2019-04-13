/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/DIO.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/DIOData.h"

namespace hal {

std::string gTestDigitalIoCallbackName;
HAL_Value gTestDigitalIoCallbackValue;

void TestDigitalIoInitializationCallback(const char* name, void* param,
                                         const struct HAL_Value* value) {
  gTestDigitalIoCallbackName = name;
  gTestDigitalIoCallbackValue = *value;
}

TEST(DigitalIoSimTests, TestDigitalIoInitialization) {
  const int INDEX_TO_TEST = 3;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterDIOInitializedCallback(
      INDEX_TO_TEST, &TestDigitalIoInitializationCallback, &callbackParam,
      false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle digitalIoHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, &status);
  EXPECT_EQ(HAL_kInvalidHandle, digitalIoHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestDigitalIoCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != digitalIoHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestDigitalIoCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestDigitalIoCallbackName = "Unset";
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, &status);
  EXPECT_EQ(HAL_kInvalidHandle, digitalIoHandle);
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
  digitalIoHandle = HAL_InitializeDIOPort(portHandle, true, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != digitalIoHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestDigitalIoCallbackName.c_str());
}

}  // namespace hal
