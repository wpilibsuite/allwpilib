/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/LayoutType.h"
#pragma warning(disable: 4244 4267 4146)

using namespace frc;

wpi::StringRef LayoutType::GetLayoutName() const { return m_layoutName; }
