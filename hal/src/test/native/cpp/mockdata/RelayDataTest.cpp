// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/HAL.h"
#include "hal/Relay.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/RelayData.h"

namespace hal {

std::string gTestRelayCallbackName;
HAL_Value gTestRelayCallbackValue;

void TestRelayInitializationCallback(const char* name, void* param,
                                     const struct HAL_Value* value) {
  gTestRelayCallbackName = name;
  gTestRelayCallbackValue = *value;
}

TEST(RelaySimTest, RelayInitialization) {
  const int INDEX_TO_TEST = 3;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterRelayInitializedForwardCallback(
      INDEX_TO_TEST, &TestRelayInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle pdpHandle;

  // Use out of range index
  portHandle = 8000;
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
  EXPECT_EQ(RESOURCE_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestRelayCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("InitializedForward", gTestRelayCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestRelayCallbackName = "Unset";
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, nullptr, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(HAL_USE_LAST_ERROR, status);
  HAL_GetLastError(&status);
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
  pdpHandle = HAL_InitializeRelayPort(portHandle, true, nullptr, &status);
  EXPECT_TRUE(HAL_kInvalidHandle != pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("InitializedForward", gTestRelayCallbackName.c_str());
  HALSIM_CancelRelayInitializedForwardCallback(INDEX_TO_TEST, callbackId);
}

}  // namespace hal
