/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/PDP.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/PDPData.h"

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

  int32_t status;

  // Use out of range index
  status = 0;
  gTestPdpCallbackName = "Unset";
  HAL_InitializePDP(INDEX_TO_TEST, &status);
  EXPECT_EQ(0, status);
  EXPECT_STREQ("Initialized", gTestPdpCallbackName.c_str());
}

}  // namespace hal
