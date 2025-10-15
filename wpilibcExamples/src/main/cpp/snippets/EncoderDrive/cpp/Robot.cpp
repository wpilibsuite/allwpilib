// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/drive/DifferentialDrive.hpp>
#include <wpi/hardware/motor/Spark.hpp>
#include <wpi/hardware/rotation/Encoder.hpp>
#include <wpi/opmode/TimedRobot.hpp>

/**
 * Encoder drive to distance snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Configures the encoder's distance-per-pulse
    // The robot moves forward 1 foot per encoder rotation
    // There are 256 pulses per encoder rotation
    m_encoder.SetDistancePerPulse(1.0 / 256.0);
    // Invert the right side of the drivetrain. You might have to invert the
    // other side
    rightLeader.SetInverted(true);
    // Configure the followers to follow the leaders
    leftLeader.AddFollower(leftFollower);
    rightLeader.AddFollower(rightFollower);
  }
  void AutonomousPeriodic() override {
    // Drives forward at half speed until the robot has moved 5 feet, then
    // stops:
    if (m_encoder.GetDistance() < 5) {
      drive.TankDrive(0.5, 0.5);
    } else {
      drive.TankDrive(0, 0);
    }
  }

 private:
  // Creates an encoder on DIO ports 0 and 1.
  frc::Encoder m_encoder{0, 1};
  // Initialize motor controllers and drive
  frc::Spark leftLeader{0};
  frc::Spark leftFollower{1};
  frc::Spark rightLeader{2};
  frc::Spark rightFollower{3};
  frc::DifferentialDrive drive{[&](double output) { leftLeader.Set(output); },
                               [&](double output) { rightLeader.Set(output); }};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
