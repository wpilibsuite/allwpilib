/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_MJPEGSERVERIMPL_H_
#define CSCORE_MJPEGSERVERIMPL_H_

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <wpi/NetworkAcceptor.h>
#include <wpi/NetworkStream.h>
#include <wpi/SafeThread.h>
#include <wpi/SmallVector.h>
#include <wpi/Twine.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpi/raw_socket_ostream.h>

#include "SinkImpl.h"

namespace cs {

class SourceImpl;

class MjpegServerImpl : public SinkImpl {
 public:
  MjpegServerImpl(const wpi::Twine& name, wpi::Logger& logger,
                  Notifier& notifier, Telemetry& telemetry,
                  const wpi::Twine& listenAddress, int port,
                  std::unique_ptr<wpi::NetworkAcceptor> acceptor);
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

  std::unique_ptr<wpi::NetworkAcceptor> m_acceptor;
  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_serverThread;

  std::vector<wpi::SafeThreadOwner<ConnThread>> m_connThreads;

  // property indices
  int m_widthProp;
  int m_heightProp;
  int m_compressionProp;
  int m_defaultCompressionProp;
  int m_fpsProp;
};

}  // namespace cs

#endif  // CSCORE_MJPEGSERVERIMPL_H_
