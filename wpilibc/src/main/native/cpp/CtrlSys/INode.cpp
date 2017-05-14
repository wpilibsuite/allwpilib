/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/INode.h"

#include "CtrlSys/Output.h"

using namespace frc;

/**
 * Get input node.
 *
 * Returns nullptr if there is no input node.
 */
INode* INode::GetInputNode() { return nullptr; }

/**
 * Set callback function.
 *
 * Nodes with no input should override this to store the function
 * Output::OutputFunc(), then call it after the node's value changes.
 */
void INode::SetCallback(Output& output) {
  if (GetInputNode() != nullptr) {
    GetInputNode()->SetCallback(output);
  }
}
