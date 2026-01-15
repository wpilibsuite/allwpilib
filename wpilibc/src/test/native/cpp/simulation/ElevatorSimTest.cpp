// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ElevatorSim.hpp"

#include <gtest/gtest.h>

#include "wpi/hardware/motor/PWMVictorSPX.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/time.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

TEST(ElevatorSimTest, StateSpaceSim) {
  wpi::sim::ElevatorSim sim(wpi::math::DCMotor::Vex775Pro(4), 14.67, 8_kg,
                            0.75_in, 0_m, 3_m, true, 0_m, {0.01});
  wpi::math::PIDController controller(10, 0.0, 0.0);

  wpi::PWMVictorSPX motor(0);
  wpi::Encoder encoder(0, 1);
  wpi::sim::EncoderSim encoderSim(encoder);

  for (size_t i = 0; i < 100; ++i) {
    controller.SetSetpoint(2.0);
    auto nextVoltage = controller.Calculate(encoderSim.GetDistance());
    motor.SetDutyCycle(nextVoltage / wpi::RobotController::GetInputVoltage());

    wpi::math::Vectord<1> u{motor.GetDutyCycle() *
                            wpi::RobotController::GetInputVoltage()};
    sim.SetInput(u);
    sim.Update(20_ms);

    const auto& y = sim.GetOutput();
    encoderSim.SetDistance(y(0));
  }

  EXPECT_NEAR(controller.GetSetpoint(), sim.GetPosition().value(), 0.2);
}

TEST(ElevatorSimTest, InitialState) {
  constexpr auto startingHeight = 0.5_m;
  wpi::sim::ElevatorSim sim(wpi::math::DCMotor::KrakenX60(2), 20, 8_kg, 0.1_m,
                            0_m, 1_m, true, startingHeight, {0.01, 0.0});

  EXPECT_DOUBLE_EQ(startingHeight.value(), sim.GetPosition().value());
  EXPECT_DOUBLE_EQ(0, sim.GetVelocity().value());
}

TEST(ElevatorSimTest, MinMax) {
  wpi::sim::ElevatorSim sim(wpi::math::DCMotor::Vex775Pro(4), 14.67, 8_kg,
                            0.75_in, 0_m, 1_m, true, 0_m, {0.01});
  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(wpi::math::Vectord<1>{0.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height > -0.05_m);
  }

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(wpi::math::Vectord<1>{12.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height < 1.05_m);
  }
}

TEST(ElevatorSimTest, Stability) {
  wpi::sim::ElevatorSim sim{wpi::math::DCMotor::Vex775Pro(4),
                            100,
                            4_kg,
                            0.5_in,
                            0_m,
                            10_m,
                            false,
                            0_m};

  sim.SetState(wpi::math::Vectord<2>{0.0, 0.0});
  sim.SetInput(wpi::math::Vectord<1>{12.0});
  for (int i = 0; i < 50; ++i) {
    sim.Update(20_ms);
  }

  wpi::math::LinearSystem<2, 1, 1> system =
      wpi::math::Models::ElevatorFromPhysicalConstants(
          wpi::math::DCMotor::Vex775Pro(4), 4_kg, 0.5_in, 100)
          .Slice(0);
  EXPECT_NEAR_UNITS(wpi::units::meter_t{system.CalculateX(
                        wpi::math::Vectord<2>{0.0, 0.0},
                        wpi::math::Vectord<1>{12.0}, 20_ms * 50)(0)},
                    sim.GetPosition(), 1_cm);
}

TEST(ElevatorSimTest, CurrentDraw) {
  constexpr auto motor = wpi::math::DCMotor::KrakenX60(2);
  wpi::sim::ElevatorSim sim(motor, 20, 8_kg, 0.1_m, 0_m, 1_m, true, 0_m,
                            {0.01, 0.0});

  EXPECT_DOUBLE_EQ(0.0, sim.GetCurrentDraw().value());
  sim.SetInputVoltage(motor.Voltage(motor.Torque(60_A), 0_rad_per_s));
  sim.Update(100_ms);
  // Current draw should start at 60 A and decrease as the back-EMF catches up
  EXPECT_TRUE(0_A < sim.GetCurrentDraw() && sim.GetCurrentDraw() < 60_A);
}
