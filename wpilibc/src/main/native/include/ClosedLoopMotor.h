/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "CtrlSys/GainNode.h"
#include "CtrlSys/INode.h"
#include "CtrlSys/Output.h"
#include "CtrlSys/PIDNode.h"
#include "CtrlSys/RefInput.h"
#include "CtrlSys/SumNode.h"
#include "PIDSource.h"
#include "SmartDashboard/SendableBase.h"
#include "SmartDashboard/SendableBuilder.h"
#include "SpeedController.h"

namespace frc {

/**
 * A class that performs closed-loop control on a motor.
 *
 * It implements SpeedController so this control can be used transparently
 * within drive bases.
 *
 * Call either SetDisplacementRange() or SetVelocityRange(), then Enable()
 * before use.
 */
class ClosedLoopMotor : public SpeedController, public SendableBase {
 public:
  ClosedLoopMotor(double Kp, double Ki, double Kd, INode& input,
                  SpeedController& motor, double period = 0.05);
  ClosedLoopMotor(double Kp, double Ki, double Kd, double Kff, INode& input,
                  SpeedController& motor, double period = 0.05);

  void SetDisplacementRange(double minimumDisplacement,
                            double maximumDisplacement,
                            bool continuous = false);
  void SetVelocityRange(double minimumVelocity, double maximumVelocity);
  void Enable();
  PIDNode& GetController();

  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

  void PIDWrite(double output) override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  RefInput m_refInput{0.0};
  GainNode m_feedforward{0.0, m_refInput};
  SumNode m_sum;
  PIDNode m_pid;
  Output m_output;

  double m_minimumInput = -1.0;
  double m_maximumInput = 1.0;
  bool m_isInverted = false;
};

}  // namespace frc
