/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/time.h>
#include <units/voltage.h>
#include <wpi/math>

/**
 * The Constants header provides a convenient place for teams to hold robot-wide
 * numerical or bool constants.  This should not be used for any other purpose.
 *
 * It is generally a good idea to place constants into subsystem- or
 * command-specific namespaces within this header, which can then be used where
 * they are needed.
 */

namespace DriveConstants {
constexpr int kLeftMotor1Port = 0;
constexpr int kLeftMotor2Port = 1;
constexpr int kRightMotor1Port = 2;
constexpr int kRightMotor2Port = 3;

constexpr int kLeftEncoderPorts[]{0, 1};
constexpr int kRightEncoderPorts[]{2, 3};
constexpr bool kLeftEncoderReversed = false;
constexpr bool kRightEncoderReversed = true;

constexpr int kEncoderCPR = 1024;
constexpr double kWheelDiameterInches = 6;
constexpr double kEncoderDistancePerPulse =
    // Assumes the encoders are directly mounted on the wheel shafts
    (kWheelDiameterInches * wpi::math::pi) / static_cast<double>(kEncoderCPR);
}  // namespace DriveConstants

namespace ArmConstants {
constexpr int kMotorPort = 4;

constexpr double kP = 1;

// These are fake gains; in actuality these must be determined individually for
// each robot
constexpr auto kS = 1_V;
constexpr auto kCos = 1_V;
constexpr auto kV = 0.5_V * 1_s / 1_rad;
constexpr auto kA = 0.1_V * 1_s * 1_s / 1_rad;

constexpr auto kMaxVelocity = 3_rad_per_s;
constexpr auto kMaxAcceleration = 10_rad / (1_s * 1_s);

constexpr int kEncoderPorts[]{4, 5};
constexpr int kEncoderPPR = 256;
constexpr auto kEncoderDistancePerPulse = 2.0_rad * wpi::math::pi / kEncoderPPR;

// The offset of the arm from the horizontal in its neutral position,
// measured from the horizontal
constexpr auto kArmOffset = 0.5_rad;
}  // namespace ArmConstants

namespace AutoConstants {
constexpr auto kAutoTimeoutSeconds = 12_s;
constexpr auto kAutoShootTimeSeconds = 7_s;
}  // namespace AutoConstants

namespace OIConstants {
constexpr int kDriverControllerPort = 1;
}  // namespace OIConstants
