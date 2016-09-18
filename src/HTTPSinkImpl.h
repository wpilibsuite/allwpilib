/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_HTTPSINKIMPL_H_
#define CAMERASERVER_HTTPSINKIMPL_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "llvm/raw_ostream.h"
#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"
#include "support/raw_istream.h"
#include "support/raw_socket_ostream.h"
#include "tcpsockets/NetworkAcceptor.h"
#include "tcpsockets/NetworkStream.h"

#include "SinkImpl.h"

namespace cs {

class SourceImpl;

class HTTPSinkImpl : public SinkImpl {
 public:
  HTTPSinkImpl(llvm::StringRef name, llvm::StringRef description,
               std::unique_ptr<wpi::NetworkAcceptor> acceptor);
  ~HTTPSinkImpl() override;

  llvm::StringRef GetDescription(
      llvm::SmallVectorImpl<char>& buf) const override;

  void Stop();

  static void SendHeader(llvm::raw_ostream& os, int code,
                         llvm::StringRef codeText, llvm::StringRef contentType,
                         llvm::StringRef extra = llvm::StringRef{});
  static void SendError(llvm::raw_ostream& os, int code,
                        llvm::StringRef message);
  static bool ReadLine(wpi::raw_istream& istream,
                       llvm::SmallVectorImpl<char>& buffer, int maxLen);
  static bool UnescapeURI(llvm::StringRef str,
                          llvm::SmallVectorImpl<char>& out);
  static bool ProcessCommand(llvm::raw_ostream& os, SourceImpl& source,
                             llvm::StringRef parameters, bool respond);
  static void SendJSON(llvm::raw_ostream& os, SourceImpl& source, bool header);

  void SendStream(wpi::raw_socket_ostream& os);

 private:
  void ServerThreadMain();
  void ConnThreadMain(wpi::NetworkStream* stream);

  std::string m_description;

  std::unique_ptr<wpi::NetworkAcceptor> m_acceptor;
  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_serverThread;

  std::mutex m_mutex;
  std::vector<std::thread> m_connThreads;
  std::vector<std::unique_ptr<wpi::NetworkStream>> m_connStreams;
};

}  // namespace cs

#endif  // CAMERASERVER_HTTPSINKIMPL_H_
