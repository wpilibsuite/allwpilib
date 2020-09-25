/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSHalProviders.h"

#include <mutex>

namespace wpilibws {

void HALSimWSHalProvider::OnNetworkConnected(
    wpi::StringRef clientId,
    std::shared_ptr<HALSimBaseWebSocketConnection> ws) {
  std::scoped_lock lock(m_connsLock);

  bool shouldRegisterCallbacks = m_conns.empty();
  m_conns.insert(std::make_pair(clientId, ws));

  if (shouldRegisterCallbacks) {
    RegisterCallbacks();
  }
}

void HALSimWSHalProvider::OnNetworkDisconnected(wpi::StringRef clientId) {
  std::scoped_lock lock(m_connsLock);

  m_conns.erase(clientId);
  if (m_conns.empty()) {
    CancelCallbacks();
  }
}

void HALSimWSHalProvider::ProcessHalCallback(const wpi::json& payload) {
  std::scoped_lock lock(m_connsLock);
  for (auto it = m_conns.begin(); it != m_conns.end(); it++) {
    auto ws = it->getValue().lock();
    if (ws) {
      wpi::json netValue = {
          {"type", m_type}, {"device", m_deviceId}, {"data", payload}};
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
