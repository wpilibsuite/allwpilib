/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/RobotDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/GenericHID.h"
#include "frc/Joystick.h"
#include "frc/Talon.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

static std::shared_ptr<SpeedController> make_shared_nodelete(
    SpeedController* ptr) {
  return std::shared_ptr<SpeedController>(ptr, NullDeleter<SpeedController>());
}

RobotDrive::RobotDrive(int leftMotorChannel, int rightMotorChannel) {
  InitRobotDrive();
  m_rearLeftMotor = std::make_shared<Talon>(leftMotorChannel);
  m_rearRightMotor = std::make_shared<Talon>(rightMotorChannel);
  SetLeftRightMotorOutputs(0.0, 0.0);
}

RobotDrive::RobotDrive(int frontLeftMotor, int rearLeftMotor,
                       int frontRightMotor, int rearRightMotor) {
  InitRobotDrive();
  m_rearLeftMotor = std::make_shared<Talon>(rearLeftMotor);
  m_rearRightMotor = std::make_shared<Talon>(rearRightMotor);
  m_frontLeftMotor = std::make_shared<Talon>(frontLeftMotor);
  m_frontRightMotor = std::make_shared<Talon>(frontRightMotor);
  SetLeftRightMotorOutputs(0.0, 0.0);
}

RobotDrive::RobotDrive(SpeedController* leftMotor,
                       SpeedController* rightMotor) {
  InitRobotDrive();
  if (leftMotor == nullptr || rightMotor == nullptr) {
    wpi_setWPIError(NullParameter);
    m_rearLeftMotor = m_rearRightMotor = nullptr;
    return;
  }
  m_rearLeftMotor = make_shared_nodelete(leftMotor);
  m_rearRightMotor = make_shared_nodelete(rightMotor);
}

RobotDrive::RobotDrive(SpeedController& leftMotor,
                       SpeedController& rightMotor) {
  InitRobotDrive();
  m_rearLeftMotor = make_shared_nodelete(&leftMotor);
  m_rearRightMotor = make_shared_nodelete(&rightMotor);
}

RobotDrive::RobotDrive(std::shared_ptr<SpeedController> leftMotor,
                       std::shared_ptr<SpeedController> rightMotor) {
  InitRobotDrive();
  if (leftMotor == nullptr || rightMotor == nullptr) {
    wpi_setWPIError(NullParameter);
    m_rearLeftMotor = m_rearRightMotor = nullptr;
    return;
  }
  m_rearLeftMotor = leftMotor;
  m_rearRightMotor = rightMotor;
}

RobotDrive::RobotDrive(SpeedController* frontLeftMotor,
                       SpeedController* rearLeftMotor,
                       SpeedController* frontRightMotor,
                       SpeedController* rearRightMotor) {
  InitRobotDrive();
  if (frontLeftMotor == nullptr || rearLeftMotor == nullptr ||
      frontRightMotor == nullptr || rearRightMotor == nullptr) {
    wpi_setWPIError(NullParameter);
    return;
  }
  m_frontLeftMotor = make_shared_nodelete(frontLeftMotor);
  m_rearLeftMotor = make_shared_nodelete(rearLeftMotor);
  m_frontRightMotor = make_shared_nodelete(frontRightMotor);
  m_rearRightMotor = make_shared_nodelete(rearRightMotor);
}

RobotDrive::RobotDrive(SpeedController& frontLeftMotor,
                       SpeedController& rearLeftMotor,
                       SpeedController& frontRightMotor,
                       SpeedController& rearRightMotor) {
  InitRobotDrive();
  m_frontLeftMotor = make_shared_nodelete(&frontLeftMotor);
  m_rearLeftMotor = make_shared_nodelete(&rearLeftMotor);
  m_frontRightMotor = make_shared_nodelete(&frontRightMotor);
  m_rearRightMotor = make_shared_nodelete(&rearRightMotor);
}

RobotDrive::RobotDrive(std::shared_ptr<SpeedController> frontLeftMotor,
                       std::shared_ptr<SpeedController> rearLeftMotor,
                       std::shared_ptr<SpeedController> frontRightMotor,
                       std::shared_ptr<SpeedController> rearRightMotor) {
  InitRobotDrive();
  if (frontLeftMotor == nullptr || rearLeftMotor == nullptr ||
      frontRightMotor == nullptr || rearRightMotor == nullptr) {
    wpi_setWPIError(NullParameter);
    return;
  }
  m_frontLeftMotor = frontLeftMotor;
  m_rearLeftMotor = rearLeftMotor;
  m_frontRightMotor = frontRightMotor;
  m_rearRightMotor = rearRightMotor;
}

void RobotDrive::Drive(double outputMagnitude, double curve) {
  double leftOutput, rightOutput;
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(),
               HALUsageReporting::kRobotDrive_ArcadeRatioCurve);
    reported = true;
  }

  if (curve < 0) {
    double value = std::log(-curve);
    double ratio = (value - m_sensitivity) / (value + m_sensitivity);
    if (ratio == 0) ratio = .0000000001;
    leftOutput = outputMagnitude / ratio;
    rightOutput = outputMagnitude;
  } else if (curve > 0) {
    double value = std::log(curve);
    double ratio = (value - m_sensitivity) / (value + m_sensitivity);
    if (ratio == 0) ratio = .0000000001;
    leftOutput = outputMagnitude;
    rightOutput = outputMagnitude / ratio;
  } else {
    leftOutput = outputMagnitude;
    rightOutput = outputMagnitude;
  }
  SetLeftRightMotorOutputs(leftOutput, rightOutput);
}

void RobotDrive::TankDrive(GenericHID* leftStick, GenericHID* rightStick,
                           bool squaredInputs) {
  if (leftStick == nullptr || rightStick == nullptr) {
    wpi_setWPIError(NullParameter);
    return;
  }
  TankDrive(leftStick->GetY(), rightStick->GetY(), squaredInputs);
}

void RobotDrive::TankDrive(GenericHID& leftStick, GenericHID& rightStick,
                           bool squaredInputs) {
  TankDrive(leftStick.GetY(), rightStick.GetY(), squaredInputs);
}

void RobotDrive::TankDrive(GenericHID* leftStick, int leftAxis,
                           GenericHID* rightStick, int rightAxis,
                           bool squaredInputs) {
  if (leftStick == nullptr || rightStick == nullptr) {
    wpi_setWPIError(NullParameter);
    return;
  }
  TankDrive(leftStick->GetRawAxis(leftAxis), rightStick->GetRawAxis(rightAxis),
            squaredInputs);
}

void RobotDrive::TankDrive(GenericHID& leftStick, int leftAxis,
                           GenericHID& rightStick, int rightAxis,
                           bool squaredInputs) {
  TankDrive(leftStick.GetRawAxis(leftAxis), rightStick.GetRawAxis(rightAxis),
            squaredInputs);
}

void RobotDrive::TankDrive(double leftValue, double rightValue,
                           bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(),
               HALUsageReporting::kRobotDrive_Tank);
    reported = true;
  }

  leftValue = Limit(leftValue);
  rightValue = Limit(rightValue);

  // square the inputs (while preserving the sign) to increase fine control
  // while permitting full power
  if (squaredInputs) {
    leftValue = std::copysign(leftValue * leftValue, leftValue);
    rightValue = std::copysign(rightValue * rightValue, rightValue);
  }

  SetLeftRightMotorOutputs(leftValue, rightValue);
}

void RobotDrive::ArcadeDrive(GenericHID* stick, bool squaredInputs) {
  // simply call the full-featured ArcadeDrive with the appropriate values
  ArcadeDrive(stick->GetY(), stick->GetX(), squaredInputs);
}

void RobotDrive::ArcadeDrive(GenericHID& stick, bool squaredInputs) {
  // simply call the full-featured ArcadeDrive with the appropriate values
  ArcadeDrive(stick.GetY(), stick.GetX(), squaredInputs);
}

void RobotDrive::ArcadeDrive(GenericHID* moveStick, int moveAxis,
                             GenericHID* rotateStick, int rotateAxis,
                             bool squaredInputs) {
  double moveValue = moveStick->GetRawAxis(moveAxis);
  double rotateValue = rotateStick->GetRawAxis(rotateAxis);

  ArcadeDrive(moveValue, rotateValue, squaredInputs);
}

void RobotDrive::ArcadeDrive(GenericHID& moveStick, int moveAxis,
                             GenericHID& rotateStick, int rotateAxis,
                             bool squaredInputs) {
  double moveValue = moveStick.GetRawAxis(moveAxis);
  double rotateValue = rotateStick.GetRawAxis(rotateAxis);

  ArcadeDrive(moveValue, rotateValue, squaredInputs);
}

void RobotDrive::ArcadeDrive(double moveValue, double rotateValue,
                             bool squaredInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(),
               HALUsageReporting::kRobotDrive_ArcadeStandard);
    reported = true;
  }

  // local variables to hold the computed PWM values for the motors
  double leftMotorOutput;
  double rightMotorOutput;

  moveValue = Limit(moveValue);
  rotateValue = Limit(rotateValue);

  // square the inputs (while preserving the sign) to increase fine control
  // while permitting full power
  if (squaredInputs) {
    moveValue = std::copysign(moveValue * moveValue, moveValue);
    rotateValue = std::copysign(rotateValue * rotateValue, rotateValue);
  }

  if (moveValue > 0.0) {
    if (rotateValue > 0.0) {
      leftMotorOutput = moveValue - rotateValue;
      rightMotorOutput = std::max(moveValue, rotateValue);
    } else {
      leftMotorOutput = std::max(moveValue, -rotateValue);
      rightMotorOutput = moveValue + rotateValue;
    }
  } else {
    if (rotateValue > 0.0) {
      leftMotorOutput = -std::max(-moveValue, rotateValue);
      rightMotorOutput = moveValue + rotateValue;
    } else {
      leftMotorOutput = moveValue - rotateValue;
      rightMotorOutput = -std::max(-moveValue, -rotateValue);
    }
  }
  SetLeftRightMotorOutputs(leftMotorOutput, rightMotorOutput);
}

void RobotDrive::MecanumDrive_Cartesian(double x, double y, double rotation,
                                        double gyroAngle) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(),
               HALUsageReporting::kRobotDrive_MecanumCartesian);
    reported = true;
  }

  double xIn = x;
  double yIn = y;
  // Negate y for the joystick.
  yIn = -yIn;
  // Compenstate for gyro angle.
  RotateVector(xIn, yIn, gyroAngle);

  double wheelSpeeds[kMaxNumberOfMotors];
  wheelSpeeds[kFrontLeftMotor] = xIn + yIn + rotation;
  wheelSpeeds[kFrontRightMotor] = -xIn + yIn - rotation;
  wheelSpeeds[kRearLeftMotor] = -xIn + yIn + rotation;
  wheelSpeeds[kRearRightMotor] = xIn + yIn - rotation;

  Normalize(wheelSpeeds);

  m_frontLeftMotor->Set(wheelSpeeds[kFrontLeftMotor] * m_maxOutput);
  m_frontRightMotor->Set(wheelSpeeds[kFrontRightMotor] * m_maxOutput);
  m_rearLeftMotor->Set(wheelSpeeds[kRearLeftMotor] * m_maxOutput);
  m_rearRightMotor->Set(wheelSpeeds[kRearRightMotor] * m_maxOutput);

  Feed();
}

void RobotDrive::MecanumDrive_Polar(double magnitude, double direction,
                                    double rotation) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, GetNumMotors(),
               HALUsageReporting::kRobotDrive_MecanumPolar);
    reported = true;
  }

  // Normalized for full power along the Cartesian axes.
  magnitude = Limit(magnitude) * std::sqrt(2.0);
  // The rollers are at 45 degree angles.
  double dirInRad = (direction + 45.0) * 3.14159 / 180.0;
  double cosD = std::cos(dirInRad);
  double sinD = std::sin(dirInRad);

  double wheelSpeeds[kMaxNumberOfMotors];
  wheelSpeeds[kFrontLeftMotor] = sinD * magnitude + rotation;
  wheelSpeeds[kFrontRightMotor] = cosD * magnitude - rotation;
  wheelSpeeds[kRearLeftMotor] = cosD * magnitude + rotation;
  wheelSpeeds[kRearRightMotor] = sinD * magnitude - rotation;

  Normalize(wheelSpeeds);

  m_frontLeftMotor->Set(wheelSpeeds[kFrontLeftMotor] * m_maxOutput);
  m_frontRightMotor->Set(wheelSpeeds[kFrontRightMotor] * m_maxOutput);
  m_rearLeftMotor->Set(wheelSpeeds[kRearLeftMotor] * m_maxOutput);
  m_rearRightMotor->Set(wheelSpeeds[kRearRightMotor] * m_maxOutput);

  Feed();
}

void RobotDrive::HolonomicDrive(double magnitude, double direction,
                                double rotation) {
  MecanumDrive_Polar(magnitude, direction, rotation);
}

void RobotDrive::SetLeftRightMotorOutputs(double leftOutput,
                                          double rightOutput) {
  wpi_assert(m_rearLeftMotor != nullptr && m_rearRightMotor != nullptr);

  if (m_frontLeftMotor != nullptr)
    m_frontLeftMotor->Set(Limit(leftOutput) * m_maxOutput);
  m_rearLeftMotor->Set(Limit(leftOutput) * m_maxOutput);

  if (m_frontRightMotor != nullptr)
    m_frontRightMotor->Set(-Limit(rightOutput) * m_maxOutput);
  m_rearRightMotor->Set(-Limit(rightOutput) * m_maxOutput);

  Feed();
}

void RobotDrive::SetInvertedMotor(MotorType motor, bool isInverted) {
  if (motor < 0 || motor > 3) {
    wpi_setWPIError(InvalidMotorIndex);
    return;
  }
  switch (motor) {
    case kFrontLeftMotor:
      m_frontLeftMotor->SetInverted(isInverted);
      break;
    case kFrontRightMotor:
      m_frontRightMotor->SetInverted(isInverted);
      break;
    case kRearLeftMotor:
      m_rearLeftMotor->SetInverted(isInverted);
      break;
    case kRearRightMotor:
      m_rearRightMotor->SetInverted(isInverted);
      break;
  }
}

void RobotDrive::SetSensitivity(double sensitivity) {
  m_sensitivity = sensitivity;
}

void RobotDrive::SetMaxOutput(double maxOutput) { m_maxOutput = maxOutput; }

void RobotDrive::GetDescription(wpi::raw_ostream& desc) const {
  desc << "RobotDrive";
}

void RobotDrive::StopMotor() {
  if (m_frontLeftMotor != nullptr) m_frontLeftMotor->StopMotor();
  if (m_frontRightMotor != nullptr) m_frontRightMotor->StopMotor();
  if (m_rearLeftMotor != nullptr) m_rearLeftMotor->StopMotor();
  if (m_rearRightMotor != nullptr) m_rearRightMotor->StopMotor();
  Feed();
}

void RobotDrive::InitRobotDrive() { SetSafetyEnabled(true); }

double RobotDrive::Limit(double number) {
  if (number > 1.0) {
    return 1.0;
  }
  if (number < -1.0) {
    return -1.0;
  }
  return number;
}

void RobotDrive::Normalize(double* wheelSpeeds) {
  double maxMagnitude = std::fabs(wheelSpeeds[0]);
  for (int i = 1; i < kMaxNumberOfMotors; i++) {
    double temp = std::fabs(wheelSpeeds[i]);
    if (maxMagnitude < temp) maxMagnitude = temp;
  }
  if (maxMagnitude > 1.0) {
    for (int i = 0; i < kMaxNumberOfMotors; i++) {
      wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
    }
  }
}

void RobotDrive::RotateVector(double& x, double& y, double angle) {
  double cosA = std::cos(angle * (3.14159 / 180.0));
  double sinA = std::sin(angle * (3.14159 / 180.0));
  double xOut = x * cosA - y * sinA;
  double yOut = x * sinA + y * cosA;
  x = xOut;
  y = yOut;
}
