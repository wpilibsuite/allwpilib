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
#include "wpi/simulation/RoboRioSim.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/time.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

TEST(ElevatorSimTest, StateSpaceSim) {
  auto gearbox = wpi::math::DCMotor::Vex775Pro(4);
  auto plant = wpi::math::Models::ElevatorFromPhysicalConstants(gearbox, 8_kg,
                                                                0.75_in, 14.67);
  wpi::sim::ElevatorSim sim(plant, gearbox, 14.67,
                            wpi::units::volt_t{9.8 / plant.B(1, 0)}, 0_m, 3_m,
                            0_m, {0.01, 0.0});
  wpi::math::PIDController controller(10, 0.0, 0.0);

  wpi::PWMVictorSPX motor(0);
  wpi::Encoder encoder(0, 1);
  wpi::sim::EncoderSim encoderSim(encoder);

  for (size_t i = 0; i < 100; ++i) {
    controller.SetSetpoint(2.0);
    auto nextVoltage = controller.Calculate(encoderSim.GetDistance());
    motor.SetThrottle(nextVoltage / wpi::RobotController::GetInputVoltage());

    wpi::math::Vectord<1> u{motor.GetThrottle() *
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
  auto gearbox = wpi::math::DCMotor::KrakenX60(2);
  auto plant = wpi::math::Models::ElevatorFromPhysicalConstants(gearbox, 8_kg,
                                                                0.1_m, 20);
  wpi::sim::ElevatorSim sim(plant, gearbox, 20,
                            wpi::units::volt_t{9.8 / plant.B(1, 0)}, 0_m, 1_m,
                            startingHeight, {0.01, 0.0});

  EXPECT_DOUBLE_EQ(startingHeight.value(), sim.GetPosition().value());
  EXPECT_DOUBLE_EQ(0, sim.GetVelocity().value());
}

TEST(ElevatorSimTest, MinMax) {
  auto gearbox = wpi::math::DCMotor::Vex775Pro(4);
  auto plant = wpi::math::Models::ElevatorFromPhysicalConstants(gearbox, 8_kg,
                                                                0.75_in, 14.67);
  wpi::sim::ElevatorSim sim(plant, gearbox, 14.67,
                            wpi::units::volt_t{9.8 / plant.B(1, 0)}, 0_m, 1_m,
                            0_m);

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(wpi::math::Vectord<1>{0.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_GE(height, 0_m);
  }

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(wpi::math::Vectord<1>{12.0});
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_LE(height, 1_m);
  }
}

TEST(ElevatorSimTest, Stability) {
  auto gearbox = wpi::math::DCMotor::Vex775Pro(4);
  auto plant = wpi::math::Models::ElevatorFromPhysicalConstants(gearbox, 4_kg,
                                                                0.5_in, 100);
  wpi::sim::ElevatorSim sim{plant, gearbox, 100, 0_V, 0_m, 10_m, 0_m};

  sim.SetState(wpi::math::Vectord<2>{0.0, 0.0});
  sim.SetInput(wpi::math::Vectord<1>{12.0});
  for (int i = 0; i < 50; ++i) {
    sim.Update(20_ms);
  }

  // This plant's velocity pole is near -22000, so its time constant is far
  // shorter than the 20 ms timestep. The sim must stay stable and settle at the
  // steady-state velocity -B₁,₀u/A₁,₁ rather than diverging.
  wpi::units::meters_per_second_t steadyStateVelocity{-plant.B(1, 0) * 12.0 /
                                                      plant.A(1, 1)};
  EXPECT_NEAR_UNITS(steadyStateVelocity, sim.GetVelocity(), 1e-6_mps);
  EXPECT_NEAR_UNITS(steadyStateVelocity * (20_ms * 50), sim.GetPosition(),
                    1_cm);
}

TEST(ElevatorSimTest, CurrentDraw) {
  wpi::sim::RoboRioSim::ResetData();

  constexpr auto motor = wpi::math::DCMotor::KrakenX60(2);
  auto plant =
      wpi::math::Models::ElevatorFromPhysicalConstants(motor, 8_kg, 0.1_m, 20);
  wpi::sim::ElevatorSim sim(plant, motor, 20,
                            wpi::units::volt_t{9.8 / plant.B(1, 0)}, 0_m, 1_m,
                            0_m, {0.01, 0.0});

  EXPECT_DOUBLE_EQ(0.0, sim.GetCurrentDraw().value());

  // Apply the voltage that pushes 60 A through the motor at zero velocity.
  // GetCurrentDraw() reports battery-side current, so the expected draw is that
  // scaled by the duty cycle.
  auto appliedVoltage = motor.Voltage(motor.Torque(60_A), 0_rad_per_s);
  double dutyCycle = appliedVoltage / wpi::RobotController::GetBatteryVoltage();
  sim.SetInputVoltage(appliedVoltage);
  EXPECT_NEAR(60.0 * dutyCycle, sim.GetCurrentDraw().value(), 1e-9);

  // Current draw decreases as the back-EMF catches up.
  sim.Update(100_ms);
  EXPECT_TRUE(0_A < sim.GetCurrentDraw() &&
              sim.GetCurrentDraw() < wpi::units::ampere_t{60.0 * dutyCycle});
}
