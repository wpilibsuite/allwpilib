/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSHalProviders.h"

namespace wpilibws {

void HALSimWSHalProvider::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> ws) {
  {
    std::lock_guard lock(mutex);
    
    // store a weak reference to the websocket object
    m_ws = ws;
  }

  // trigger a send of the current state
  // -> even if this gets called before, it's ok, because we cleared the
  //    state above atomically
  OnSimCallback(""); // TODO Should we send a full state message?
}

void HALSimWSHalProvider::OnStaticSimCallback(const char* name, void* param,
                                              const struct HAL_Value* value) {
  static_cast<HALSimWSHalProvider*>(param)->OnSimCallback(name);
}

void HALSimWSHalProvider::OnSimCallback(const char* cbName) {
  // Ensures all operations are performed in-order
  // -> this includes the network send
  std::lock_guard lock(mutex);

  // OnSimValueChanged will only return the values of interest
  // so we won't need to diff
  auto result = OnSimValueChanged(cbName);
  if (!result.empty()) {
    auto ws = m_ws.lock();
    if (ws) {
      wpi::json netValue = {
          {"type", m_type}, {"device", m_deviceId}, {"data", result}};
      ws->OnSimValueChanged(netValue);
    }
  }
}

HALSimWSHalChanProvider::HALSimWSHalChanProvider(int32_t channel,
                                                 const std::string& key,
                                                 const std::string& type)
    : HALSimWSHalProvider(key, type), m_channel(channel) {
  m_deviceId = std::to_string(channel);
}

}  // namespace wpilibws
