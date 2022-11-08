// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string_view>

#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
class ILocalStorage;
}  // namespace nt::net

namespace nt {

class IConnectionList;

class NetworkServer {
 public:
  NetworkServer(std::string_view persistentFilename,
                std::string_view listenAddress, unsigned int port3,
                unsigned int port4, net::ILocalStorage& localStorage,
                IConnectionList& connList, wpi::Logger& logger,
                std::function<void()> initDone);
  ~NetworkServer();

  void FlushLocal();
  void Flush();

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace nt
