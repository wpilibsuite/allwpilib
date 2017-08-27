/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/raw_ostream.h>

#include "ErrorBase.h"
#include "MotorSafety.h"
#include "MotorSafetyHelper.h"

namespace frc {

class SpeedController;
class GenericHID;

/**
 * Utility class for handling Robot drive based on a definition of the motor
 * configuration.
 * The robot drive class handles basic driving for a robot. Currently, 2 and 4
 * motor tank and mecanum drive trains are supported. In the future other drive
 * types like swerve might be implemented. Motor channel numbers are passed
 * supplied on creation of the class. Those are used for either the Drive
 * function (intended for hand created drive code, such as autonomous) or with
 * the Tank/Arcade functions intended to be used for Operator Control driving.
 */
class RobotDrive : public MotorSafety, public ErrorBase {
 public:
  enum MotorType {
    kFrontLeftMotor = 0,
    kFrontRightMotor = 1,
    kRearLeftMotor = 2,
    kRearRightMotor = 3
  };

  RobotDrive(int leftMotorChannel, int rightMotorChannel);
  RobotDrive(int frontLeftMotorChannel, int rearLeftMotorChannel,
             int frontRightMotorChannel, int rearRightMotorChannel);
  RobotDrive(SpeedController* leftMotor, SpeedController* rightMotor);
  RobotDrive(SpeedController& leftMotor, SpeedController& rightMotor);
  RobotDrive(std::shared_ptr<SpeedController> leftMotor,
             std::shared_ptr<SpeedController> rightMotor);
  RobotDrive(SpeedController* frontLeftMotor, SpeedController* rearLeftMotor,
             SpeedController* frontRightMotor, SpeedController* rearRightMotor);
  RobotDrive(SpeedController& frontLeftMotor, SpeedController& rearLeftMotor,
             SpeedController& frontRightMotor, SpeedController& rearRightMotor);
  RobotDrive(std::shared_ptr<SpeedController> frontLeftMotor,
             std::shared_ptr<SpeedController> rearLeftMotor,
             std::shared_ptr<SpeedController> frontRightMotor,
             std::shared_ptr<SpeedController> rearRightMotor);
  virtual ~RobotDrive() = default;

  RobotDrive(const RobotDrive&) = delete;
  RobotDrive& operator=(const RobotDrive&) = delete;

  void Drive(double outputMagnitude, double curve);
  void TankDrive(GenericHID* leftStick, GenericHID* rightStick,
                 bool squaredInputs = true);
  void TankDrive(GenericHID& leftStick, GenericHID& rightStick,
                 bool squaredInputs = true);
  void TankDrive(GenericHID* leftStick, int leftAxis, GenericHID* rightStick,
                 int rightAxis, bool squaredInputs = true);
  void TankDrive(GenericHID& leftStick, int leftAxis, GenericHID& rightStick,
                 int rightAxis, bool squaredInputs = true);
  void TankDrive(double leftValue, double rightValue,
                 bool squaredInputs = true);
  void ArcadeDrive(GenericHID* stick, bool squaredInputs = true);
  void ArcadeDrive(GenericHID& stick, bool squaredInputs = true);
  void ArcadeDrive(GenericHID* moveStick, int moveChannel,
                   GenericHID* rotateStick, int rotateChannel,
                   bool squaredInputs = true);
  void ArcadeDrive(GenericHID& moveStick, int moveChannel,
                   GenericHID& rotateStick, int rotateChannel,
                   bool squaredInputs = true);
  void ArcadeDrive(double moveValue, double rotateValue,
                   bool squaredInputs = true);
  void MecanumDrive_Cartesian(double x, double y, double rotation,
                              double gyroAngle = 0.0);
  void MecanumDrive_Polar(double magnitude, double direction, double rotation);
  void HolonomicDrive(double magnitude, double direction, double rotation);
  virtual void SetLeftRightMotorOutputs(double leftOutput, double rightOutput);
  void SetInvertedMotor(MotorType motor, bool isInverted);
  void SetSensitivity(double sensitivity);
  void SetMaxOutput(double maxOutput);

  void SetExpiration(double timeout) override;
  double GetExpiration() const override;
  bool IsAlive() const override;
  void StopMotor() override;
  bool IsSafetyEnabled() const override;
  void SetSafetyEnabled(bool enabled) override;
  void GetDescription(llvm::raw_ostream& desc) const override;

 protected:
  void InitRobotDrive();
  double Limit(double number);
  void Normalize(double* wheelSpeeds);
  void RotateVector(double& x, double& y, double angle);

  static const int kMaxNumberOfMotors = 4;
  double m_sensitivity = 0.5;
  double m_maxOutput = 1.0;
  std::shared_ptr<SpeedController> m_frontLeftMotor;
  std::shared_ptr<SpeedController> m_frontRightMotor;
  std::shared_ptr<SpeedController> m_rearLeftMotor;
  std::shared_ptr<SpeedController> m_rearRightMotor;
  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;

 private:
  int GetNumMotors() {
    int motors = 0;
    if (m_frontLeftMotor) motors++;
    if (m_frontRightMotor) motors++;
    if (m_rearLeftMotor) motors++;
    if (m_rearRightMotor) motors++;
    return motors;
  }
};

}  // namespace frc
