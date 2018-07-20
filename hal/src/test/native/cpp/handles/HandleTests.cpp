/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/handles/IndexedClassedHandleResource.h"

#define HAL_TestHandle HAL_Handle

namespace {
class MyTestClass {};
}  // namespace

namespace hal {
TEST(HandleTests, ClassedHandleTest) {
  hal::IndexedClassedHandleResource<HAL_TestHandle, MyTestClass, 8,
                                    HAL_HandleEnum::Vendor>
      testClass;
  int32_t status = 0;
  testClass.Allocate(0, std::make_unique<MyTestClass>(), &status);
  EXPECT_EQ(0, status);
}

}  // namespace hal
