// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "glass/Model.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace glass {

class DataSource;

class FMSModel : public Model {
 public:
  virtual DataSource* GetFmsAttachedData() = 0;
  virtual DataSource* GetDsAttachedData() = 0;
  virtual DataSource* GetAllianceStationIdData() = 0;
  virtual DataSource* GetMatchTimeData() = 0;
  virtual DataSource* GetEStopData() = 0;
  virtual DataSource* GetEnabledData() = 0;
  virtual DataSource* GetTestData() = 0;
  virtual DataSource* GetAutonomousData() = 0;
  virtual std::string_view GetGameSpecificMessage(
      wpi::SmallVectorImpl<char>& buf) = 0;

  virtual void SetFmsAttached(bool val) = 0;
  virtual void SetDsAttached(bool val) = 0;
  virtual void SetAllianceStationId(int val) = 0;
  virtual void SetMatchTime(double val) = 0;
  virtual void SetEStop(bool val) = 0;
  virtual void SetEnabled(bool val) = 0;
  virtual void SetTest(bool val) = 0;
  virtual void SetAutonomous(bool val) = 0;
  virtual void SetGameSpecificMessage(std::string_view val) = 0;
};

/**
 * Displays FMS view.
 *
 * @param matchTimeEnabled If not null, a checkbox is displayed for
 *                         "enable match time" linked to this value
 * @param editableDsAttached If true, DS attached should be editable
 */
void DisplayFMS(FMSModel* model, bool editableDsAttached);
void DisplayFMSReadOnly(FMSModel* model);

}  // namespace glass
