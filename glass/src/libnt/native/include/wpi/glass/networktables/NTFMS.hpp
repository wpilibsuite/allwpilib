// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/FMS.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace glass {

class NTFMSModel : public FMSModel {
 public:
  static constexpr const char* kType = "FMSInfo";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTFMSModel(std::string_view path);
  NTFMSModel(nt::NetworkTableInstance inst, std::string_view path);

  BooleanSource* GetFmsAttachedData() override { return &m_fmsAttached; }
  BooleanSource* GetDsAttachedData() override { return &m_dsAttached; }
  IntegerSource* GetAllianceStationIdData() override {
    return &m_allianceStationId;
  }
  // NT does not provide match time
  DoubleSource* GetMatchTimeData() override { return nullptr; }
  BooleanSource* GetEStopData() override { return &m_estop; }
  BooleanSource* GetEnabledData() override { return &m_enabled; }
  BooleanSource* GetTestData() override { return &m_test; }
  BooleanSource* GetAutonomousData() override { return &m_autonomous; }
  StringSource* GetGameSpecificMessageData() override {
    return &m_gameSpecificMessageData;
  }

  // NT is read-only (it's continually set by robot code)
  void SetFmsAttached(bool val) override {}
  void SetDsAttached(bool val) override {}
  void SetAllianceStationId(int val) override {}
  void SetMatchTime(double val) override {}
  void SetEStop(bool val) override {}
  void SetEnabled(bool val) override {}
  void SetTest(bool val) override {}
  void SetAutonomous(bool val) override {}
  void SetGameSpecificMessage(std::string_view val) override {}

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::StringSubscriber m_gameSpecificMessage;
  nt::BooleanSubscriber m_alliance;
  nt::IntegerSubscriber m_station;
  nt::IntegerSubscriber m_controlWord;

  BooleanSource m_fmsAttached;
  BooleanSource m_dsAttached;
  IntegerSource m_allianceStationId;
  BooleanSource m_estop;
  BooleanSource m_enabled;
  BooleanSource m_test;
  BooleanSource m_autonomous;
  StringSource m_gameSpecificMessageData;
};

}  // namespace glass
