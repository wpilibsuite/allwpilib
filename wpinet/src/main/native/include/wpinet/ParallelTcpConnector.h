// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "wpinet/uv/Timer.h"

namespace wpi {

class Logger;

namespace uv {
class GetAddrInfoReq;
class Loop;
class Tcp;
class Timer;
}  // namespace uv

/**
 * Parallel TCP connector.  Attempts parallel resolution and connection to
 * multiple servers with automatic retry if none connect.
 *
 * Each successful TCP connection results in a call to the connected callback.
 * For correct operation, the consuming code (either the connected callback or
 * e.g. task it starts) must call Succeeded() to indicate if the connection has
 * succeeded prior to the reconnect rate timeout.  A successful connection
 * results in the connector terminating all other connection attempts.
 *
 * After the reconnect rate times out, all remaining active connection attempts
 * are canceled and new ones started.
 */
class ParallelTcpConnector
    : public std::enable_shared_from_this<ParallelTcpConnector> {
  struct private_init {};

 public:
  /**
   * Create.
   *
   * @param loop loop
   * @param reconnectRate how long to wait after starting connection attempts
   *                      to cancel and attempt connecting again
   * @param logger logger
   * @param connected callback function when a connection succeeds; may be
   *                  called multiple times if it does not call Succeeded()
   *                  before returning
   * @return Parallel connector
   */
  static std::shared_ptr<ParallelTcpConnector> Create(
      wpi::uv::Loop& loop, wpi::uv::Timer::Time reconnectRate,
      wpi::Logger& logger, std::function<void(wpi::uv::Tcp& tcp)> connected) {
    if (loop.IsClosing()) {
      return nullptr;
    }
    return std::make_shared<ParallelTcpConnector>(
        loop, reconnectRate, logger, std::move(connected), private_init{});
  }

  ParallelTcpConnector(wpi::uv::Loop& loop, wpi::uv::Timer::Time reconnectRate,
                       wpi::Logger& logger,
                       std::function<void(wpi::uv::Tcp& tcp)> connected,
                       const private_init&);
  ~ParallelTcpConnector();

  ParallelTcpConnector(const ParallelTcpConnector&) = delete;
  ParallelTcpConnector& operator=(const ParallelTcpConnector&) = delete;

  /**
   * Closes resources, canceling all pending action attempts.
   */
  void Close();

  /**
   * Changes the servers/ports to connect to.  Starts connection attempts if not
   * already connected.
   *
   * @param servers array of server/port pairs
   */
  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers);

  /**
   * Tells the parallel connector that the current connection has terminated and
   * it is necessary to start reconnection attempts.
   */
  void Disconnected();

  /**
   * Tells the parallel connector that a particular connection has succeeded and
   * it should stop trying to connect.
   *
   * @param tcp connection passed to connected callback
   */
  void Succeeded(wpi::uv::Tcp& tcp);

 private:
  bool IsConnected() const { return m_isConnected || m_servers.empty(); }
  void Connect();
  void CancelAll(wpi::uv::Tcp* except = nullptr);

  wpi::uv::Loop& m_loop;
  wpi::Logger& m_logger;
  wpi::uv::Timer::Time m_reconnectRate;
  std::function<void(wpi::uv::Tcp& tcp)> m_connected;
  std::shared_ptr<wpi::uv::Timer> m_reconnectTimer;
  std::vector<std::pair<std::string, unsigned int>> m_servers;
  std::vector<std::weak_ptr<wpi::uv::GetAddrInfoReq>> m_resolvers;
  std::vector<std::pair<sockaddr_storage, std::weak_ptr<wpi::uv::Tcp>>>
      m_attempts;
  bool m_isConnected{false};
};

}  // namespace wpi
