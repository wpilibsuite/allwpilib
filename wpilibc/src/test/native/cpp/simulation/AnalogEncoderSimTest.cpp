/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <hal/HAL.h>
#include <units/math.h>
#include <wpi/math>

#include "frc/AnalogEncoder.h"
#include "frc/AnalogInput.h"
#include "frc/simulation/AnalogEncoderSim.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(AnalogEncoderSimTest, TestBasic) {
  frc::AnalogInput ai(0);
  frc::AnalogEncoder encoder{ai};
  frc::sim::AnalogEncoderSim encoderSim{encoder};

  encoderSim.SetPosition(180_deg);
  EXPECT_NEAR(encoder.Get().to<double>(), 0.5, 1E-8);
  EXPECT_NEAR(encoderSim.GetTurns().to<double>(), 0.5, 1E-8);
  EXPECT_NEAR(encoderSim.GetPosition().Radians().to<double>(), wpi::math::pi,
              1E-8);
}
