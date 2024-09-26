// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/I2C.h"
#include "hal/simulation/I2CData.h"

namespace hal {

std::string gTestI2CCallbackName;
HAL_Value gTestI2CCallbackValue;

void TestI2CInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestI2CCallbackName = name;
  gTestI2CCallbackValue = *value;
}

TEST(I2CSimTest, I2CInitialization) {
  const int INDEX_TO_TEST = 1;

  int32_t status = 0;
  HAL_I2CPort port;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterI2CInitializedCallback(
      INDEX_TO_TEST, &TestI2CInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  port = HAL_I2C_kMXP;
  gTestI2CCallbackName = "Unset";
  HAL_InitializeI2C(port, &status);
  EXPECT_STREQ("Initialized", gTestI2CCallbackName.c_str());
}

}  // namespace hal
