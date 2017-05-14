/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <limits>
#include <mutex>

#include "CtrlSys/INode.h"
#include "CtrlSys/NodeBase.h"

namespace frc {

/**
 * Represents an integrator in a control system diagram.
 */
class IntegralNode : public NodeBase {
 public:
  IntegralNode(double K, INode& input, double period = kDefaultPeriod);
  virtual ~IntegralNode() = default;

  double GetOutput() override;

  void SetGain(double K);
  double GetGain() const;

  void SetIZone(double maxInputMagnitude);

  void Reset();

 private:
  double m_gain;
  double m_period;

  double m_total = 0.0;
  double m_maxInputMagnitude = std::numeric_limits<double>::infinity();

  mutable std::mutex m_mutex;
};

}  // namespace frc
