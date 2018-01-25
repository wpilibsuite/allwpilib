/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"
#include "HAL/Relay.h"
#include "HAL/handles/HandlesInternal.h"
#include "MockData/RelayData.h"
#include "gtest/gtest.h"

namespace hal {

std::string gTestRelayCallbackName;
HAL_Value gTestRelayCallbackValue;

void TestRelayInitializationCallback(const char* name, void* param,
                                     const struct HAL_Value* value) {
  gTestRelayCallbackName = name;
  gTestRelayCallbackValue = *value;
}

TEST(RelaySimTests, TestRelayInitialization) {
  const int INDEX_TO_TEST = 3;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterRelayInitializedForwardCallback(
      INDEX_TO_TEST, &TestRelayInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle pdpHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestRelayCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("InitializedForward", gTestRelayCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestRelayCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetRelayData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterRelayInitializedForwardCallback(
      INDEX_TO_TEST, &TestRelayInitializationCallback, &callbackParam, false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("InitializedForward", gTestRelayCallbackName.c_str());
}

}  // namespace hal
