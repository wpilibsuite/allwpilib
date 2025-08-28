// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <units/math.hpp>
#include <units/time.hpp>

#include "frc/Encoder.h"
#include "frc/RobotController.h"
#include "wpi/math/controller/PIDController.hpp"
#include "frc/motorcontrol/PWMVictorSPX.h"
#include "frc/simulation/ElevatorSim.h"
#include "frc/simulation/EncoderSim.h"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

using namespace wpi::math;

TEST(ElevatorSimTest, StateSpaceSim) {
  sim::ElevatorSim sim(DCMotor::Vex775Pro(4), 14.67, 8_kg, 0.75_in,
                            0_m, 3_m, true, 0_m, {0.01});
  PIDController controller(10, 0.0, 0.0);

  frc::PWMVictorSPX motor(0);
  frc::Encoder encoder(0, 1);
  frc::sim::EncoderSim encoderSim(encoder);

  for (size_t i = 0; i < 100; ++i) {
    controller.SetSetpoint(2.0);
    auto nextVoltage = controller.Calculate(encoderSim.GetDistance());
    motor.Set(nextVoltage / frc::RobotController::GetInputVoltage());

    Vectord<1> u{motor.Get() * frc::RobotController::GetInputVoltage()};
    sim.SetInput(u);
    sim.Update(20_ms);

    const auto& y = sim.GetOutput();
    encoderSim.SetDistance(y(0));
  }

  EXPECT_NEAR(controller.GetSetpoint(), sim.GetPosition().value(), 0.2);
}

TEST(ElevatorSimTest, InitialState) {
  constexpr auto startingHeight = 0.5_m;
  sim::ElevatorSim sim(DCMotor::KrakenX60(2), 20, 8_kg, 0.1_m, 0_m,
                            1_m, true, startingHeight, {0.01, 0.0});

  EXPECT_DOUBLE_EQ(startingHeight.value(), sim.GetPosition().value());
  EXPECT_DOUBLE_EQ(0, sim.GetVelocity().value());
}

TEST(ElevatorSimTest, MinMax) {
  sim::ElevatorSim sim(DCMotor::Vex775Pro(4), 14.67, 8_kg, 0.75_in,
                            0_m, 1_m, true, 0_m, {0.01});
  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(Vectord<1>{0.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height > -0.05_m);
  }

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(Vectord<1>{12.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height < 1.05_m);
  }
}

TEST(ElevatorSimTest, Stability) {
  sim::ElevatorSim sim{
      DCMotor::Vex775Pro(4), 100, 4_kg, 0.5_in, 0_m, 10_m, false, 0_m};

  sim.SetState(Vectord<2>{0.0, 0.0});
  sim.SetInput(Vectord<1>{12.0});
  for (int i = 0; i < 50; ++i) {
    sim.Update(20_ms);
  }

  LinearSystem<2, 1, 1> system =
      LinearSystemId::ElevatorSystem(DCMotor::Vex775Pro(4), 4_kg,
                                          0.5_in, 100)
          .Slice(0);
  EXPECT_NEAR_UNITS(
      units::meter_t{system.CalculateX(Vectord<2>{0.0, 0.0},
                                       Vectord<1>{12.0}, 20_ms * 50)(0)},
      sim.GetPosition(), 1_cm);
}
