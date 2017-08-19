/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"

#include "cscore.h"

namespace cs {

class CameraSourceTest : public ::testing::Test {
 protected:
  CameraSourceTest() {
  }
};

TEST_F(CameraSourceTest, HTTPCamera) {
  auto source = HttpCamera("axis", "http://localhost:8000");
}

}  // namespace cs
