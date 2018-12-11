/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/LayoutType.h"

using namespace frc;

LayoutType::LayoutType(wpi::Twine& layoutName) : m_layoutName(layoutName) {}

wpi::Twine& LayoutType::GetLayoutName() const { return m_layoutName; }
