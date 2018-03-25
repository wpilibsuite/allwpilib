/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/NodeBase.h"

using namespace frc;

NodeBase::NodeBase(INode& input) : m_input(input) {}

INode* NodeBase::GetInputNode() { return &m_input; }

double NodeBase::GetOutput() { return m_input.GetOutput(); }
