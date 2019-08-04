/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimDsNt.h"

void HALSimDSNT::Initialize() {
  rootTable =
      nt::NetworkTableInstance::GetDefault().GetTable("sim")->GetSubTable(
          "DS_CONTROL");  // Not to be confused with sim::DriverStation from
                          // HALSim LowFi

  // LOOP TIMING //

  auto timinghz = rootTable->GetEntry("timing_hz");
  timinghz.ForceSetDouble(50);
  timinghz.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        double valIn = ev.value->GetDouble();
        double val = 0;
        val = (valIn < 1 ? 1 : valIn > 100 ? 100 : valIn);

        if (val != valIn) {
          this->rootTable->GetEntry("timing_hz").ForceSetDouble(val);
          Flush();
        }

        this->timingHz = val;
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  // MODES //

  modeTable = rootTable->GetSubTable("mode");
  auto mtele = modeTable->GetEntry("teleop?");
  auto mauto = modeTable->GetEntry("auto?");
  auto mtest = modeTable->GetEntry("test?");
  auto enabled = modeTable->GetEntry("enabled?");
  auto estop = modeTable->GetEntry("estop?");

  mtele.ForceSetBoolean(true);
  mauto.ForceSetBoolean(false);
  mtest.ForceSetBoolean(false);
  enabled.ForceSetBoolean(false);
  estop.ForceSetBoolean(false);

  mtele.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        this->HandleModePress(HALSimDSNT_Mode::teleop, ev.value->GetBoolean());
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  mauto.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        this->HandleModePress(HALSimDSNT_Mode::auton, ev.value->GetBoolean());
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  mtest.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        this->HandleModePress(HALSimDSNT_Mode::test, ev.value->GetBoolean());
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  enabled.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        std::scoped_lock lock(modeMutex);
        if (!this->isEstop) {
          this->isEnabled = ev.value->GetBoolean();
        } else {
          this->isEnabled = false;
        }
        this->DoModeUpdate();
        this->UpdateModeButtons();
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  estop.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        std::scoped_lock lock(modeMutex);
        this->isEstop = ev.value->GetBoolean();
        if (this->isEstop) {
          this->isEnabled = false;
        }
        this->DoModeUpdate();
        this->UpdateModeButtons();
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  // ALLIANCES //

  allianceTable = rootTable->GetSubTable("alliance");
  auto allianceStation = allianceTable->GetEntry("station");
  auto allianceColorRed = allianceTable->GetEntry("red?");

  allianceStation.ForceSetDouble(1);
  allianceColorRed.ForceSetBoolean(true);

  allianceStation.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        double stnIn = ev.value->GetDouble();
        double stn = 0;
        stn = (stnIn > 3 ? 3 : stnIn < 1 ? 1 : stnIn);

        if (stn != stnIn) {
          this->allianceTable->GetEntry("station").ForceSetDouble(stn);
          Flush();
        }

        this->allianceStation = stn;
        this->DoAllianceUpdate();
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  allianceColorRed.AddListener(
      [this](const nt::EntryNotification& ev) -> void {
        this->isAllianceRed = ev.value->GetBoolean();
        this->DoAllianceUpdate();
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  // FINAL LOGIC //

  Flush();

  loopThread = std::thread([this]() -> void {
    this->running = true;
    this->LoopFunc();
  });
  loopThread.detach();
}

void HALSimDSNT::HandleModePress(enum HALSimDSNT_Mode mode, bool isPressed) {
  if (isPressed) {
    if (mode != currentMode) {
      std::scoped_lock lock(modeMutex);
      currentMode = mode;
      isEnabled = false;
      this->DoModeUpdate();
    }
  }

  this->UpdateModeButtons();
}

void HALSimDSNT::UpdateModeButtons() {
  modeTable->GetEntry("teleop?").ForceSetBoolean(currentMode ==
                                                 HALSimDSNT_Mode::teleop);
  modeTable->GetEntry("auto?").ForceSetBoolean(currentMode ==
                                               HALSimDSNT_Mode::auton);
  modeTable->GetEntry("test?").ForceSetBoolean(currentMode ==
                                               HALSimDSNT_Mode::test);
  modeTable->GetEntry("enabled?").ForceSetBoolean(isEnabled);
  Flush();
}

void HALSimDSNT::DoModeUpdate() {
  HALSIM_SetDriverStationAutonomous(currentMode == HALSimDSNT_Mode::auton);
  HALSIM_SetDriverStationTest(currentMode == HALSimDSNT_Mode::test);
  HALSIM_SetDriverStationEnabled(isEnabled);
  if (isEnabled && !lastIsEnabled) {
    currentMatchTime = 0;
  }
  lastIsEnabled = isEnabled;
  HALSIM_SetDriverStationEStop(isEstop);
  HALSIM_SetDriverStationFmsAttached(false);
  HALSIM_SetDriverStationDsAttached(true);
  HALSIM_NotifyDriverStationNewData();
}

void HALSimDSNT::DoAllianceUpdate() {
  HALSIM_SetDriverStationAllianceStationId(static_cast<HAL_AllianceStationID>(
      (isAllianceRed ? HAL_AllianceStationID_kRed1
                     : HAL_AllianceStationID_kBlue1) +
      (static_cast<int32_t>(allianceStation) - 1)));
}

void HALSimDSNT::LoopFunc() {
  while (running) {
    double dt = 1000 / timingHz;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int64_t>(dt)));
    if (isEnabled) {
      currentMatchTime = currentMatchTime + dt;
      HALSIM_SetDriverStationMatchTime(currentMatchTime);
    }
    HALSIM_NotifyDriverStationNewData();
  }
}

void HALSimDSNT::Flush() { rootTable->GetInstance().Flush(); }
