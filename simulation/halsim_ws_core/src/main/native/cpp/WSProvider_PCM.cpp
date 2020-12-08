/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PCM.h"

#include <hal/Ports.h>
#include <hal/simulation/PCMData.h>

#define SOLENOID_INITIALIZED_JSONID(channel) "<solenoid_init_" #channel
#define SOLENOID_OUTPUT_JSONID(channel) "<solenoid_output_" #channel

#define REGISTER_SOLENOID(solenoidChannel, halsim, jsonid, ctype, haltype) \
  HALSIM_RegisterPCM##halsim##Callback(                                    \
      m_channel,                                                           \
      solenoidChannel,                                                     \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderPCM*>(param)->ProcessHalCallback(      \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterPCM##halsim##Callback(                                  \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPCM*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)

#define REGISTER_SOLENOID_CALLBACKS(channel) \
  m_solenoidInitCbKeys[channel] = REGISTER_SOLENOID(channel, SolenoidInitialized, SOLENOID_INITIALIZED_JSONID(channel), bool, boolean); \
  m_solenoidOutputCbKeys[channel] = REGISTER_SOLENOID(channel, SolenoidOutput, SOLENOID_OUTPUT_JSONID(channel), bool, boolean);

#define CANCEL_SOLENOID_CALLBACKS(channel) \
  HALSIM_CancelPCMSolenoidInitializedCallback(m_channel, channel, m_solenoidInitCbKeys[channel]); \
  HALSIM_CancelPCMSolenoidOutputCallback(m_channel, channel, m_solenoidOutputCbKeys[channel]);

namespace wpilibws {
void HALSimWSProviderPCM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPCM>("PCM", HAL_GetNumPCMModules(), webRegisterFunc);
}

HALSimWSProviderPCM::~HALSimWSProviderPCM() { DoCancelCallbacks(); }

void HALSimWSProviderPCM::RegisterCallbacks() {
  REGISTER_SOLENOID_CALLBACKS(0);
  REGISTER_SOLENOID_CALLBACKS(1);
  REGISTER_SOLENOID_CALLBACKS(2);
  REGISTER_SOLENOID_CALLBACKS(3);
  REGISTER_SOLENOID_CALLBACKS(4);
  REGISTER_SOLENOID_CALLBACKS(5);
  REGISTER_SOLENOID_CALLBACKS(6);
  REGISTER_SOLENOID_CALLBACKS(7);

  m_anySolenoidInitCbKey = REGISTER(AnySolenoidInitialized, "<any_solenoid_init", bool, boolean);
  m_compressorInitCbKey = REGISTER(CompressorInitialized, "<compressor_init", bool, boolean);
  m_compressorOnCbKey = REGISTER(CompressorOn, "<compressor_on", bool, boolean);
  m_closedLoopEnabledCbKey = REGISTER(ClosedLoopEnabled, "<closed_loop_enabled", bool, boolean);
  m_pressureSwitchCbKey = REGISTER(PressureSwitch, "<pressure_switch", bool, boolean);
  m_compressorCurrentCbKey = REGISTER(CompressorCurrent, "<compressor_current", double, double);
}

void HALSimWSProviderPCM::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderPCM::DoCancelCallbacks() {
  CANCEL_SOLENOID_CALLBACKS(0);
  CANCEL_SOLENOID_CALLBACKS(1);
  CANCEL_SOLENOID_CALLBACKS(2);
  CANCEL_SOLENOID_CALLBACKS(3);
  CANCEL_SOLENOID_CALLBACKS(4);
  CANCEL_SOLENOID_CALLBACKS(5);
  CANCEL_SOLENOID_CALLBACKS(6);
  CANCEL_SOLENOID_CALLBACKS(7);
  for (int i = 0; i < NUM_SOLENOIDS_PER_PCM; i++) {
    m_solenoidInitCbKeys[0] = 0;
    m_solenoidOutputCbKeys[0] = 0;
  }

  HALSIM_CancelPCMAnySolenoidInitializedCallback(m_channel, m_anySolenoidInitCbKey);
  HALSIM_CancelPCMCompressorInitializedCallback(m_channel, m_compressorInitCbKey);
  HALSIM_CancelPCMCompressorOnCallback(m_channel, m_compressorOnCbKey);
  HALSIM_CancelPCMClosedLoopEnabledCallback(m_channel, m_closedLoopEnabledCbKey);
  HALSIM_CancelPCMPressureSwitchCallback(m_channel, m_pressureSwitchCbKey);
  HALSIM_CancelPCMCompressorCurrentCallback(m_channel, m_compressorCurrentCbKey);
}

}  // namespace wpilibws
