/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/WebSocket.h>
#include <wpi/json.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Stream.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>

namespace wpilibws {

class WebServerClientTest {
 public:
  using BufferPool = wpi::uv::SimpleBufferPool<4>;
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;

  explicit WebServerClientTest(wpi::uv::Loop& loop) : m_loop(loop) {}
  WebServerClientTest(const WebServerClientTest&) = delete;
  WebServerClientTest& operator=(const WebServerClientTest&) = delete;

  bool Initialize();
  void AttemptConnect();

  void SendMessage(const wpi::json& msg);
  const wpi::json& GetLastMessage();
  bool IsConnectedWS() { return m_ws_connected; }

 private:
  void InitializeWebSocket(const std::string& host, int port,
                           const std::string& uri);

  bool m_tcp_connected = false;
  std::shared_ptr<wpi::uv::Timer> m_connect_timer;
  int m_connect_attempts = 0;
  wpi::uv::Loop& m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_tcp_client;
  wpi::json m_json;

  bool m_ws_connected = false;
  std::shared_ptr<wpi::WebSocket> m_websocket;
  std::shared_ptr<UvExecFunc> m_exec;
  std::unique_ptr<BufferPool> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
