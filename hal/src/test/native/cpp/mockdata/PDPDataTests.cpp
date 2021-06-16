// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/PDP.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/PDPData.h"

namespace hal {

std::string gTestPdpCallbackName;
HAL_Value gTestPdpCallbackValue;

void TestPdpInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestPdpCallbackName = name;
  gTestPdpCallbackValue = *value;
}

TEST(PdpSimTests, TestPdpInitialization) {
  const int INDEX_TO_TEST = 1;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterPDPInitializedCallback(
      INDEX_TO_TEST, &TestPdpInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  int32_t status = 0;

  // Use out of range index
  gTestPdpCallbackName = "Unset";
  HAL_InitializePDP(INDEX_TO_TEST, &status);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPdpCallbackName.c_str());
}

}  // namespace hal
