// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/Model.hpp"

namespace wpi::util {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace wpi::glass {

class BooleanSource;
class DoubleSource;
class IntegerSource;
class StringSource;

class FMSModel : public Model {
 public:
  virtual BooleanSource* GetFmsAttachedData() = 0;
  virtual BooleanSource* GetDsAttachedData() = 0;
  virtual IntegerSource* GetAllianceStationIdData() = 0;
  virtual DoubleSource* GetMatchTimeData() = 0;
  virtual BooleanSource* GetEStopData() = 0;
  virtual BooleanSource* GetEnabledData() = 0;
  virtual BooleanSource* GetTestData() = 0;
  virtual BooleanSource* GetAutonomousData() = 0;
  virtual StringSource* GetGameSpecificMessageData() = 0;

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

}  // namespace wpi::glass
