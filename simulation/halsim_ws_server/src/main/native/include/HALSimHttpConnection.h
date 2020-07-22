/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

#include <HALSimBaseWebSocketConnection.h>
#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Buffer.h>

#include <wpi/HttpWebSocketServerConnection.h>

namespace wpilibws {

class HALSimWeb;

class HALSimHttpConnection
    : public wpi::HttpWebSocketServerConnection<HALSimHttpConnection>,
      public HALSimBaseWebSocketConnection {
 public:
  using BufferPool = wpi::uv::SimpleBufferPool<4>;

  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;

  explicit HALSimHttpConnection(std::shared_ptr<wpi::uv::Stream> stream,
                                wpi::StringRef webroot_sys,
                                wpi::StringRef webroot_user)
      : wpi::HttpWebSocketServerConnection<HALSimHttpConnection>(stream, {}),
        m_webroot_sys(webroot_sys),
        m_webroot_user(webroot_user) {}

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
  // Absolute paths of folders to retrieve data from
  // -> /
  std::string m_webroot_sys;
  // -> /user
  std::string m_webroot_user;

  // is the websocket connected?
  bool m_isWsConnected = false;

  // these are only valid if the websocket is connected
  std::shared_ptr<UvExecFunc> m_exec;
  std::unique_ptr<BufferPool> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
