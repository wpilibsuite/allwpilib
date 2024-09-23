// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/PowerDistribution.h"
#include "hal/simulation/PowerDistributionData.h"

namespace hal {

std::string gTestPdpCallbackName;
HAL_Value gTestPdpCallbackValue;

void TestPdpInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestPdpCallbackName = name;
  gTestPdpCallbackValue = *value;
}

TEST(PdpSimTest, PdpInitialization) {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPowerDistributionInitializedCallback(
      INDEX_TO_TEST, &TestPdpInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;

  // Use out of range index
  gTestPdpCallbackName = "Unset";
  HAL_InitializePowerDistribution(
      INDEX_TO_TEST, HAL_PowerDistributionType_kCTRE, nullptr, &status);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPdpCallbackName.c_str());
}

}  // namespace hal
