/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mutex>

#include "CtrlSys/INode.h"
#include "CtrlSys/NodeBase.h"

namespace frc {

/**
 * Returns the integral of the input node's output.
 */
class DerivativeNode : public NodeBase {
 public:
  DerivativeNode(double K, INode& input, double period = kDefaultPeriod);
  virtual ~DerivativeNode() = default;

  double GetOutput() override;

  void SetGain(double K);
  double GetGain() const;

  void Reset();

 private:
  double m_gain;
  double m_period;

  double m_prevInput = 0.0;

  mutable std::mutex m_mutex;
};

}  // namespace frc
