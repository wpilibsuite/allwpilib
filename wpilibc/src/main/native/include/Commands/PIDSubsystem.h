/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/Twine.h>

#include "Commands/Subsystem.h"
#include "PIDController.h"
#include "PIDOutput.h"
#include "PIDSource.h"

namespace frc {

/**
 * This class is designed to handle the case where there is a Subsystem which
 * uses a single PIDController almost constantly (for instance, an elevator
 * which attempts to stay at a constant height).
 *
 * It provides some convenience methods to run an internal PIDController. It
 * also allows access to the internal PIDController in order to give total
 * control to the programmer.
 */
class PIDSubsystem : public Subsystem, public PIDOutput, public PIDSource {
 public:
  PIDSubsystem(const llvm::Twine& name, double p, double i, double d);
  PIDSubsystem(const llvm::Twine& name, double p, double i, double d, double f);
  PIDSubsystem(const llvm::Twine& name, double p, double i, double d, double f,
               double period);
  PIDSubsystem(double p, double i, double d);
  PIDSubsystem(double p, double i, double d, double f);
  PIDSubsystem(double p, double i, double d, double f, double period);
  ~PIDSubsystem() override = default;

  void Enable();
  void Disable();

  // PIDOutput interface
  void PIDWrite(double output) override;

  // PIDSource interface
  double PIDGet() override;
  void SetSetpoint(double setpoint);
  void SetSetpointRelative(double deltaSetpoint);
  void SetInputRange(double minimumInput, double maximumInput);
  void SetOutputRange(double minimumOutput, double maximumOutput);
  double GetSetpoint();
  double GetPosition();
  double GetRate();

  virtual void SetAbsoluteTolerance(double absValue);
  virtual void SetPercentTolerance(double percent);
  virtual bool OnTarget() const;

 protected:
  std::shared_ptr<PIDController> GetPIDController();

  virtual double ReturnPIDInput() = 0;
  virtual void UsePIDOutput(double output) = 0;

 private:
  // The internal PIDController
  std::shared_ptr<PIDController> m_controller;
};

}  // namespace frc
