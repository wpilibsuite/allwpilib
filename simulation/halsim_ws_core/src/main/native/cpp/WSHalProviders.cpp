// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSHalProviders.hpp"

#include <memory>

#include <fmt/format.h>

namespace wpilibws {

void HALSimWSHalProvider::OnNetworkConnected(
    std::shared_ptr<HALSimBaseWebSocketConnection> ws) {
  {
    // store a weak reference to the websocket object
    m_ws = ws;
  }

  RegisterCallbacks();
}

void HALSimWSHalProvider::OnNetworkDisconnected() {
  CancelCallbacks();
}

void HALSimWSHalProvider::ProcessHalCallback(const wpi::util::json& payload) {
  auto ws = m_ws.lock();
  if (ws) {
    auto netValue = wpi::util::json::object();
    netValue["type"] = m_type;
    netValue["device"] = m_deviceId;
    netValue["data"] = payload;
    ws->OnSimValueChanged(netValue);
  }
}

HALSimWSHalChanProvider::HALSimWSHalChanProvider(int32_t channel,
                                                 std::string_view key,
                                                 std::string_view type)
    : HALSimWSHalProvider(key, type), m_channel(channel) {
  m_deviceId = fmt::format("{}", channel);
}

}  // namespace wpilibws
