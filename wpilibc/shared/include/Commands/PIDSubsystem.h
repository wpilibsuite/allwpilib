/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __PID_SUBSYSTEM_H__
#define __PID_SUBSYSTEM_H__

#include "Commands/Subsystem.h"
#include "PIDController.h"
#include "PIDSource.h"
#include "PIDOutput.h"

#include <memory>

/**
 * This class is designed to handle the case where there is a {@link Subsystem}
 * which uses a single {@link PIDController} almost constantly (for instance,
 * an elevator which attempts to stay at a constant height).
 *
 * <p>It provides some convenience methods to run an internal {@link
 * PIDController}.
 * It also allows access to the internal {@link PIDController} in order to give
 * total control
 * to the programmer.</p>
 *
 */
class PIDSubsystem : public Subsystem, public PIDOutput, public PIDSource {
 public:
  PIDSubsystem(const std::string &name, double p, double i, double d);
  PIDSubsystem(const std::string &name, double p, double i, double d, double f);
  PIDSubsystem(const std::string &name, double p, double i, double d, double f,
               double period);
  PIDSubsystem(double p, double i, double d);
  PIDSubsystem(double p, double i, double d, double f);
  PIDSubsystem(double p, double i, double d, double f, double period);
  virtual ~PIDSubsystem() = default;

  void Enable();
  void Disable();

  // PIDOutput interface
  virtual void PIDWrite(float output);

  // PIDSource interface
  virtual double PIDGet();
  void SetSetpoint(double setpoint);
  void SetSetpointRelative(double deltaSetpoint);
  void SetInputRange(float minimumInput, float maximumInput);
  void SetOutputRange(float minimumOutput, float maximumOutput);
  double GetSetpoint();
  double GetPosition();
  double GetRate();

  virtual void SetAbsoluteTolerance(float absValue);
  virtual void SetPercentTolerance(float percent);
  virtual bool OnTarget() const;

 protected:
  std::shared_ptr<PIDController> GetPIDController();

  virtual double ReturnPIDInput() = 0;
  virtual void UsePIDOutput(double output) = 0;

 private:
  /** The internal {@link PIDController} */
  std::shared_ptr<PIDController> m_controller;

 public:
  virtual void InitTable(std::shared_ptr<ITable> table);
  virtual std::string GetSmartDashboardType() const;
};

#endif
