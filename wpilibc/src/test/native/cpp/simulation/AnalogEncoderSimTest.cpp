// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <hal/HAL.h>
#include <units/math.h>

#include "frc/AnalogEncoder.h"
#include "frc/AnalogInput.h"
#include "frc/simulation/AnalogEncoderSim.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(AnalogEncoderSimTest, Basic) {
  frc::AnalogInput ai(0);
  frc::AnalogEncoder encoder{ai};
  frc::sim::AnalogEncoderSim encoderSim{encoder};

  encoderSim.SetPosition(180_deg);
  EXPECT_NEAR(encoder.Get().value(), 0.5, 1E-8);
  EXPECT_NEAR(encoderSim.GetTurns().value(), 0.5, 1E-8);
  EXPECT_NEAR(encoderSim.GetPosition().Radians().value(), std::numbers::pi,
              1E-8);
}
