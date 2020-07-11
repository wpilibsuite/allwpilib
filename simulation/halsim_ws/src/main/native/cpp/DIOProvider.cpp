#include "DIOProvider.h"

#include <hal/Ports.h>
#include <hal/simulation/DIOData.h>

#include "MessageSchema.h"

#include <iostream>

void DIOProvider::Initialize(WSRegisterFunc wsRegisterFunc) {
  CreateProviders<DIOProvider>("DIO", HAL_GetNumDigitalChannels(), HALSIM_RegisterDIOAllCallbacks, wsRegisterFunc);
}

wpi::json DIOProvider::OnSimValueChanged(const char* name) {
  std::string cbType(name);

  // Bail out if we're not initialized yet
  if (!HALSIM_GetDIOInitialized(m_channel)) {
    return {};
  }

  if (cbType == "Initialized" || cbType == "IsInput") {
    wpilibws::digitalio::config::Message configMessage;
    configMessage.channel = m_channel;
    configMessage.isInput = HALSIM_GetDIOIsInput(m_channel);

    return {
      {"topic", "digital/config"},
      {"payload", configMessage }
    };
  }
  else if (cbType == "Value" && !HALSIM_GetDIOIsInput(m_channel)) {
    wpilibws::digitalio::value::Message valueMessage;
    valueMessage.channel = m_channel;
    valueMessage.value = HALSIM_GetDIOValue(m_channel);

    return {
      {"topic", "digital/out"},
      {"payload", valueMessage }
    };
  }


  return {};
}

void DIOProvider::OnEndpointValueChanged(const wpi::json& msg) {
  // Bail out if this was not configured as an input
  if (!HALSIM_GetDIOIsInput(m_channel)) {
    return;
  }

  // TODO If at some point we want to support more than just "value"
  // e.g rising/falling edge etc, we'll have to specify that here

  auto payload = msg["payload"];
  auto message = payload.get<wpilibws::digitalio::value::Message>();

  HALSIM_SetDIOValue(m_channel, message.value);
}