/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_RoboRIO.h"

#include <hal/Ports.h>
#include <mockdata/RoboRioData.h>

void HALSimNTProviderRoboRIO::Initialize() {
  InitializeDefault(1, HALSIM_RegisterRoboRioAllCallbacks);
}

void HALSimNTProviderRoboRIO::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("fpga_button?").SetBoolean(HALSIM_GetRoboRioFPGAButton(chan));

  table->GetEntry("vin_voltage").SetDouble(HALSIM_GetRoboRioVInVoltage(chan));
  table->GetEntry("vin_current").SetDouble(HALSIM_GetRoboRioVInCurrent(chan));

  auto t6v = table->GetSubTable("6V");
  t6v->GetEntry("voltage").SetDouble(HALSIM_GetRoboRioUserVoltage6V(chan));
  t6v->GetEntry("current").SetDouble(HALSIM_GetRoboRioUserCurrent6V(chan));
  t6v->GetEntry("active?").SetBoolean(HALSIM_GetRoboRioUserActive6V(chan));
  t6v->GetEntry("faults").SetDouble(HALSIM_GetRoboRioUserFaults6V(chan));

  auto t5v = table->GetSubTable("5V");
  t5v->GetEntry("voltage").SetDouble(HALSIM_GetRoboRioUserVoltage5V(chan));
  t5v->GetEntry("current").SetDouble(HALSIM_GetRoboRioUserCurrent5V(chan));
  t5v->GetEntry("active?").SetBoolean(HALSIM_GetRoboRioUserActive5V(chan));
  t5v->GetEntry("faults").SetDouble(HALSIM_GetRoboRioUserFaults5V(chan));

  auto t3v3 = table->GetSubTable("3V3");
  t3v3->GetEntry("voltage").SetDouble(HALSIM_GetRoboRioUserVoltage3V3(chan));
  t3v3->GetEntry("current").SetDouble(HALSIM_GetRoboRioUserCurrent3V3(chan));
  t3v3->GetEntry("active?").SetBoolean(HALSIM_GetRoboRioUserActive3V3(chan));
  t3v3->GetEntry("faults").SetDouble(HALSIM_GetRoboRioUserFaults3V3(chan));
}

void HALSimNTProviderRoboRIO::OnInitializedChannel(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("fpga_button?")
      .AddListener(
          [=](const nt::EntryNotification& ev) -> void {
            HALSIM_SetRoboRioFPGAButton(chan, ev.value->GetBoolean());
          },
          NT_NotifyKind::NT_NOTIFY_UPDATE);
}
