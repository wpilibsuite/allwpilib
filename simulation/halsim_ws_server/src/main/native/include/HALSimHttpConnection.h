// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cinttypes>
#include <memory>
#include <utility>

#include <HALSimBaseWebSocketConnection.h>
#include <wpi/HttpWebSocketServerConnection.h>
#include <wpi/mutex.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Buffer.h>

#include "HALSimWeb.h"

namespace wpi {
class json;
}  // namespace wpi

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
  bool IsValidWsUpgrade(wpi::StringRef protocol) override;
  void ProcessWsUpgrade() override;
  void SendFileResponse(int code, const wpi::Twine& codeText,
                        const wpi::Twine& contentType,
                        const wpi::Twine& filename,
                        const wpi::Twine& extraHeader = wpi::Twine{});

  void MySendError(int code, const wpi::Twine& message);
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
