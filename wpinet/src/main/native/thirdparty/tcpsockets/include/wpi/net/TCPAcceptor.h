/*
   TCPAcceptor.h

   TCPAcceptor class interface. TCPAcceptor provides methods to passively
   establish TCP/IP connections with clients.

   ------------------------------------------

   Copyright (c) 2013 [Vic Hargrave - http://vichargrave.com]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef WPINET_TCPACCEPTOR_H_
#define WPINET_TCPACCEPTOR_H_

#include <atomic>
#include <memory>
#include <string>
#include <string_view>

#include "wpinet/NetworkAcceptor.h"
#include "wpinet/TCPStream.h"

namespace wpi {

class Logger;

class TCPAcceptor : public NetworkAcceptor {
  int m_lsd;
  int m_port;
  std::string m_address;
  bool m_listening;
  std::atomic_bool m_shutdown;
  Logger& m_logger;

 public:
  TCPAcceptor(int port, std::string_view address, Logger& logger);
  ~TCPAcceptor() override;

  int start() override;
  void shutdown() final;
  std::unique_ptr<NetworkStream> accept() override;
};

}  // namespace wpi

#endif  // WPINET_TCPACCEPTOR_H_
