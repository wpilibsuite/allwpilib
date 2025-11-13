// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "SinkImpl.hpp"
#include "wpi/net/NetworkAcceptor.hpp"
#include "wpi/net/NetworkStream.hpp"
#include "wpi/net/raw_socket_ostream.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/raw_istream.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace wpi::cs {

class SourceImpl;

class MjpegServerImpl : public SinkImpl {
 public:
  MjpegServerImpl(std::string_view name, wpi::util::Logger& logger,
                  Notifier& notifier, Telemetry& telemetry,
                  std::string_view listenAddress, int port,
                  std::unique_ptr<wpi::net::NetworkAcceptor> acceptor);
  ~MjpegServerImpl() override;

  void Stop();
  std::string GetListenAddress() { return m_listenAddress; }
  int GetPort() { return m_port; }

 private:
  void SetSourceImpl(std::shared_ptr<SourceImpl> source) override;

  void ServerThreadMain();

  class ConnThread;

  // Never changed, so not protected by mutex
  std::string m_listenAddress;
  int m_port;

  std::unique_ptr<wpi::net::NetworkAcceptor> m_acceptor;
  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_serverThread;

  std::vector<wpi::util::SafeThreadOwner<ConnThread>> m_connThreads;

  // property indices
  int m_widthProp;
  int m_heightProp;
  int m_compressionProp;
  int m_defaultCompressionProp;
  int m_fpsProp;
};

}  // namespace wpi::cs
