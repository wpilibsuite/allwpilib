/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_DriverStation.h"

#include <hal/Ports.h>
#include <mockdata/DriverStationData.h>

void HALSimNTProviderDriverStation::Initialize() {
  InitializeDefaultSingle(HALSIM_RegisterDriverStationAllCallbacks);
}

void HALSimNTProviderDriverStation::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  bool auton = HALSIM_GetDriverStationAutonomous(),
       test = HALSIM_GetDriverStationTest(),
       enabled = HALSIM_GetDriverStationEnabled();

  bool teleop = (!auton && !test && enabled);

  table->GetEntry("enabled?").SetBoolean(enabled);
  table->GetEntry("autonomous?").SetBoolean(auton);
  table->GetEntry("test?").SetBoolean(test);
  table->GetEntry("teleop?").SetBoolean(teleop);
  table->GetEntry("estop?").SetBoolean(HALSIM_GetDriverStationEStop());
  table->GetEntry("fms?").SetBoolean(HALSIM_GetDriverStationFmsAttached());
  table->GetEntry("ds?").SetBoolean(HALSIM_GetDriverStationDsAttached());
  table->GetEntry("match_time").SetDouble(HALSIM_GetDriverStationMatchTime());

  // TODO: Joysticks

  auto alliance = table->GetSubTable("alliance");
  auto allianceValue = HALSIM_GetDriverStationAllianceStationId();
  alliance->GetEntry("color").SetString(
      (allianceValue == HAL_AllianceStationID_kRed1 ||
       allianceValue == HAL_AllianceStationID_kRed2 ||
       allianceValue == HAL_AllianceStationID_kRed3)
          ? "red"
          : "blue");
  int station = 0;

  switch (allianceValue) {
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kBlue1:
      station = 1;
      break;
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kBlue2:
      station = 2;
      break;
    case HAL_AllianceStationID_kRed3:
    case HAL_AllianceStationID_kBlue3:
      station = 3;
      break;
  }
  alliance->GetEntry("station").SetDouble(station);
}
