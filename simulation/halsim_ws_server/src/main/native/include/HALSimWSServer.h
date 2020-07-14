/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/StringMap.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

#include <WSProviderContainer.h>
#include <WSBaseProvider.h>

namespace wpilibws {

class HALSimHttpConnection;

class HALSimWeb {
 public:
  static std::shared_ptr<HALSimWeb> GetInstance() { return g_instance; }
  static void SetInstance(std::shared_ptr<HALSimWeb> inst) {
    g_instance = inst;
  }

  HALSimWeb(ProviderContainer& providers) : m_providers(providers) {}

  HALSimWeb(const HALSimWeb&) = delete;
  HALSimWeb& operator=(const HALSimWeb&) = delete;

  bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

  // network -> sim
  void OnNetValueChanged(const wpi::json& msg);

 private:
  static std::shared_ptr<HALSimWeb> g_instance;

  void MainLoop();

  // connected http connection that contains active websocket
  std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

  // list of providers
  ProviderContainer& m_providers;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_server;

  // Absolute paths of folders to retrieve data from
  // -> /
  std::string m_webroot_sys;
  // -> /user
  std::string m_webroot_user;
};

}