// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Robot.hpp"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/simulation/AnalogInputSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/ElevatorSim.hpp"
#include "wpi/simulation/JoystickSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/time.hpp"

class PotentiometerPIDTest : public testing::Test {
  wpi::math::DCMotor m_elevatorGearbox = wpi::math::DCMotor::Vex775Pro(4);
  static constexpr double kElevatorGearing = 10.0;
  static constexpr wpi::units::meter_t kElevatorDrumRadius = 2.0_in;
  static constexpr wpi::units::kilogram_t kCarriageMass = 4.0_kg;

  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  wpi::sim::ElevatorSim m_elevatorSim{m_elevatorGearbox,
                                      kElevatorGearing,
                                      kCarriageMass,
                                      kElevatorDrumRadius,
                                      0.0_m,
                                      Robot::kFullHeight,
                                      true,
                                      0.0_m};
  wpi::sim::PWMMotorControllerSim m_motorSim{Robot::kMotorChannel};
  wpi::sim::AnalogInputSim m_analogSim{Robot::kPotChannel};
  wpi::sim::JoystickSim m_joystickSim{Robot::kJoystickChannel};
  int32_t m_callback;
  int32_t m_port;

 public:
  void SimPeriodicBefore() {
    m_elevatorSim.SetInputVoltage(m_motorSim.GetSpeed() *
                                  wpi::RobotController::GetBatteryVoltage());
    m_elevatorSim.Update(20_ms);

    /*
    meters = (v / 3.3v) * range
    meters / range = v / 3.3v
    3.3v * (meters / range) = v
     */
    m_analogSim.SetVoltage(
        (wpi::RobotController::GetVoltage3V3() *
         (m_elevatorSim.GetPosition().value() / Robot::kFullHeight))
            .value());
  }

  static void CallSimPeriodicBefore(void* param) {
    static_cast<PotentiometerPIDTest*>(param)->SimPeriodicBefore();
  }

  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::DriverStationSim::ResetData();

    m_joystickSim.SetButtonsMaximumIndex(12);

    m_callback =
        HALSIM_RegisterSimPeriodicBeforeCallback(CallSimPeriodicBefore, this);

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    wpi::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    HALSIM_CancelSimPeriodicBeforeCallback(m_callback);
    m_analogSim.ResetData();
  }
};

TEST_F(PotentiometerPIDTest, Teleop) {
  // teleop init
  {
    wpi::sim::DriverStationSim::SetAutonomous(false);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_analogSim.GetInitialized());
  }

  // first setpoint
  {
    // advance 50 timesteps
    wpi::sim::StepTiming(1_s);

    EXPECT_NEAR(Robot::kSetpoints[0].value(),
                m_elevatorSim.GetPosition().value(), 0.1);
  }

  // second setpoint
  {
    // press button to advance setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 50 timesteps
    wpi::sim::StepTiming(1_s);

    EXPECT_NEAR(Robot::kSetpoints[1].value(),
                m_elevatorSim.GetPosition().value(), 0.1);
  }

  // we need to unpress the button
  {
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    // advance 10 timesteps
    wpi::sim::StepTiming(0.2_s);
  }

  // third setpoint
  {
    // press button to advance setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 50 timesteps
    wpi::sim::StepTiming(1_s);

    EXPECT_NEAR(Robot::kSetpoints[2].value(),
                m_elevatorSim.GetPosition().value(), 0.1);
  }

  // we need to unpress the button
  {
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    // advance 10 timesteps
    wpi::sim::StepTiming(0.2_s);
  }

  // rollover: first setpoint
  {
    // press button to advance setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 60 timesteps
    wpi::sim::StepTiming(1.2_s);
    EXPECT_NEAR(Robot::kSetpoints[0].value(),
                m_elevatorSim.GetPosition().value(), 0.1);
  }
}
