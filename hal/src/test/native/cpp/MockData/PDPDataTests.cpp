/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PDP.h"
#include "HAL/HAL.h"
#include "HAL/handles/HandlesInternal.h"
#include "MockData/PDPData.h"
#include "gtest/gtest.h"

namespace hal {

std::string gTestPdpCallbackName;
HAL_Value gTestPdpCallbackValue;

void TestPdpInitializationCallback(const char *name, void *param,
                                   const struct HAL_Value *value) {
  gTestPdpCallbackName = name;
  gTestPdpCallbackValue = *value;
}

TEST(PdpSimTests, TestPdpInitialization) {
  const int INDEX_TO_TEST = 6;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPDPInitializedCallback(
      INDEX_TO_TEST, &TestPdpInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle pdpHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestPdpCallbackName = "Unset";
  pdpHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestPdpCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPdpCallbackName = "Unset";
  pdpHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(0x900001F, pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPdpCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPdpCallbackName = "Unset";
  pdpHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, pdpHandle);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestPdpCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetPDPData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterPDPInitializedCallback(
      INDEX_TO_TEST, &TestPdpInitializationCallback, &callbackParam, false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestPdpCallbackName = "Unset";
  pdpHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(0x901001F, pdpHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPdpCallbackName.c_str());
}

} // namespace hal
