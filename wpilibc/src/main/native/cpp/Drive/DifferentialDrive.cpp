/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/DifferentialDrive.h"

#include <cmath>

#include <HAL/HAL.h>

#include "SmartDashboard/SendableBuilder.h"
#include "SpeedController.h"

using namespace frc;

/**
 * Construct a DifferentialDrive.
 *
 * To pass multiple motors per side, use a SpeedControllerGroup. If a motor
 * needs to be inverted, do so before passing it in.
 */
DifferentialDrive::DifferentialDrive(SpeedController& leftMotor,
                                     SpeedController& rightMotor)
    : m_leftMotor(leftMotor), m_rightMotor(rightMotor) {
  AddChild(&m_leftMotor);
  AddChild(&m_rightMotor);
  static int instances = 0;
  ++instances;
  SetName("DifferentialDrive", instances);
}

/**
 * Arcade drive method for differential drive platform.
 *
 * Note: Some drivers may prefer inverted rotation controls. This can be done by
 * negating the value passed for rotation.
 *
 * @param xSpeed        The speed at which the robot should drive along the X
 *                      axis [-1.0..1.0]. Forward is negative.
 * @param zRotation     The rotation rate of the robot around the Z axis
 *                      [-1.0..1.0]. Clockwise is positive.
 * @param squaredInputs If set, decreases the input sensitivity at low speeds.
 */
void DifferentialDrive::ArcadeDrive(double xSpeed, double zRotation,
                                    bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive_ArcadeStandard);
    reported = true;
  }

  xSpeed = Limit(xSpeed);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  zRotation = Limit(zRotation);
  zRotation = ApplyDeadband(zRotation, m_deadband);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squaredInputs) {
    xSpeed = std::copysign(xSpeed * xSpeed, xSpeed);
    zRotation = std::copysign(zRotation * zRotation, zRotation);
  }

  double leftMotorOutput;
  double rightMotorOutput;

  double maxInput =
      std::copysign(std::max(std::abs(xSpeed), std::abs(zRotation)), xSpeed);

  if (xSpeed >= 0.0) {
    // First quadrant, else second quadrant
    if (zRotation >= 0.0) {
      leftMotorOutput = maxInput;
      rightMotorOutput = xSpeed - zRotation;
    } else {
      leftMotorOutput = xSpeed + zRotation;
      rightMotorOutput = maxInput;
    }
  } else {
    // Third quadrant, else fourth quadrant
    if (zRotation >= 0.0) {
      leftMotorOutput = xSpeed + zRotation;
      rightMotorOutput = maxInput;
    } else {
      leftMotorOutput = maxInput;
      rightMotorOutput = xSpeed - zRotation;
    }
  }

  m_leftMotor.Set(Limit(leftMotorOutput) * m_maxOutput);
  m_rightMotor.Set(-Limit(rightMotorOutput) * m_maxOutput);

  m_safetyHelper.Feed();
}

/**
 * Curvature drive method for differential drive platform.
 *
 * The rotation argument controls the curvature of the robot's path rather than
 * its rate of heading change. This makes the robot more controllable at high
 * speeds. Also handles the robot's quick turn functionality - "quick turn"
 * overrides constant-curvature turning for turn-in-place maneuvers.
 *
 * @param xSpeed      The robot's speed along the X axis [-1.0..1.0]. Forward is
 *                    positive.
 * @param zRotation   The robot's rotation rate around the Z axis [-1.0..1.0].
 *                    Clockwise is positive.
 * @param isQuickTurn If set, overrides constant-curvature turning for
 *                    turn-in-place maneuvers.
 */
void DifferentialDrive::CurvatureDrive(double xSpeed, double zRotation,
                                       bool isQuickTurn) {
  static bool reported = false;
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
    //            HALUsageReporting::kRobotDrive_Curvature);
    reported = true;
  }

  xSpeed = Limit(xSpeed);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  zRotation = Limit(zRotation);
  zRotation = ApplyDeadband(zRotation, m_deadband);

  double angularPower;
  bool overPower;

  if (isQuickTurn) {
    if (std::abs(xSpeed) < m_quickStopThreshold) {
      m_quickStopAccumulator = (1 - m_quickStopAlpha) * m_quickStopAccumulator +
                               m_quickStopAlpha * Limit(zRotation) * 2;
    }
    overPower = true;
    angularPower = zRotation;
  } else {
    overPower = false;
    angularPower = std::abs(xSpeed) * zRotation - m_quickStopAccumulator;

    if (m_quickStopAccumulator > 1) {
      m_quickStopAccumulator -= 1;
    } else if (m_quickStopAccumulator < -1) {
      m_quickStopAccumulator += 1;
    } else {
      m_quickStopAccumulator = 0.0;
    }
  }

  double leftMotorOutput = xSpeed + angularPower;
  double rightMotorOutput = xSpeed - angularPower;

  // If rotation is overpowered, reduce both outputs to within acceptable range
  if (overPower) {
    if (leftMotorOutput > 1.0) {
      rightMotorOutput -= leftMotorOutput - 1.0;
      leftMotorOutput = 1.0;
    } else if (rightMotorOutput > 1.0) {
      leftMotorOutput -= rightMotorOutput - 1.0;
      rightMotorOutput = 1.0;
    } else if (leftMotorOutput < -1.0) {
      rightMotorOutput -= leftMotorOutput + 1.0;
      leftMotorOutput = -1.0;
    } else if (rightMotorOutput < -1.0) {
      leftMotorOutput -= rightMotorOutput + 1.0;
      rightMotorOutput = -1.0;
    }
  }

  m_leftMotor.Set(leftMotorOutput * m_maxOutput);
  m_rightMotor.Set(-rightMotorOutput * m_maxOutput);

  m_safetyHelper.Feed();
}

/**
 * Tank drive method for differential drive platform.
 *
 * @param leftSpeed     The robot left side's speed along the X axis
 *                      [-1.0..1.0]. Forward is positive.
 * @param rightSpeed    The robot right side's speed along the X axis
 *                      [-1.0..1.0]. Forward is positive.
 * @param squaredInputs If set, decreases the input sensitivity at low speeds.
 */
void DifferentialDrive::TankDrive(double leftSpeed, double rightSpeed,
                                  bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive_Tank);
    reported = true;
  }

  leftSpeed = Limit(leftSpeed);
  leftSpeed = ApplyDeadband(leftSpeed, m_deadband);

  rightSpeed = Limit(rightSpeed);
  rightSpeed = ApplyDeadband(rightSpeed, m_deadband);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squaredInputs) {
    leftSpeed = std::copysign(leftSpeed * leftSpeed, leftSpeed);
    rightSpeed = std::copysign(rightSpeed * rightSpeed, rightSpeed);
  }

  m_leftMotor.Set(leftSpeed * m_maxOutput);
  m_rightMotor.Set(-rightSpeed * m_maxOutput);

  m_safetyHelper.Feed();
}

/**
 * Sets the QuickStop speed threshold in curvature drive.
 *
 * QuickStop compensates for the robot's moment of inertia when stopping after a
 * QuickTurn.
 *
 * While QuickTurn is enabled, the QuickStop accumulator takes on the rotation
 * rate value outputted by the low-pass filter when the robot's speed along the
 * X axis is below the threshold. When QuickTurn is disabled, the accumulator's
 * value is applied against the computed angular power request to slow the
 * robot's rotation.
 *
 * @param threshold X speed below which quick stop accumulator will receive
 *                  rotation rate values [0..1.0].
 */
void DifferentialDrive::SetQuickStopThreshold(double threshold) {
  m_quickStopThreshold = threshold;
}

/**
 * Sets the low-pass filter gain for QuickStop in curvature drive.
 *
 * The low-pass filter filters incoming rotation rate commands to smooth out
 * high frequency changes.
 *
 * @param alpha Low-pass filter gain [0.0..2.0]. Smaller values result in slower
 *              output changes. Values between 1.0 and 2.0 result in output
 *              oscillation. Values below 0.0 and above 2.0 are unstable.
 */
void DifferentialDrive::SetQuickStopAlpha(double alpha) {
  m_quickStopAlpha = alpha;
}

void DifferentialDrive::StopMotor() {
  m_leftMotor.StopMotor();
  m_rightMotor.StopMotor();
  m_safetyHelper.Feed();
}

void DifferentialDrive::GetDescription(llvm::raw_ostream& desc) const {
  desc << "DifferentialDrive";
}

void DifferentialDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("DifferentialDrive");
  builder.AddDoubleProperty("Left Motor Speed",
                            [=]() { return m_leftMotor.Get(); },
                            [=](double value) { m_leftMotor.Set(value); });
  builder.AddDoubleProperty("Right Motor Speed",
                            [=]() { return -m_rightMotor.Get(); },
                            [=](double value) { m_rightMotor.Set(-value); });
}
