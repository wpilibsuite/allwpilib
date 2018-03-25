/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mutex>

#include "Controller.h"
#include "CtrlSys/INode.h"
#include "Notifier.h"
#include "PIDOutput.h"

namespace frc {

/**
 * INode adapter for PIDOutput subclasses.
 *
 * Wraps a PIDOutput object in the INode interface by calling PIDWrite() on it
 * at a regular interval specified in the constructor. This is called in a
 * separate thread.
 */
class Output : public Controller {
 public:
  Output(INode& input, PIDOutput& output,
         double period = INode::kDefaultPeriod);
  virtual ~Output() = default;

  void Enable() override;
  void Disable() override;

  void SetRange(double minU, double maxU);

 protected:
  virtual void OutputFunc();

  friend class OutputGroup;
  friend class RefInput;

 private:
  INode& m_input;
  PIDOutput& m_output;
  double m_period;

  Notifier m_thread;
  std::mutex m_mutex;

  double m_minU = -1.0;
  double m_maxU = 1.0;
};

}  // namespace frc
