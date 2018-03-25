/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <limits>

#include <HAL/HAL.h>

#include "CtrlSys/GainNode.h"
#include "CtrlSys/INode.h"
#include "CtrlSys/Output.h"
#include "CtrlSys/PIDNode.h"
#include "CtrlSys/RefInput.h"
#include "CtrlSys/SumNode.h"
#include "PIDInterface.h"
#include "PIDOutput.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

/**
 * Class implements a PID Control Loop.
 *
 * Creates a separate thread which reads the given PIDSource and takes
 * care of the integral calculations, as well as writing the given PIDOutput.
 *
 * This feedback controller runs in discrete time, so time deltas are not used
 * in the integral and derivative calculations. Therefore, the sample rate
 * affects the controller's behavior for a given set of PID constants.
 */
class PIDController : public SendableBase, public PIDInterface {
 public:
  PIDController(double Kp, double Ki, double Kd, INode& input,
                PIDOutput& output, double period = INode::kDefaultPeriod);
  PIDController(double Kp, double Ki, double Kd, double Kff, INode& input,
                PIDOutput& output, double period = INode::kDefaultPeriod);
  virtual ~PIDController() = default;

  PIDController(const PIDController&) = delete;
  PIDController& operator=(const PIDController) = delete;

  void SetPID(double Kp, double Ki, double Kd) override;
  void SetPID(double Kp, double Ki, double Kd, double Kff);
  double GetP() const override;
  double GetI() const override;
  double GetD() const override;
  double GetF() const;

  void SetContinuous(bool continuous = true);
  void SetInputRange(double minimumInput, double maximumInput);
  void SetOutputRange(double minimumOutput, double maximumOutput);
  void SetIZone(double maxErrorMagnitude);

  void SetSetpoint(double setpoint) override;
  double GetSetpoint() const override;

  void SetAbsoluteTolerance(
      double tolerance,
      double deltaTolerance = std::numeric_limits<double>::infinity());
  double GetError();
  bool OnTarget() const;

  void Enable() override;
  void Disable() override;
  bool IsEnabled() const override;
  void SetEnabled(bool enable);

  void Reset() override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  RefInput m_refInput{0.0};
  GainNode m_feedforward{0.0, m_refInput};
  SumNode m_sum;
  PIDNode m_pid;
  Output m_output;
  double m_tolerance = 0.05;
  bool m_enabled = false;
};

}  // namespace frc
