/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/I2C.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/I2CData.h"

namespace hal {

std::string gTestI2CCallbackName;
HAL_Value gTestI2CCallbackValue;

void TestI2CInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestI2CCallbackName = name;
  gTestI2CCallbackValue = *value;
}

TEST(I2CSimTests, TestI2CInitialization) {
  const int INDEX_TO_TEST = 1;

  int32_t status;
  HAL_I2CPort port;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterI2CInitializedCallback(
      INDEX_TO_TEST, &TestI2CInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  status = 0;
  port = HAL_I2C_kMXP;
  gTestI2CCallbackName = "Unset";
  HAL_InitializeI2C(port, &status);
  EXPECT_STREQ("Initialized", gTestI2CCallbackName.c_str());
}

}  // namespace hal
