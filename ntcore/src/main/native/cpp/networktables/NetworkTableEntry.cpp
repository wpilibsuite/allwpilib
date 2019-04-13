/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTableEntry.h"

#include "networktables/NetworkTableInstance.h"

using namespace nt;

NetworkTableInstance NetworkTableEntry::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}
