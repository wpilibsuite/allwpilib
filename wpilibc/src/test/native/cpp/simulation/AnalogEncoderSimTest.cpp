// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogEncoderSim.hpp"

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/AnalogInput.hpp"
#include "wpi/hardware/rotation/AnalogEncoder.hpp"
#include "wpi/units/math.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

TEST(AnalogEncoderSimTest, Basic) {
  wpi::AnalogInput ai(0);
  wpi::AnalogEncoder encoder{ai, 360, 0};
  wpi::sim::AnalogEncoderSim encoderSim{encoder};

  encoderSim.Set(180);
  EXPECT_NEAR(encoder.Get(), 180, 1E-8);
  EXPECT_NEAR(encoderSim.Get(), 180, 1E-8);
}
