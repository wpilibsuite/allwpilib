/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "DerivativeNode.h"
#include "GainNode.h"
#include "INode.h"
#include "IntegralNode.h"
#include "NodeBase.h"
#include "SumNode.h"

namespace frc {

/**
 * A PID controller implementation based on the control system diagram
 * framework. This is more general than the PIDController as its output can be
 * chained with other nodes that are not actuators.
 *
 * For a simple closed-loop PID controller, see PIDController.
 */
class PIDNode : public NodeBase {
 public:
  PIDNode(double Kp, double Ki, double Kd, INode& input,
          double period = kDefaultPeriod);
  PIDNode(double Kp, double Ki, double Kd, INode& feedforward, INode& input,
          double period = kDefaultPeriod);
  virtual ~PIDNode() = default;

  double GetOutput() override;

  void SetPID(double p, double i, double d);

  void SetP(double p);
  double GetP() const;

  void SetI(double i);
  double GetI() const;

  void SetD(double d);
  double GetD() const;

  void SetOutputRange(double minU, double maxU);
  void SetIZone(double maxInputMagnitude);

  void Reset();

 private:
  GainNode m_P;
  IntegralNode m_I;
  DerivativeNode m_D;
  SumNode m_sum;

  double m_minU = -1.0;
  double m_maxU = 1.0;
};

}  // namespace frc
