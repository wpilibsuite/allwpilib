// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <fmt/format.h>

#include "wpi/framework/OpModeRobot.hpp"
#include "wpi/opmode/PeriodicOpMode.hpp"

class Robot;

class MyTeleop : public wpi::PeriodicOpMode {
 public:
  /** Constructor. */
  MyTeleop() {
    /*
     * The constructor is called when the opmode is selected on the DS.
     * The robot is typically disabled when this is called (but in a
     * competition match might be enabled).
     *
     * Note: No-arg constructors are okay too.
     *
     * Can call super(period) to set a different periodic time interval.
     *
     * Additional periodic methods may be configured with addPeriodic().
     */
    fmt::println("MyTeleop constructed");
  }

  ~MyTeleop() override {
    /* Called when the opmode is de-selected. */
    fmt::println("MyTeleop destroyed");
  }

  void DisabledPeriodic() override {
    /* Called periodically (on every DS packet) while the robot is disabled. */
    m_disabledCount++;
    if (m_disabledCount % 20 == 0) {
      fmt::println("MyTeleop disabled periodic {}", m_disabledCount);
    }
  }

  void Start() override {
    /* Called once when the robot is first enabled. */
    fmt::println("MyTeleop start");
  }

  void Periodic() override {
    /* Called periodically (set time interval) while the robot is enabled. */
    m_periodicCount++;
    if (m_periodicCount % 20 == 0) {
      fmt::println("MyTeleop periodic {}", m_periodicCount);
    }
  }

  void End() override {
    /* Called when the robot is disabled (after previously being enabled). */
    fmt::println("MyTeleop end");
  }

 private:
  int m_disabledCount = 0;
  int m_periodicCount = 0;
};

class MyAuto : public wpi::PeriodicOpMode {
 public:
  /** Can pass the Robot instance into an opmode via the constructor. */
  explicit MyAuto(Robot& robot) : m_robot{robot} {}

  void Periodic() override {}

 private:
  Robot& m_robot;
};

class HangTeleop : public wpi::PeriodicOpMode {
 public:
  void Periodic() override {
    using namespace std::chrono_literals;
    fmt::println("sleeping");
    std::this_thread::sleep_for(10s);
    fmt::println("woke up");
  }
};

class Robot : public wpi::OpModeRobot<Robot> {
 public:
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  Robot() {
    fmt::println("Robot constructed");
    AddOpMode<MyTeleop>(wpi::RobotMode::TELEOPERATED, "My Teleop", "Examples",
                        "An example teleop opmode");
    AddOpMode<MyAuto>(wpi::RobotMode::AUTONOMOUS, "My Auto", "Group 1");
    AddOpMode<HangTeleop>(wpi::RobotMode::TELEOPERATED, "Hang Teleop",
                          "Examples", "An opmode that hangs in periodic");
    PublishOpModes();
  }

  /** This function is called exactly once when the DS first connects. */
  void DriverStationConnected() override {
    fmt::println("driver station connected");
  }

  /**
   * This function is called periodically anytime when no opmode is selected,
   * including when the Driver Station is disconnected.
   */
  void NonePeriodic() override {
    m_count++;
    if (m_count >= 20) {
      fmt::println("no opmode selected");
      m_count = 0;
    }
  }

 private:
  int m_count = 0;
};

int main() {
  return wpi::StartRobot<Robot>();
}
