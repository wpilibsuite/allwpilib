/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <HALSimBaseWebSocketConnection.h>
#include <wpi/WebSocket.h>
#include <wpi/mutex.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/Stream.h>

#include "HALSimWS.h"

namespace wpi {
class json;
}  // namespace wpi

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
