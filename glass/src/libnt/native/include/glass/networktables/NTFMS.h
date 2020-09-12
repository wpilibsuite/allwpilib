/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/FMS.h"

namespace glass {

class NTFMSModel : public FMSModel {
 public:
  static constexpr const char* kType = "FMSInfo";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTFMSModel(wpi::StringRef path);
  NTFMSModel(NT_Inst inst, wpi::StringRef path);

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
  wpi::StringRef GetGameSpecificMessage(
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
  NetworkTablesHelper m_nt;
  NT_Entry m_gameSpecificMessage;
  NT_Entry m_alliance;
  NT_Entry m_station;
  NT_Entry m_controlWord;

  DataSource m_fmsAttached;
  DataSource m_dsAttached;
  DataSource m_allianceStationId;
  DataSource m_estop;
  DataSource m_enabled;
  DataSource m_test;
  DataSource m_autonomous;
};

}  // namespace glass
