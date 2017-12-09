/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <HAL/Ports.h>
#include <MockData/EncoderData.h>

#include <NTProvider_Encoder.h>

void HALSimNTProviderEncoder::Initialize() {
    InitializeDefault(HAL_GetNumEncoders(), HALSIM_RegisterEncoderAllCallbacks);
}

void HALSimNTProviderEncoder::OnCallback(uint32_t chan, std::shared_ptr<nt::NetworkTable> table) {
    table->GetEntry("init?").SetBoolean(HALSIM_GetEncoderInitialized(chan));
    table->GetEntry("count").SetDouble(HALSIM_GetEncoderCount(chan));
    table->GetEntry("period").SetDouble(HALSIM_GetEncoderPeriod(chan));
    table->GetEntry("reset?").SetBoolean(HALSIM_GetEncoderReset(chan));
    table->GetEntry("max_period").SetDouble(HALSIM_GetEncoderMaxPeriod(chan));
    table->GetEntry("direction").SetString(HALSIM_GetEncoderDirection(chan) ? "FWD" : "RVS");
    table->GetEntry("reverse_direction?").SetBoolean(HALSIM_GetEncoderReverseDirection(chan));
    table->GetEntry("samples_to_avg").SetDouble(HALSIM_GetEncoderSamplesToAverage(chan));
}