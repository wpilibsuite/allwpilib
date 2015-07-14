/*
   TCPAcceptor.cpp

   TCPAcceptor class definition. TCPAcceptor provides methods to passively
   establish TCP/IP connections with clients.

   ------------------------------------------

   Copyright © 2013 [Vic Hargrave - http://vichargrave.com]

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

#include "TCPAcceptor.h"

#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

TCPAcceptor::TCPAcceptor(int port, const char* address)
    : m_lsd(0), m_port(port), m_address(address), m_listening(false) {}

TCPAcceptor::~TCPAcceptor() {
  if (m_lsd > 0) {
    close(m_lsd);
  }
}

int TCPAcceptor::start() {
  if (m_listening) return 0;

  m_lsd = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;

  std::memset(&address, 0, sizeof(address));
  address.sin_family = PF_INET;
  address.sin_port = htons(m_port);
  if (m_address.size() > 0) {
    inet_pton(PF_INET, m_address.c_str(), &(address.sin_addr));
  } else {
    address.sin_addr.s_addr = INADDR_ANY;
  }

  int optval = 1;
  setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

  int result = bind(m_lsd, (struct sockaddr*)&address, sizeof(address));
  if (result != 0) {
    perror("bind() failed");
    return result;
  }

  result = listen(m_lsd, 5);
  if (result != 0) {
    perror("listen() failed");
    return result;
  }
  m_listening = true;
  return result;
}

std::unique_ptr<TCPStream> TCPAcceptor::accept() {
  if (!m_listening) return nullptr;

  struct sockaddr_in address;
  socklen_t len = sizeof(address);
  std::memset(&address, 0, sizeof(address));
  int sd = ::accept(m_lsd, (struct sockaddr*)&address, &len);
  if (sd < 0) {
    perror("accept() failed");
    return nullptr;
  }
  return std::unique_ptr<TCPStream>(new TCPStream(sd, &address));
}
