/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NTProvider_Analog.h"

#include <hal/Ports.h>
#include <mockdata/AnalogInData.h>
#include <mockdata/AnalogOutData.h>

void HALSimNTProviderAnalogIn::Initialize() {
  InitializeDefault(HAL_GetNumAnalogInputs(),
                    HALSIM_RegisterAnalogInAllCallbacks);
}

void HALSimNTProviderAnalogIn::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetAnalogInInitialized(chan));
  table->GetEntry("avg_bits").SetDouble(HALSIM_GetAnalogInAverageBits(chan));
  table->GetEntry("oversample_bits")
      .SetDouble(HALSIM_GetAnalogInOversampleBits(chan));
  table->GetEntry("voltage").SetDouble(HALSIM_GetAnalogInVoltage(chan));

  auto accum = table->GetSubTable("accum");
  accum->GetEntry("init?").SetBoolean(
      HALSIM_GetAnalogInAccumulatorInitialized(chan));
  accum->GetEntry("value").SetDouble(HALSIM_GetAnalogInAccumulatorValue(chan));
  accum->GetEntry("count").SetDouble(HALSIM_GetAnalogInAccumulatorCount(chan));
  accum->GetEntry("center").SetDouble(
      HALSIM_GetAnalogInAccumulatorCenter(chan));
  accum->GetEntry("deadband")
      .SetDouble(HALSIM_GetAnalogInAccumulatorDeadband(chan));
}

void HALSimNTProviderAnalogIn::OnInitializedChannel(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("voltage").AddListener(
      [=](const nt::EntryNotification& ev) -> void {
        HALSIM_SetAnalogInVoltage(chan, ev.value->GetDouble());
      },
      NT_NotifyKind::NT_NOTIFY_UPDATE);
}

void HALSimNTProviderAnalogOut::Initialize() {
  InitializeDefault(HAL_GetNumAnalogOutputs(),
                    HALSIM_RegisterAnalogOutAllCallbacks);
}

void HALSimNTProviderAnalogOut::OnCallback(
    uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
  table->GetEntry("init?").SetBoolean(HALSIM_GetAnalogOutInitialized(chan));
  table->GetEntry("voltage").SetDouble(HALSIM_GetAnalogOutVoltage(chan));
}
