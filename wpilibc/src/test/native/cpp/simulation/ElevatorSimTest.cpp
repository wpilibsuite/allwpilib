/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <units/time.h>

#include "frc/Encoder.h"
#include "frc/PWMVictorSPX.h"
#include "frc/RobotController.h"
#include "frc/StateSpaceUtil.h"
#include "frc/controller/PIDController.h"
#include "frc/simulation/ElevatorSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/LinearSystemId.h"
#include "gtest/gtest.h"

TEST(ElevatorSim, StateSpaceSim) {
  frc::sim::ElevatorSim sim(frc::DCMotor::Vex775Pro(4), 14.67, 8_kg,
                            units::meter_t(0.75 * 25.4 / 1000.0), 0_m, 3_m,
                            {0.01});
  frc2::PIDController controller(10, 0.0, 0.0);

  frc::PWMVictorSPX motor(0);
  frc::Encoder encoder(0, 1);
  frc::sim::EncoderSim encoderSim(encoder);

  for (size_t i = 0; i < 100; ++i) {
    controller.SetSetpoint(2.0);
    auto nextVoltage = controller.Calculate(encoderSim.GetDistance());
    motor.Set(nextVoltage / frc::RobotController::GetInputVoltage());

    auto u = frc::MakeMatrix<1, 1>(motor.Get() *
                                   frc::RobotController::GetInputVoltage());
    sim.SetInput(u);
    sim.Update(20_ms);

    const auto& y = sim.GetOutput();
    encoderSim.SetDistance(y(0));
  }

  EXPECT_NEAR(controller.GetSetpoint(), sim.GetPosition().to<double>(), 0.2);
}

TEST(ElevatorSim, MinMax) {
  frc::sim::ElevatorSim sim(frc::DCMotor::Vex775Pro(4), 14.67, 8_kg,
                            units::meter_t(0.75 * 25.4 / 1000.0), 0_m, 1_m,
                            {0.01});
  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(frc::MakeMatrix<1, 1>(0.0));
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height > -0.05_m);
  }

  for (size_t i = 0; i < 100; ++i) {
    sim.SetInput(frc::MakeMatrix<1, 1>(12.0));
    sim.Update(20_ms);

    auto height = sim.GetPosition();
    EXPECT_TRUE(height < 1.05_m);
  }
}
