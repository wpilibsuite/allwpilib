// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>

#include "hal/handles/IndexedClassedHandleResource.h"

#define HAL_TestHandle HAL_Handle

namespace {
class MyTestClass {};
}  // namespace

namespace hal {
TEST(HandleTest, ClassedHandle) {
  hal::IndexedClassedHandleResource<HAL_TestHandle, MyTestClass, 8,
                                    HAL_HandleEnum::Vendor>
      testClass;
  int32_t status = 0;
  testClass.Allocate(0, std::make_unique<MyTestClass>(), &status);
  EXPECT_EQ(0, status);
}

}  // namespace hal
