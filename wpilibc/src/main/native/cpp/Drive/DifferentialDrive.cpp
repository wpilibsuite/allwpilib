/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/DifferentialDrive.h"

#include <cmath>

#include <HAL/HAL.h>

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
    : m_leftMotor(leftMotor), m_rightMotor(rightMotor) {}

/**
 * Arcade drive method for differential drive platform.
 *
 * Note: Some drivers may prefer inverted rotation controls. This can be done by
 * negating the value passed for rotation.
 *
 * @param y             The value to use for forwards/backwards. [-1.0..1.0]
 * @param rotation      The value to use for the rotation right/left.
 *                      [-1.0..1.0]
 * @param squaredInputs If set, decreases the input sensitivity at low speeds.
 */
void DifferentialDrive::ArcadeDrive(double y, double rotation,
                                    bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive_ArcadeStandard);
    reported = true;
  }

  y = Limit(y);
  y = ApplyDeadband(y, m_deadband);

  rotation = Limit(rotation);
  rotation = ApplyDeadband(rotation, m_deadband);

  // square the inputs (while preserving the sign) to increase fine control
  // while permitting full power
  if (squaredInputs) {
    y = std::copysign(y * y, y);
    rotation = std::copysign(rotation * rotation, rotation);
  }

  double leftMotorOutput;
  double rightMotorOutput;

  double maxInput = std::copysign(std::max(std::abs(y), std::abs(rotation)), y);

  if (y > 0.0) {
    // First quadrant, else second quadrant
    if (rotation > 0.0) {
      leftMotorOutput = maxInput;
      rightMotorOutput = y - rotation;
    } else {
      leftMotorOutput = y + rotation;
      rightMotorOutput = maxInput;
    }
  } else {
    // Third quadrant, else fourth quadrant
    if (rotation > 0.0) {
      leftMotorOutput = y + rotation;
      rightMotorOutput = maxInput;
    } else {
      leftMotorOutput = maxInput;
      rightMotorOutput = y - rotation;
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
 * @param y           The value to use for forwards/backwards. [-1.0..1.0]
 * @param rotation    The value to use for the rotation right/left. [-1.0..1.0]
 * @param isQuickTurn If set, overrides constant-curvature turning for
 *                    turn-in-place maneuvers.
 */
void DifferentialDrive::CurvatureDrive(double y, double rotation,
                                       bool isQuickTurn) {
  static bool reported = false;
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
    //            HALUsageReporting::kRobotDrive_Curvature);
    reported = true;
  }

  y = Limit(y);
  y = ApplyDeadband(y, m_deadband);

  rotation = Limit(rotation);
  rotation = ApplyDeadband(rotation, m_deadband);

  double angularPower;
  bool overPower;

  if (isQuickTurn) {
    if (std::abs(y) < 0.2) {
      constexpr double alpha = 0.1;
      m_quickStopAccumulator =
          (1 - alpha) * m_quickStopAccumulator + alpha * Limit(rotation) * 2;
    }
    overPower = true;
    angularPower = rotation;
  } else {
    overPower = false;
    angularPower = std::abs(y) * rotation - m_quickStopAccumulator;

    if (m_quickStopAccumulator > 1) {
      m_quickStopAccumulator -= 1;
    } else if (m_quickStopAccumulator < -1) {
      m_quickStopAccumulator += 1;
    } else {
      m_quickStopAccumulator = 0.0;
    }
  }

  double leftMotorOutput = y + angularPower;
  double rightMotorOutput = y - angularPower;

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
 * @param left  The value to use for left side motors. [-1.0..1.0]
 * @param right The value to use for right side motors. [-1.0..1.0]
 * @param squaredInputs If set, decreases the input sensitivity at low speeds.
 */
void DifferentialDrive::TankDrive(double left, double right,
                                  bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive_Tank);
    reported = true;
  }

  left = Limit(left);
  left = ApplyDeadband(left, m_deadband);

  right = Limit(right);
  right = ApplyDeadband(right, m_deadband);

  // square the inputs (while preserving the sign) to increase fine control
  // while permitting full power
  if (squaredInputs) {
    left = std::copysign(left * left, left);
    right = std::copysign(right * right, right);
  }

  m_leftMotor.Set(left * m_maxOutput);
  m_rightMotor.Set(-right * m_maxOutput);

  m_safetyHelper.Feed();
}

void DifferentialDrive::StopMotor() {
  m_leftMotor.StopMotor();
  m_rightMotor.StopMotor();
  m_safetyHelper.Feed();
}

void DifferentialDrive::GetDescription(llvm::raw_ostream& desc) const {
  desc << "DifferentialDrive";
}
