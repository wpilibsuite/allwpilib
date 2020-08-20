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
    // store a weak reference to the websocket object
    m_ws = ws;
  }

  RegisterCallbacks();
}

void HALSimWSHalProvider::OnNetworkDisconnected() { CancelCallbacks(); }

void HALSimWSHalProvider::ProcessHalCallback(const wpi::json& payload) {
  auto ws = m_ws.lock();
  if (ws) {
    wpi::json netValue = {
        {"type", m_type}, {"device", m_deviceId}, {"data", payload}};
    ws->OnSimValueChanged(netValue);
  }
}

HALSimWSHalChanProvider::HALSimWSHalChanProvider(int32_t channel,
                                                 const std::string& key,
                                                 const std::string& type)
    : HALSimWSHalProvider(key, type), m_channel(channel) {
  m_deviceId = std::to_string(channel);
}

}  // namespace wpilibws
