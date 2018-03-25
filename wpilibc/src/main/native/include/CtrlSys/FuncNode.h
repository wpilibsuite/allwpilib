/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include "CtrlSys/INode.h"

namespace frc {

/**
 * Converts a callable into a control system node whose output is the callable's
 * return value.
 */
class FuncNode : public INode {
 public:
  FuncNode(std::function<double()> func);  // NOLINT

  double GetOutput() override;

 private:
  std::function<double()> m_func;
};

}  // namespace frc
