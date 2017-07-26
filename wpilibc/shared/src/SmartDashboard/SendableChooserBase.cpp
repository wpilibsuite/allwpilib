/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableChooserBase.h"

const char* frc::SendableChooserBase::kDefault = "default";
const char* frc::SendableChooserBase::kOptions = "options";
const char* frc::SendableChooserBase::kSelected = "selected";

std::shared_ptr<ITable> frc::SendableChooserBase::GetTable() const {
  return m_table;
}

std::string frc::SendableChooserBase::GetSmartDashboardType() const {
  return "String Chooser";
}
