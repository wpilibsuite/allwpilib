// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "INetworkClient.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
class ILocalStorage;
}  // namespace nt::net

namespace nt {

class IConnectionList;

class NetworkClient final : public INetworkClient {
 public:
  NetworkClient(int inst, std::string_view id, net::ILocalStorage& localStorage,
                IConnectionList& connList, wpi::Logger& logger);
  ~NetworkClient() final;

  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) final;

  void StartDSClient(unsigned int port) final;
  void StopDSClient() final;

  void FlushLocal() final;
  void Flush() final;

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

class NetworkClient3 final : public INetworkClient {
 public:
  NetworkClient3(int inst, std::string_view id,
                 net::ILocalStorage& localStorage, IConnectionList& connList,
                 wpi::Logger& logger);
  ~NetworkClient3() final;

  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) final;

  void StartDSClient(unsigned int port) final;
  void StopDSClient() final;

  void FlushLocal() final;
  void Flush() final;

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace nt
