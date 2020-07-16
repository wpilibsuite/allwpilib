#pragma once

#include <memory>
#include <string>

#include <wpi/json.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>
#include <wpi/uv/AsyncFunction.h>

#include <WSProviderContainer.h>

namespace wpilibws {

class HALSimWSClientConnection;

class HALSimWS {
  public:
    static std::shared_ptr<HALSimWS> GetInstance() { return g_instance; }
    static void SetInstance(std::shared_ptr<HALSimWS> inst) {
      g_instance = inst;
    }

    HALSimWS(ProviderContainer& providers) : m_providers(providers) {}
    HALSimWS(const HALSimWS&) = delete;
    HALSimWS& operator=(const HALSimWS&) = delete;

    bool Initialize();
    static void Main(void*);
    static void Exit(void*);

    bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
    void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

    void OnNetValueChanged(const wpi::json& msg);

    const std::string GetTargetHost() { return m_host; }
    const std::string GetTargetUri() { return m_uri; }
    int GetTargetPort() { return m_port; }

  private:
    static std::shared_ptr<HALSimWS> g_instance;

    void MainLoop();

    void AttemptConnect();

    bool m_tcp_connected = false;
    std::shared_ptr<wpi::uv::Timer> m_connect_timer;
    int m_connect_attempts = 0;

    std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

    ProviderContainer& m_providers;

    std::shared_ptr<wpi::uv::Loop> m_loop;
    std::shared_ptr<wpi::uv::Tcp> m_tcp_client;

    std::string m_host;
    std::string m_uri;
    int m_port;
};

}