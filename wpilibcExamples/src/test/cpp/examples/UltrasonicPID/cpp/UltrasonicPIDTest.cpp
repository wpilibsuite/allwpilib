// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <frc/RobotController.h>
#include <frc/simulation/DifferentialDrivetrainSim.h>
#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/simulation/SimHooks.h>
#include <frc/simulation/UltrasonicSim.h>
#include <frc/system/plant/DCMotor.h>
#include <frc/system/plant/LinearSystemId.h>
#include <gtest/gtest.h>
#include <hal/simulation/MockHooks.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/time.h>

#include "Robot.h"

class UltrasonicPIDTest : public testing::TestWithParam<double> {
  frc::DCMotor m_gearbox = frc::DCMotor::Falcon500(2);
  static constexpr auto kGearing =
      frc::sim::DifferentialDrivetrainSim::KitbotGearing::k10p71;
  static constexpr auto kvLinear = 1.98 * 1_V / 1_mps;
  static constexpr auto kaLinear = 0.2 * 1_V / 1_mps_sq;
  static constexpr auto kvVoltAngular = 1.5 * 1_V / 1_rad_per_s;
  static constexpr auto kaAngular = 0.3 * 1_V / 1_rad_per_s_sq;
  static constexpr auto kWheelDiameter = 0.15_m;
  static constexpr auto kTrackwidth = 0.7_m;

  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  frc::sim::DifferentialDrivetrainSim m_driveSim{
      frc::LinearSystemId::IdentifyDrivetrainSystem(
          kvLinear, kaLinear, kvVoltAngular, kaAngular, kTrackwidth),
      kTrackwidth, m_gearbox, kGearing, kWheelDiameter / 2.0};
  frc::sim::PWMSim m_leftMotorSim{Robot::kLeftMotorPort};
  frc::sim::PWMSim m_rightMotorSim{Robot::kRightMotorPort};
  frc::sim::UltrasonicSim m_ultrasonicSim{Robot::kUltrasonicPingPort,
                                          Robot::kUltrasonicEchoPort};
  int32_t m_callback;

  units::millimeter_t m_distance;

 public:
  void SimPeriodicBefore() {
    m_driveSim.SetInputs(
        m_leftMotorSim.GetSpeed() * frc::RobotController::GetBatteryVoltage(),
        m_rightMotorSim.GetSpeed() * frc::RobotController::GetBatteryVoltage());
    m_driveSim.Update(20_ms);

    auto startingDistance = units::meter_t{GetParam()};
    m_distance = m_driveSim.GetLeftPosition() - startingDistance;

    m_ultrasonicSim.SetRange(m_distance);
  }

  static void CallSimPeriodicBefore(void* param) {
    static_cast<UltrasonicPIDTest*>(param)->SimPeriodicBefore();
  }

  void SetUp() override {
    frc::sim::PauseTiming();
    frc::sim::DriverStationSim::ResetData();

    m_callback =
        HALSIM_RegisterSimPeriodicBeforeCallback(CallSimPeriodicBefore, this);

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    HALSIM_CancelSimPeriodicBeforeCallback(m_callback);
    m_leftMotorSim.ResetData();
    m_rightMotorSim.ResetData();
  }
};

TEST_P(UltrasonicPIDTest, Auto) {
  // auto init
  {
    frc::sim::DriverStationSim::SetAutonomous(true);
    frc::sim::DriverStationSim::SetEnabled(true);
    frc::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_leftMotorSim.GetInitialized());
    EXPECT_TRUE(m_rightMotorSim.GetInitialized());
  }

  {
    frc::sim::StepTiming(5_s);

    EXPECT_NEAR(Robot::kHoldDistance.value(), m_distance.value(), 10.0);
  }
}

INSTANTIATE_TEST_SUITE_P(UltrasonicPIDTests, UltrasonicPIDTest,
                         testing::Values(1.3, 0.5, 5.0));
