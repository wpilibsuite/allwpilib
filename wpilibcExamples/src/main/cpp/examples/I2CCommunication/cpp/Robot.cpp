// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/format.h>

#include <frc/DriverStation.h>
#include <frc/I2C.h>
#include <frc/TimedRobot.h>
#include <frc/Timer.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's I2C port.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    // Creates a string to hold current robot state information, including
    // alliance, enabled state, operation mode, and match time. The message
    // is sent in format "AEM###" where A is the alliance color, (R)ed or
    // (B)lue, E is the enabled state, (E)nabled or (D)isabled, M is the
    // operation mode, (A)utonomous or (T)eleop, and ### is the zero-padded
    // time remaining in the match.
    //
    // For example, "RET043" would indicate that the robot is on the red
    // alliance, enabled in teleop mode, with 43 seconds left in the match.
    auto string = fmt::format(
        "{}{}{}{:03}",
        frc::DriverStation::GetAlliance() == frc::DriverStation::Alliance::kRed
            ? "R"
            : "B",
        frc::DriverStation::IsEnabled() ? "E" : "D",
        frc::DriverStation::IsAutonomous() ? "A" : "T",
        static_cast<int>(frc::Timer::GetMatchTime().value()));

    arduino.WriteBulk(reinterpret_cast<uint8_t*>(string.data()), string.size());
  }

 private:
  static constexpr int deviceAddress = 4;
  frc::I2C arduino{frc::I2C::Port::kOnboard, deviceAddress};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
