/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/Sendable.h"

void Sendable::InitTable(std::shared_ptr<ITable> subtable) {
  GetTable() = subtable;
  UpdateTable();
}
