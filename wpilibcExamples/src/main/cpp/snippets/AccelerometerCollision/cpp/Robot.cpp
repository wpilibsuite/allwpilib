// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/units/acceleration.hpp"

/**
 * Collision detection snippets for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  void RobotPeriodic() override {
    // Gets the current accelerations in the X and Y directions
    auto xAccel = accelerometer.GetAccelX();
    auto yAccel = accelerometer.GetAccelY();
    // Calculates the jerk in the X and Y directions
    auto xJerk = (xAccel - prevXAccel) / GetPeriod();
    auto yJerk = (yAccel - prevYAccel) / GetPeriod();
    prevXAccel = xAccel;
    prevYAccel = yAccel;

    wpi::telemetry::Log("X Jerk", xJerk);
    wpi::telemetry::Log("Y Jerk", yJerk);
  }

 private:
  wpi::units::meters_per_second_squared<> prevXAccel = 0.0_mps2;
  wpi::units::meters_per_second_squared<> prevYAccel = 0.0_mps2;
  wpi::OnboardIMU accelerometer{wpi::OnboardIMU::MountOrientation::FLAT};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
