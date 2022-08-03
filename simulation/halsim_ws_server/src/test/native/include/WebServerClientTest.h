// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/json.h>
#include <wpinet/WebSocket.h>
#include <wpinet/uv/AsyncFunction.h>
#include <wpinet/uv/Buffer.h>
#include <wpinet/uv/Loop.h>
#include <wpinet/uv/Stream.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Timer.h>

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
