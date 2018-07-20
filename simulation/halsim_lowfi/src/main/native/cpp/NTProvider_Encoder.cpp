/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_Encoder.h"

#include <hal/Ports.h>
#include <mockdata/EncoderData.h>

void HALSimNTProviderEncoder::Initialize() {
  InitializeDefault(HAL_GetNumEncoders(), HALSIM_RegisterEncoderAllCallbacks);
}

void HALSimNTProviderEncoder::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetEncoderInitialized(chan));
  table->GetEntry("count").SetDouble(HALSIM_GetEncoderCount(chan));
  table->GetEntry("period").SetDouble(HALSIM_GetEncoderPeriod(chan));
  table->GetEntry("reset?").SetBoolean(HALSIM_GetEncoderReset(chan));
  table->GetEntry("max_period").SetDouble(HALSIM_GetEncoderMaxPeriod(chan));
  table->GetEntry("direction").SetBoolean(HALSIM_GetEncoderDirection(chan));
  table->GetEntry("reverse_direction?")
      .SetBoolean(HALSIM_GetEncoderReverseDirection(chan));
  table->GetEntry("samples_to_avg")
      .SetDouble(HALSIM_GetEncoderSamplesToAverage(chan));
}

void HALSimNTProviderEncoder::OnInitializedChannel(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("count").AddListener(
      [=](const nt::EntryNotification& ev) -> void {
        HALSIM_SetEncoderCount(chan,
                               static_cast<int32_t>(ev.value->GetDouble()));
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);

  table->GetEntry("direction")
      .AddListener(
          [=](const nt::EntryNotification& ev) -> void {
            HALSIM_SetEncoderDirection(chan, ev.value->GetBoolean());
          },
          NT_NotifyKind::NT_NOTIFY_UPDATE);
}
