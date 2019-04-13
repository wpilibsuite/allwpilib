/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/GamepadBase.h"
#pragma warning(disable: 4244 4267 4146)

using namespace frc;

GamepadBase::GamepadBase(int port) : GenericHID(port) {}
