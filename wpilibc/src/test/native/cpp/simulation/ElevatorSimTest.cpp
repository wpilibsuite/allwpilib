// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <units/math.h>
#include <units/time.h>

#include "frc/Encoder.h"
#include "frc/RobotController.h"
#include "frc/controller/PIDController.h"
#include "frc/motorcontrol/PWMVictorSPX.h"
#include "frc/simulation/ElevatorSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/LinearSystemId.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(ElevatorSimTest, StateSpaceSim) {
  frc::sim::ElevatorSim sim(frc::DCMotor::Vex775Pro(4), 14.67, 8_kg, 0.75_in,
                            0_m, 3_m, true, 0_m, {0.01});
  frc::PIDController controller(10, 0.0, 0.0);

  frc::PWMVictorSPX motor(0);
  frc::Encoder encoder(0, 1);
  frc::sim::EncoderSim encoderSim(encoder);

  for (size_t i = 0; i < 100; ++i) {
    controller.SetSetpoint(2.0);
    auto nextVoltage = controller.Calculate(encoderSim.GetDistance());
    motor.Set(nextVoltage / frc::RobotController::GetInputVoltage());

    frc::Vectord<1> u{motor.Get() * frc::RobotController::GetInputVoltage()};
    sim.SetInput(u);
    sim.Update(20_ms);

    const auto& y = sim.GetOutput();
    encoderSim.SetDistance(y(0));
  }

  EXPECT_NEAR(controller.GetSetpoint(), sim.GetPosition().value(), 0.2);
}

TEST(ElevatorSimTest, MinMax) {
  frc::sim::ElevatorSim sim(frc::DCMotor::Vex775Pro(4), 14.67, 8_kg, 0.75_in,
                            0_m, 1_m, true, 0_m, {0.01});
  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(frc::Vectord<1>{0.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height > -0.05_m);
  }

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(frc::Vectord<1>{12.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height < 1.05_m);
  }
}

TEST(ElevatorSimTest, Stability) {
  frc::sim::ElevatorSim sim{
      frc::DCMotor::Vex775Pro(4), 100, 4_kg, 0.5_in, 0_m, 10_m, false, 0_m};

  sim.SetState(frc::Vectord<2>{0.0, 0.0});
  sim.SetInput(frc::Vectord<1>{12.0});
  for (int i = 0; i < 50; ++i) {
    sim.Update(20_ms);
  }

  frc::LinearSystem<2, 1, 1> system =
      frc::LinearSystemId::ElevatorSystem(frc::DCMotor::Vex775Pro(4), 4_kg,
                                          0.5_in, 100)
          .Slice(0);
  EXPECT_NEAR_UNITS(
      units::meter_t{system.CalculateX(frc::Vectord<2>{0.0, 0.0},
                                       frc::Vectord<1>{12.0}, 20_ms * 50)(0)},
      sim.GetPosition(), 1_cm);
}
