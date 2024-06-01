// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>
#include <hal/HAL.h>
#include <units/math.h>

#include "frc/AnalogEncoder.h"
#include "frc/AnalogInput.h"
#include "frc/simulation/AnalogEncoderSim.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(AnalogEncoderSimTest, Basic) {
  frc::AnalogInput ai(0);
  frc::AnalogEncoder encoder{ai, 360, 0};
  frc::sim::AnalogEncoderSim encoderSim{encoder};

  encoderSim.Set(180);
  EXPECT_NEAR(encoder.Get(), 180, 1E-8);
  EXPECT_NEAR(encoderSim.Get(), 180, 1E-8);
}
