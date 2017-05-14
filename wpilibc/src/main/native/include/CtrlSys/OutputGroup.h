/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <vector>

#include "CtrlSys/Output.h"
#include "Notifier.h"

namespace frc {

/**
 * Allows grouping Output instances together to run in one thread.
 *
 * Each output's OutputFunc() is called at a regular interval. This can be used
 * to avoid unnecessary context switches for Output instances that are running
 * at the same sample rate and priority.
 */
class OutputGroup {
 public:
  template <class... Outputs>
  explicit OutputGroup(Output& output, Outputs&&... outputs);

  explicit OutputGroup(Output& output);

  virtual ~OutputGroup() = default;

  void Enable(double period = INode::kDefaultPeriod);
  void Disable();

 protected:
  virtual void OutputFunc();

 private:
  std::vector<std::reference_wrapper<Output>> m_outputs;
  Notifier m_thread;
};

}  // namespace frc

#include "CtrlSys/OutputGroup.inc"
