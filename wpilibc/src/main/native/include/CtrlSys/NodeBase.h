/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "CtrlSys/INode.h"

namespace frc {

/**
 * Provides a common base class for nodes with one input.
 */
class NodeBase : public INode {
 public:
  explicit NodeBase(INode& input);
  virtual ~NodeBase() = default;

  INode* GetInputNode() override;
  double GetOutput() override;

 private:
  INode& m_input;
};

}  // namespace frc
