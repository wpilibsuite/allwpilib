/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/SPI.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/SPIData.h"

namespace hal {

std::string gTestSpiCallbackName;
HAL_Value gTestSpiCallbackValue;

void TestSpiInitializationCallback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  gTestSpiCallbackName = name;
  gTestSpiCallbackValue = *value;
}

TEST(SpiSimTests, TestSpiInitialization) {
  const int INDEX_TO_TEST = 2;

  int32_t status;
  HAL_SPIPort port;

  int callbackParam = 0;
  int callbackId = HALSIM_RegisterSPIInitializedCallback(
      INDEX_TO_TEST, &TestSpiInitializationCallback, &callbackParam, false);
  ASSERT_TRUE(0 != callbackId);

  status = 0;
  port = HAL_SPI_kOnboardCS2;
  gTestSpiCallbackName = "Unset";
  HAL_InitializeSPI(port, &status);
  EXPECT_STREQ("Initialized", gTestSpiCallbackName.c_str());
}

}  // namespace hal
