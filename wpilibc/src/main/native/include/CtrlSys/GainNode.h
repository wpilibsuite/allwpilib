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

class GainNode : public NodeBase {
 public:
  GainNode(double K, INode& input);
  virtual ~GainNode() = default;

  double GetOutput() override;

  void SetGain(double K);
  double GetGain() const;

 private:
  double m_gain;

  mutable std::mutex m_mutex;
};

}  // namespace frc
