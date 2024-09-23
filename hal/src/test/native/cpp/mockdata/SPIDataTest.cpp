// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>

#include "hal/SPI.h"
#include "hal/simulation/SPIData.h"

namespace hal {

std::string gTestSpiCallbackName;
HAL_Value gTestSpiCallbackValue;

void TestSpiInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestSpiCallbackName = name;
  gTestSpiCallbackValue = *value;
}

TEST(SpiSimTest, SpiInitialization) {
  const int INDEX_TO_TEST = 2;

  int32_t status = 0;
  HAL_SPIPort port;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterSPIInitializedCallback(
      INDEX_TO_TEST, &TestSpiInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  port = HAL_SPI_kOnboardCS2;
  gTestSpiCallbackName = "Unset";
  HAL_InitializeSPI(port, &status);
  EXPECT_STREQ("Initialized", gTestSpiCallbackName.c_str());
}

}  // namespace hal
