/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "GyroBase.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Get the PIDOutput for the PIDSource base object. Can be set to return
 * angle or rate using SetPIDSourceType(). Defaults to angle.
 *
 * @return The PIDOutput (angle or rate, defaults to angle)
 */
double GyroBase::PIDGet() {
  switch (GetPIDSourceType()) {
    case PIDSourceType::kRate:
      return GetRate();
    case PIDSourceType::kDisplacement:
      return GetAngle();
    default:
      return 0;
  }
}

void GyroBase::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetAngle());
  }
}

void GyroBase::StartLiveWindowMode() {}

void GyroBase::StopLiveWindowMode() {}

std::string GyroBase::GetSmartDashboardType() const { return "Gyro"; }

void GyroBase::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> GyroBase::GetTable() const { return m_table; }
