/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <WSBaseProvider.h>
#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/StringMap.h>
#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

namespace wpilibws {

class HALSimHttpConnection;

class HALSimWeb {
 public:
  static std::shared_ptr<HALSimWeb> GetInstance() { return g_instance; }
  static void SetInstance(std::shared_ptr<HALSimWeb> inst) {
    g_instance = inst;
  }

  explicit HALSimWeb(ProviderContainer& providers,
                     HALSimWSProviderSimDevices& simDevicesProvider)
      : m_providers(providers), m_simDevicesProvider(simDevicesProvider) {}

  HALSimWeb(const HALSimWeb&) = delete;
  HALSimWeb& operator=(const HALSimWeb&) = delete;

  bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

  // network -> sim
  void OnNetValueChanged(const wpi::json& msg);

  std::string GetServerUri() const { return m_uri; }
  int GetServerPort() { return m_port; }
  std::shared_ptr<wpi::uv::Loop> GetLoop() { return m_loop; }

 private:
  static std::shared_ptr<HALSimWeb> g_instance;

  void MainLoop();

  // connected http connection that contains active websocket
  std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

  // list of providers
  ProviderContainer& m_providers;
  HALSimWSProviderSimDevices& m_simDevicesProvider;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_server;

  // Absolute paths of folders to retrieve data from
  // -> /
  std::string m_webroot_sys;
  // -> /user
  std::string m_webroot_user;

  std::string m_uri;
  int m_port;
};

}  // namespace wpilibws
