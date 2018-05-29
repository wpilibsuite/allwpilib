/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "MotorEncoderSimulator.h"
#include "MotorModel/LinearMotorModelSimulation.h"
#include "Talon.h"
#include "WpiSimulators/WpiEncoderSimulator.h"
#include "WpiSimulators/WpiMotorSimulator.h"
#include "gtest/gtest.h"

TEST(MotorEncoderSimulatorTest, TestWithoutDistancePerPulseFullSpeed) {
  frc::Talon talon{3};
  frc::Encoder encoder{3, 1};
  std::shared_ptr<frc::sim::lowfi::WpiMotorSimulator> motorSim(
      new frc::sim::lowfi::WpiMotorSimulator(3));
  std::shared_ptr<frc::sim::lowfi::WpiEncoderSimulator> encoderSim(
      new frc::sim::lowfi::WpiEncoderSimulator(0));
  std::shared_ptr<frc::sim::lowfi::LinearMotorModelSimulation> motorModelSim(
      new frc::sim::lowfi::LinearMotorModelSimulation(6000));

  motorSim->SetMotorModelSimulation(motorModelSim);

  frc::sim::lowfi::MotorEncoderSimulator connector(motorSim, encoderSim);

  talon.Set(-1);
  motorSim->Update(1);
  connector.Update();

  // Position
  EXPECT_EQ(-6000, encoder.Get());
  EXPECT_DOUBLE_EQ(-6000, encoder.GetDistance());

  // Velocity
  EXPECT_DOUBLE_EQ(-1.0 / 6000, encoder.GetPeriod());
  EXPECT_DOUBLE_EQ(-6000, encoder.GetRate());
}

TEST(MotorEncoderSimulatorTest, TestWithoutDistancePerPulseRealisitcUpdate) {
  frc::Talon talon{3};
  frc::Encoder encoder{3, 1};
  std::shared_ptr<frc::sim::lowfi::WpiMotorSimulator> motorSim(
      new frc::sim::lowfi::WpiMotorSimulator(3));
  std::shared_ptr<frc::sim::lowfi::WpiEncoderSimulator> encoderSim(
      new frc::sim::lowfi::WpiEncoderSimulator(0));
  std::shared_ptr<frc::sim::lowfi::LinearMotorModelSimulation> motorModelSim(
      new frc::sim::lowfi::LinearMotorModelSimulation(6000));

  motorSim->SetMotorModelSimulation(motorModelSim);

  frc::sim::lowfi::MotorEncoderSimulator connector(motorSim, encoderSim);

  talon.Set(0.5);
  motorSim->Update(.02);
  connector.Update();

  // Position
  EXPECT_EQ(60, encoder.Get());
  EXPECT_DOUBLE_EQ(60, encoder.GetDistance());

  // Velocity
  EXPECT_DOUBLE_EQ(1.0 / 3000, encoder.GetPeriod());
  EXPECT_DOUBLE_EQ(3000, encoder.GetRate());
}

TEST(MotorEncoderSimulatorTest, TestWithDistancePerPulseFullSpeed) {
  frc::Talon talon{3};
  frc::Encoder encoder{3, 1};
  encoder.SetDistancePerPulse(.001);
  std::shared_ptr<frc::sim::lowfi::WpiMotorSimulator> motorSim(
      new frc::sim::lowfi::WpiMotorSimulator(3));
  std::shared_ptr<frc::sim::lowfi::WpiEncoderSimulator> encoderSim(
      new frc::sim::lowfi::WpiEncoderSimulator(0));
  std::shared_ptr<frc::sim::lowfi::LinearMotorModelSimulation> motorModelSim(
      new frc::sim::lowfi::LinearMotorModelSimulation(6000));

  motorSim->SetMotorModelSimulation(motorModelSim);

  frc::sim::lowfi::MotorEncoderSimulator connector(motorSim, encoderSim);

  talon.Set(-1);

  motorSim->Update(1);
  connector.Update();

  // Position
  EXPECT_EQ(-6000000, encoder.Get());
  EXPECT_DOUBLE_EQ(-6000, encoder.GetDistance());

  // Velocity
  EXPECT_EQ(-1.0 / 6000, encoder.GetPeriod());
  EXPECT_DOUBLE_EQ(-6, encoder.GetRate());
}

TEST(MotorEncoderSimulatorTest, TestWithDistancePerPulseRealistic) {
  frc::Talon talon{3};
  frc::Encoder encoder{3, 1};
  encoder.SetDistancePerPulse(.001);
  std::shared_ptr<frc::sim::lowfi::WpiMotorSimulator> motorSim(
      new frc::sim::lowfi::WpiMotorSimulator(3));
  std::shared_ptr<frc::sim::lowfi::WpiEncoderSimulator> encoderSim(
      new frc::sim::lowfi::WpiEncoderSimulator(0));
  std::shared_ptr<frc::sim::lowfi::LinearMotorModelSimulation> motorModelSim(
      new frc::sim::lowfi::LinearMotorModelSimulation(6000));

  motorSim->SetMotorModelSimulation(motorModelSim);

  frc::sim::lowfi::MotorEncoderSimulator connector(motorSim, encoderSim);

  talon.Set(0.5);

  motorSim->Update(.02);
  connector.Update();

  // Position
  EXPECT_EQ(60000, encoder.Get());
  EXPECT_DOUBLE_EQ(60, encoder.GetDistance());

  // Velocity
  EXPECT_EQ(1.0 / 3000, encoder.GetPeriod());
  EXPECT_DOUBLE_EQ(3, encoder.GetRate());
}
