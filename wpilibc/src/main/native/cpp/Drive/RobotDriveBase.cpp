/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/RobotDriveBase.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

#include <HAL/HAL.h>

#include "Base.h"
#include "SpeedController.h"

using namespace frc;

RobotDriveBase::RobotDriveBase() { m_safetyHelper.SetSafetyEnabled(true); }

/**
 * Change the default value for deadband scaling. The default value is
 * 0.02. Values smaller then the deadband are set to 0, while values
 * larger then the deadband are scaled from 0.0 to 1.0. See ApplyDeadband().
 *
 * @param deadband The deadband to set.
 */
void RobotDriveBase::SetDeadband(double deadband) { m_deadband = deadband; }

/**
 * Configure the scaling factor for using RobotDrive with motor controllers in a
 * mode other than PercentVbus or to limit the maximum output.
 *
 * @param maxOutput Multiplied with the output percentage computed by the drive
 *                  functions.
 */
void RobotDriveBase::SetMaxOutput(double maxOutput) { m_maxOutput = maxOutput; }

void RobotDriveBase::SetExpiration(double timeout) {
  m_safetyHelper.SetExpiration(timeout);
}

double RobotDriveBase::GetExpiration() const {
  return m_safetyHelper.GetExpiration();
}

bool RobotDriveBase::IsAlive() const { return m_safetyHelper.IsAlive(); }

bool RobotDriveBase::IsSafetyEnabled() const {
  return m_safetyHelper.IsSafetyEnabled();
}

void RobotDriveBase::SetSafetyEnabled(bool enabled) {
  m_safetyHelper.SetSafetyEnabled(enabled);
}

/**
 * Limit motor values to the -1.0 to +1.0 range.
 */
double RobotDriveBase::Limit(double value) {
  if (value > 1.0) {
    return 1.0;
  }
  if (value < -1.0) {
    return -1.0;
  }
  return value;
}

/**
 * Returns 0.0 if the given value is within the specified range around zero. The
 * remaining range between the deadband and 1.0 is scaled from 0.0 to 1.0.
 *
 * @param value    value to clip
 * @param deadband range around zero
 */
double RobotDriveBase::ApplyDeadband(double value, double deadband) {
  if (std::abs(value) > deadband) {
    if (value > 0.0) {
      return (value - deadband) / (1.0 - deadband);
    } else {
      return (value + deadband) / (1.0 - deadband);
    }
  } else {
    return 0.0;
  }
}

/**
 * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
 */
void RobotDriveBase::Normalize(llvm::MutableArrayRef<double> wheelSpeeds) {
  double maxMagnitude = std::abs(wheelSpeeds[0]);
  for (size_t i = 1; i < wheelSpeeds.size(); i++) {
    double temp = std::abs(wheelSpeeds[i]);
    if (maxMagnitude < temp) {
      maxMagnitude = temp;
    }
  }
  if (maxMagnitude > 1.0) {
    for (size_t i = 0; i < wheelSpeeds.size(); i++) {
      wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
    }
  }
}
