/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/ArrayRef.h>
#include <llvm/raw_ostream.h>

#include "MotorSafety.h"
#include "MotorSafetyHelper.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

class SpeedController;

/**
 * Common base class for drive platforms.
 */
class RobotDriveBase : public MotorSafety, public SendableBase {
 public:
  /**
   * The location of a motor on the robot for the purpose of driving.
   */
  enum MotorType {
    kFrontLeft = 0,
    kFrontRight = 1,
    kRearLeft = 2,
    kRearRight = 3,
    kLeft = 0,
    kRight = 1,
    kBack = 2
  };

  RobotDriveBase();
  ~RobotDriveBase() override = default;

  RobotDriveBase(const RobotDriveBase&) = delete;
  RobotDriveBase& operator=(const RobotDriveBase&) = delete;

  void SetDeadband(double deadband);
  void SetMaxOutput(double maxOutput);

  void SetExpiration(double timeout) override;
  double GetExpiration() const override;
  bool IsAlive() const override;
  void StopMotor() override = 0;
  bool IsSafetyEnabled() const override;
  void SetSafetyEnabled(bool enabled) override;
  void GetDescription(llvm::raw_ostream& desc) const override = 0;

 protected:
  double Limit(double number);
  double ApplyDeadband(double number, double deadband);
  void Normalize(llvm::MutableArrayRef<double> wheelSpeeds);

  double m_deadband = 0.02;
  double m_maxOutput = 1.0;
  MotorSafetyHelper m_safetyHelper{this};
};

}  // namespace frc
