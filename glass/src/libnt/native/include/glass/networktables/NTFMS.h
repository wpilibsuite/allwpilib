// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <networktables/BooleanTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "glass/DataSource.h"
#include "glass/other/FMS.h"

namespace glass {

class NTFMSModel : public FMSModel {
 public:
  static constexpr const char* kType = "FMSInfo";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTFMSModel(std::string_view path);
  NTFMSModel(nt::NetworkTableInstance inst, std::string_view path);

  DataSource* GetFmsAttachedData() override { return &m_fmsAttached; }
  DataSource* GetDsAttachedData() override { return &m_dsAttached; }
  DataSource* GetAllianceStationIdData() override {
    return &m_allianceStationId;
  }
  // NT does not provide match time
  DataSource* GetMatchTimeData() override { return nullptr; }
  DataSource* GetEStopData() override { return &m_estop; }
  DataSource* GetEnabledData() override { return &m_enabled; }
  DataSource* GetTestData() override { return &m_test; }
  DataSource* GetAutonomousData() override { return &m_autonomous; }
  std::string_view GetGameSpecificMessage(
      wpi::SmallVectorImpl<char>& buf) override;

  // NT is read-only (it's continually set by robot code)
  void SetFmsAttached(bool val) override {}
  void SetDsAttached(bool val) override {}
  void SetAllianceStationId(int val) override {}
  void SetMatchTime(double val) override {}
  void SetEStop(bool val) override {}
  void SetEnabled(bool val) override {}
  void SetTest(bool val) override {}
  void SetAutonomous(bool val) override {}
  void SetGameSpecificMessage(const char* val) override {}

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::StringSubscriber m_gameSpecificMessage;
  nt::BooleanSubscriber m_alliance;
  nt::IntegerSubscriber m_station;
  nt::IntegerSubscriber m_controlWord;

  DataSource m_fmsAttached;
  DataSource m_dsAttached;
  DataSource m_allianceStationId;
  DataSource m_estop;
  DataSource m_enabled;
  DataSource m_test;
  DataSource m_autonomous;
};

}  // namespace glass
