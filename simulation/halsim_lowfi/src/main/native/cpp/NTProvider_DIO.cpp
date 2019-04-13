/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_DIO.h"

#include <hal/Ports.h>
#include <mockdata/DIOData.h>

void HALSimNTProviderDIO::Initialize() {
  InitializeDefault(HAL_GetNumDigitalChannels(),
                    HALSIM_RegisterDIOAllCallbacks);
}

void HALSimNTProviderDIO::OnCallback(uint32_t chan,
                                     std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetDIOInitialized(chan));
  table->GetEntry("value").SetBoolean(HALSIM_GetDIOValue(chan));
  table->GetEntry("pulse_length").SetDouble(HALSIM_GetDIOPulseLength(chan));
  table->GetEntry("input?").SetBoolean(HALSIM_GetDIOIsInput(chan));
}

void HALSimNTProviderDIO::OnInitializedChannel(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("value").AddListener(
      [=](const nt::EntryNotification& ev) -> void {
        if (HALSIM_GetDIOIsInput(chan)) {
          HALSIM_SetDIOValue(chan, ev.value->GetBoolean());
        }
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);
}
