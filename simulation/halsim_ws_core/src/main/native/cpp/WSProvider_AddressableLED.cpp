/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_AddressableLED.h"

#include <hal/Ports.h>
#include <hal/simulation/AddressableLEDData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterAddressableLED##halsim##Callback(                        \
      m_channel,                                                          \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderAddressableLED*>(param)               \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)
namespace wpilibws {
void HALSimWSProviderAddressableLED::Initialize(
    WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAddressableLED>(
      "AddressableLED", HAL_GetNumAddressableLEDs(), webRegisterFunc);
}

HALSimWSProviderAddressableLED::~HALSimWSProviderAddressableLED() {
  DoCancelCallbacks();
}

void HALSimWSProviderAddressableLED::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_outputPortCbKey = REGISTER(OutputPort, "<>output_port", int32_t, int);
  m_lengthCbKey = REGISTER(Length, "<>length", int32_t, int);
  m_runningCbKey = REGISTER(Running, "<>running", bool, boolean);
  // TODO: why does this use a ConstBufferCallback, how do i integrate it
  // m_dataCbKey = REGISTER(Data, "<>data", int32_t, int);
}

void HALSimWSProviderAddressableLED::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderAddressableLED::DoCancelCallbacks() {
  HALSIM_CancelAddressableLEDInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAddressableLEDOutputPortCallback(m_channel, m_outputPortCbKey);
  HALSIM_CancelAddressableLEDLengthCallback(m_channel, m_lengthCbKey);
  HALSIM_CancelAddressableLEDRunningCallback(m_channel, m_runningCbKey);
  HALSIM_CancelAddressableLEDDataCallback(m_channel, m_dataCbKey);

  m_initCbKey = 0;
  m_outputPortCbKey = 0;
  m_lengthCbKey = 0;
  m_runningCbKey = 0;
  m_dataCbKey = 0;
}
}  // namespace wpilibws
