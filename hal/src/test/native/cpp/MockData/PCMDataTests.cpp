/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/PCMData.h"
#include "HAL/HAL.h"
#include "HAL/Solenoid.h"
#include "HAL/handles/HandlesInternal.h"
#include "gtest/gtest.h"

namespace hal {

std::string gTestSolenoidCallbackName;
HAL_Value gTestSolenoidCallbackValue;

void TestSolenoidInitializationCallback(const char *name, void *param,
                                        const struct HAL_Value *value) {
  gTestSolenoidCallbackName = name;
  gTestSolenoidCallbackValue = *value;
}

TEST(SolenoidSimTests, TestSolenoidInitialization) {
  const int INDEX_TO_TEST = 6;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPCMSolenoidInitializedCallback(
      0, 5, &TestSolenoidInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status;
  HAL_PortHandle portHandle;
  HAL_DigitalHandle solenoidHandle;

  // Use out of range index
  status = 0;
  portHandle = 8000;
  gTestSolenoidCallbackName = "Unset";
  solenoidHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, solenoidHandle);
  EXPECT_EQ(PARAMETER_OUT_OF_RANGE, status);
  EXPECT_STREQ("Unset", gTestSolenoidCallbackName.c_str());

  // Successful setup
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestSolenoidCallbackName = "Unset";
  solenoidHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(0xF02000E, solenoidHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestSolenoidCallbackName.c_str());

  // Double initialize... should fail
  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestSolenoidCallbackName = "Unset";
  solenoidHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(HAL_kInvalidHandle, solenoidHandle);
  EXPECT_EQ(RESOURCE_IS_ALLOCATED, status);
  EXPECT_STREQ("Unset", gTestSolenoidCallbackName.c_str());

  // Reset, should allow you to re-register
  hal::HandleBase::ResetGlobalHandles();
  HALSIM_ResetPCMData(INDEX_TO_TEST);
  callbackId = HALSIM_RegisterPCMSolenoidInitializedCallback(
      0, INDEX_TO_TEST, &TestSolenoidInitializationCallback, &callbackParam,
      false);

  status = 0;
  portHandle = HAL_GetPort(INDEX_TO_TEST);
  gTestSolenoidCallbackName = "Unset";
  solenoidHandle = HAL_InitializeSolenoidPort(portHandle, &status);
  EXPECT_EQ(0xF02000E, solenoidHandle);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestSolenoidCallbackName.c_str());
}

} // namespace hal
