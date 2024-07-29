// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <HALSimBaseWebSocketConnection.h>
#include <wpi/json_fwd.h>
#include <wpi/mutex.h>
#include <wpinet/WebSocket.h>
#include <wpinet/uv/Buffer.h>
#include <wpinet/uv/Stream.h>

#include "HALSimWS.h"

namespace wpilibws {

class HALSimWS;

class HALSimWSClientConnection
    : public HALSimBaseWebSocketConnection,
      public std::enable_shared_from_this<HALSimWSClientConnection> {
 public:
  explicit HALSimWSClientConnection(std::shared_ptr<HALSimWS> client,
                                    std::shared_ptr<wpi::uv::Stream> stream)
      : m_client(std::move(client)),
        m_stream(std::move(stream)),
        m_buffers(128) {}

 public:
  void OnSimValueChanged(const wpi::json& msg) override;
  void Initialize();

 private:
  std::shared_ptr<HALSimWS> m_client;
  std::shared_ptr<wpi::uv::Stream> m_stream;

  bool m_ws_connected = false;
  wpi::WebSocket* m_websocket = nullptr;

  wpi::uv::SimpleBufferPool<4> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
