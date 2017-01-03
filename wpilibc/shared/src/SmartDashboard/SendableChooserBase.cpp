/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableChooserBase.h"

using namespace frc;

const char* SendableChooserBase::kDefault = "default";
const char* SendableChooserBase::kOptions = "options";
const char* SendableChooserBase::kSelected = "selected";

std::shared_ptr<ITable> SendableChooserBase::GetTable() const {
  return m_table;
}

std::string SendableChooserBase::GetSmartDashboardType() const {
  return "String Chooser";
}
