/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/Twine.h>

#include "Commands/Command.h"
#include "PIDController.h"
#include "PIDOutput.h"
#include "PIDSource.h"

namespace frc {

class PIDCommand : public Command, public PIDOutput, public PIDSource {
 public:
  PIDCommand(const llvm::Twine& name, double p, double i, double d);
  PIDCommand(const llvm::Twine& name, double p, double i, double d,
             double period);
  PIDCommand(const llvm::Twine& name, double p, double i, double d, double f,
             double period);
  PIDCommand(double p, double i, double d);
  PIDCommand(double p, double i, double d, double period);
  PIDCommand(double p, double i, double d, double f, double period);
  virtual ~PIDCommand() = default;

  void SetSetpointRelative(double deltaSetpoint);

  // PIDOutput interface
  void PIDWrite(double output) override;

  // PIDSource interface
  double PIDGet() override;

 protected:
  std::shared_ptr<PIDController> GetPIDController() const;
  void _Initialize() override;
  void _Interrupted() override;
  void _End() override;
  void SetSetpoint(double setpoint);
  double GetSetpoint() const;
  double GetPosition();

  virtual double ReturnPIDInput() = 0;
  virtual void UsePIDOutput(double output) = 0;

 private:
  // The internal PIDController
  std::shared_ptr<PIDController> m_controller;

 public:
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
