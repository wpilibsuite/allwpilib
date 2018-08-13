/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableChooserBase.h"

using namespace frc;

std::atomic_int SendableChooserBase::s_instances{0};

SendableChooserBase::SendableChooserBase()
    : SendableBase(false), m_instance{s_instances++} {}
