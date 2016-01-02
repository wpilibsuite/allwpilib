/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ErrorBase.h"
#include <stdlib.h>
#include <memory>
#include <sstream>
#include "HAL/HAL.hpp"
#include "MotorSafety.h"
#include "MotorSafetyHelper.h"

class SpeedController;
class GenericHID;

/**
 * Utility class for handling Robot drive based on a definition of the motor
 * configuration.
 * The robot drive class handles basic driving for a robot. Currently, 2 and 4
 * motor tank and
 * mecanum drive trains are supported. In the future other drive types like
 * swerve might be
 * implemented. Motor channel numbers are passed supplied on creation of the
 * class. Those
 * are used for either the Drive function (intended for hand created drive code,
 * such as
 * autonomous) or with the Tank/Arcade functions intended to be used for
 * Operator Control
 * driving.
 */
class RobotDrive : public MotorSafety, public ErrorBase {
 public:
  enum MotorType {
    kFrontLeftMotor = 0,
    kFrontRightMotor = 1,
    kRearLeftMotor = 2,
    kRearRightMotor = 3
  };

  RobotDrive(uint32_t leftMotorChannel, uint32_t rightMotorChannel);
  RobotDrive(uint32_t frontLeftMotorChannel, uint32_t rearLeftMotorChannel,
             uint32_t frontRightMotorChannel, uint32_t rearRightMotorChannel);
  RobotDrive(SpeedController *leftMotor, SpeedController *rightMotor);
  RobotDrive(SpeedController &leftMotor, SpeedController &rightMotor);
  RobotDrive(std::shared_ptr<SpeedController> leftMotor,
             std::shared_ptr<SpeedController> rightMotor);
  RobotDrive(SpeedController *frontLeftMotor, SpeedController *rearLeftMotor,
             SpeedController *frontRightMotor, SpeedController *rearRightMotor);
  RobotDrive(SpeedController &frontLeftMotor, SpeedController &rearLeftMotor,
             SpeedController &frontRightMotor, SpeedController &rearRightMotor);
  RobotDrive(std::shared_ptr<SpeedController> frontLeftMotor,
             std::shared_ptr<SpeedController> rearLeftMotor,
             std::shared_ptr<SpeedController> frontRightMotor,
             std::shared_ptr<SpeedController> rearRightMotor);
  virtual ~RobotDrive() = default;

  RobotDrive(const RobotDrive&) = delete;
  RobotDrive& operator=(const RobotDrive&) = delete;

  void Drive(float outputMagnitude, float curve);
  void TankDrive(GenericHID *leftStick, GenericHID *rightStick,
                 bool squaredInputs = true);
  void TankDrive(GenericHID &leftStick, GenericHID &rightStick,
                 bool squaredInputs = true);
  void TankDrive(GenericHID *leftStick, uint32_t leftAxis,
                 GenericHID *rightStick, uint32_t rightAxis,
                 bool squaredInputs = true);
  void TankDrive(GenericHID &leftStick, uint32_t leftAxis,
                 GenericHID &rightStick, uint32_t rightAxis,
                 bool squaredInputs = true);
  void TankDrive(float leftValue, float rightValue, bool squaredInputs = true);
  void ArcadeDrive(GenericHID *stick, bool squaredInputs = true);
  void ArcadeDrive(GenericHID &stick, bool squaredInputs = true);
  void ArcadeDrive(GenericHID *moveStick, uint32_t moveChannel,
                   GenericHID *rotateStick, uint32_t rotateChannel,
                   bool squaredInputs = true);
  void ArcadeDrive(GenericHID &moveStick, uint32_t moveChannel,
                   GenericHID &rotateStick, uint32_t rotateChannel,
                   bool squaredInputs = true);
  void ArcadeDrive(float moveValue, float rotateValue,
                   bool squaredInputs = true);
  void MecanumDrive_Cartesian(float x, float y, float rotation,
                              float gyroAngle = 0.0);
  void MecanumDrive_Polar(float magnitude, float direction, float rotation);
  void HolonomicDrive(float magnitude, float direction, float rotation);
  virtual void SetLeftRightMotorOutputs(float leftOutput, float rightOutput);
  void SetInvertedMotor(MotorType motor, bool isInverted);
  void SetSensitivity(float sensitivity);
  void SetMaxOutput(double maxOutput);
  void SetCANJaguarSyncGroup(uint8_t syncGroup);

  void SetExpiration(float timeout) override;
  float GetExpiration() const override;
  bool IsAlive() const override;
  void StopMotor() override;
  bool IsSafetyEnabled() const override;
  void SetSafetyEnabled(bool enabled) override;
  void GetDescription(std::ostringstream& desc) const override;

 protected:
  void InitRobotDrive();
  float Limit(float num);
  void Normalize(double *wheelSpeeds);
  void RotateVector(double &x, double &y, double angle);

  static const int32_t kMaxNumberOfMotors = 4;
  float m_sensitivity = 0.5;
  double m_maxOutput = 1.0;
  std::shared_ptr<SpeedController> m_frontLeftMotor;
  std::shared_ptr<SpeedController> m_frontRightMotor;
  std::shared_ptr<SpeedController> m_rearLeftMotor;
  std::shared_ptr<SpeedController> m_rearRightMotor;
  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;
  uint8_t m_syncGroup = 0;

 private:
  int32_t GetNumMotors() {
    int motors = 0;
    if (m_frontLeftMotor) motors++;
    if (m_frontRightMotor) motors++;
    if (m_rearLeftMotor) motors++;
    if (m_rearRightMotor) motors++;
    return motors;
  }
};
