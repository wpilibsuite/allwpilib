// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <wpi/halsim/ws_core/HALSimBaseWebSocketConnection.hpp>
#include <wpi/net/WebSocket.hpp>
#include <wpi/net/uv/Buffer.hpp>
#include <wpi/net/uv/Stream.hpp>
#include <wpi/util/json_fwd.hpp>
#include <wpi/util/mutex.hpp>

#include "wpi/halsim/ws_client/HALSimWS.hpp"

namespace wpilibws {

class HALSimWS;

class HALSimWSClientConnection
    : public HALSimBaseWebSocketConnection,
      public std::enable_shared_from_this<HALSimWSClientConnection> {
 public:
  explicit HALSimWSClientConnection(
      std::shared_ptr<HALSimWS> client,
      std::shared_ptr<wpi::net::uv::Stream> stream)
      : m_client(std::move(client)),
        m_stream(std::move(stream)),
        m_buffers(128) {}

 public:
  void OnSimValueChanged(const wpi::util::json& msg) override;
  void Initialize();

 private:
  std::shared_ptr<HALSimWS> m_client;
  std::shared_ptr<wpi::net::uv::Stream> m_stream;

  bool m_ws_connected = false;
  wpi::net::WebSocket* m_websocket = nullptr;

  wpi::net::uv::SimpleBufferPool<4> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
