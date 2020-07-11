#include "AnalogProvider.h"
#include "MessageSchema.h"

#include <hal/Ports.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/AnalogOutData.h>

void AnalogInProvider::Initialize(WSRegisterFunc wsRegisterFunc) {
  CreateProviders<AnalogInProvider>("AIN",
                                    HAL_GetNumAnalogInputs(),
                                    HALSIM_RegisterAnalogInAllCallbacks,
                                    wsRegisterFunc);
}

wpi::json AnalogInProvider::OnSimValueChanged(const char* name) {
  // We can technically just ignore this, since this is an input
  return {};
}

void AnalogInProvider::OnEndpointValueChanged(const wpi::json& msg) {
  auto payload = msg["payload"];
  auto message = payload.get<wpilibws::analogio::value::Message>();

  HALSIM_SetAnalogInVoltage(m_channel, message.voltage);
}

void AnalogOutProvider::Initialize(WSRegisterFunc wsRegisterFunc) {
  CreateProviders<AnalogOutProvider>("AOUT",
                                     HAL_GetNumAnalogOutputs(),
                                     HALSIM_RegisterAnalogOutAllCallbacks,
                                     wsRegisterFunc);
}

wpi::json AnalogOutProvider::OnSimValueChanged(const char* name) {
  std::string cbType(name);

  // Bail out if we're not initialized yet
  if (!HALSIM_GetAnalogOutInitialized(m_channel)) {
    return {};
  }

  if (cbType == "Voltage") {
    wpilibws::analogio::value::Message valueMessage;
    valueMessage.channel = m_channel;
    valueMessage.voltage = HALSIM_GetAnalogOutVoltage(m_channel);

    return {
      { "topic", "analog/out" },
      { "payload", valueMessage }
    };
  }

  return {};
}