// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cinttypes>
#include <memory>
#include <string_view>
#include <utility>

#include "wpi/halsim/ws_core/HALSimBaseWebSocketConnection.hpp"
#include "wpi/util/json_fwd.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/net/HttpWebSocketServerConnection.hpp"
#include "wpi/net/uv/AsyncFunction.hpp"
#include "wpi/net/uv/Buffer.hpp"

#include "wpi/halsim/ws_server/HALSimWeb.hpp"

namespace wpilibws {

class HALSimHttpConnection
    : public wpi::HttpWebSocketServerConnection<HALSimHttpConnection>,
      public HALSimBaseWebSocketConnection {
 public:
  HALSimHttpConnection(std::shared_ptr<HALSimWeb> server,
                       std::shared_ptr<wpi::uv::Stream> stream)
      : wpi::HttpWebSocketServerConnection<HALSimHttpConnection>(stream, {}),
        m_server(std::move(server)),
        m_buffers(128) {}

 public:
  // callable from any thread
  void OnSimValueChanged(const wpi::json& msg) override;

 protected:
  void ProcessRequest() override;
  bool IsValidWsUpgrade(std::string_view protocol) override;
  void ProcessWsUpgrade() override;
  void SendFileResponse(int code, std::string_view codeText,
                        std::string_view contentType, std::string_view filename,
                        std::string_view extraHeader = {});

  void MySendError(int code, std::string_view message);
  void Log(int code);

 private:
  std::shared_ptr<HALSimWeb> m_server;

  // is the websocket connected?
  bool m_isWsConnected = false;

  // these are only valid if the websocket is connected
  wpi::uv::SimpleBufferPool<4> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
