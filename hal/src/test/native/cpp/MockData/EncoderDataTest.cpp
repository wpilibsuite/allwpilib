/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Encoder.h"
#include "HAL/HAL.h"
#include "HAL/handles/HandlesInternal.h"
#include "MockData/EncoderData.h"
#include "gtest/gtest.h"

namespace hal {

std::string gTestEncoderCallbackName;
HAL_Value gTestEncoderCallbackValue;

void TestEncoderInitializationCallback(const char *name, void *param,
                                       const struct HAL_Value *value) {
  gTestEncoderCallbackName = name;
  gTestEncoderCallbackValue = *value;
}

TEST(EncoderSimTests, TestEncoderInitialization) {
  const int INDEX_TO_TEST = 4;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterEncoderInitializedCallback(
      INDEX_TO_TEST, &TestEncoderInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_Handle digitalSourceHandleA = HAL_kInvalidHandle;
  HAL_Handle digitalSourceHandleB = HAL_kInvalidHandle;
  HAL_AnalogTriggerType analogTriggerTypeA = HAL_Trigger_kState;
  HAL_AnalogTriggerType analogTriggerTypeB = HAL_Trigger_kState;
  HAL_Bool reverseDirection = false;
  HAL_EncoderEncodingType encodingType = HAL_Encoder_k2X;
  HAL_DigitalHandle encoderHandle = HAL_kInvalidHandle;

  // Use out of range index
  status = 0;
  digitalSourceHandleA = 8000;
  digitalSourceHandleB = 8000;
  gTestEncoderCallbackName = "Unset";
  encoderHandle = HAL_InitializeEncoder(
      digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
      analogTriggerTypeB, reverseDirection, encodingType, &status);
  EXPECT_EQ(HAL_kInvalidHandle, encoderHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestEncoderCallbackName.c_str());

  // Successful setup
  status = 0;
  digitalSourceHandleA = 8000;
  digitalSourceHandleB = 8000;
  gTestEncoderCallbackName = "Unset";
  encoderHandle = HAL_InitializeEncoder(
      digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
      analogTriggerTypeB, reverseDirection, encodingType, &status);
  EXPECT_EQ(0x900001F, encoderHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestEncoderCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  digitalSourceHandleA = 8000;
  digitalSourceHandleB = 8000;
  gTestEncoderCallbackName = "Unset";
  encoderHandle = HAL_InitializeEncoder(
      digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
      analogTriggerTypeB, reverseDirection, encodingType, &status);
  EXPECT_EQ(HAL_kInvalidHandle, encoderHandle);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestEncoderCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetEncoderData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterEncoderInitializedCallback(
      INDEX_TO_TEST, &TestEncoderInitializationCallback, &callbackParam, false);

  status = 0;
  digitalSourceHandleA = 8000;
  digitalSourceHandleB = 8000;
  gTestEncoderCallbackName = "Unset";
  encoderHandle = HAL_InitializeEncoder(
      digitalSourceHandleA, analogTriggerTypeA, digitalSourceHandleB,
      analogTriggerTypeB, reverseDirection, encodingType, &status);
  EXPECT_EQ(0x901001F, encoderHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestEncoderCallbackName.c_str());
}
} // namespace hal
