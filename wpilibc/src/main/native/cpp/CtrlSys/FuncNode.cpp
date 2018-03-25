/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/FuncNode.h"

using namespace frc;

FuncNode::FuncNode(std::function<double()> func) { m_func = func; }

double FuncNode::GetOutput() { return m_func(); }
