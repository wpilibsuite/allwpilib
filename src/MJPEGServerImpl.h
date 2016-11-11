/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_MJPEGSERVERIMPL_H_
#define CS_MJPEGSERVERIMPL_H_

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "llvm/raw_ostream.h"
#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"
#include "support/SafeThread.h"
#include "support/raw_istream.h"
#include "support/raw_socket_ostream.h"
#include "tcpsockets/NetworkAcceptor.h"
#include "tcpsockets/NetworkStream.h"

#include "SinkImpl.h"

namespace cs {

class SourceImpl;

class MJPEGServerImpl : public SinkImpl {
 public:
  MJPEGServerImpl(llvm::StringRef name, llvm::StringRef description,
                  std::unique_ptr<wpi::NetworkAcceptor> acceptor);
  ~MJPEGServerImpl() override;

  void Stop();

 private:
  void SetSourceImpl(std::shared_ptr<SourceImpl> source) override;

  void ServerThreadMain();

  class ConnThread;

  std::unique_ptr<wpi::NetworkAcceptor> m_acceptor;
  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_serverThread;

  std::vector<wpi::SafeThreadOwner<ConnThread>> m_connThreads;
};

}  // namespace cs

#endif  // CS_MJPEGSERVERIMPL_H_
