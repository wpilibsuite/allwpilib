/*
   TCPStream.h

   TCPStream class definition. TCPStream provides methods to trasnfer
   data between peers over a TCP/IP connection.

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

#include "TCPStream.h"

#include <arpa/inet.h>
#include <unistd.h>

TCPStream::TCPStream(int sd, struct sockaddr_in* address) : m_sd(sd) {
  char ip[50];
  inet_ntop(PF_INET, (struct in_addr*)&(address->sin_addr.s_addr), ip,
            sizeof(ip) - 1);
  m_peerIP = ip;
  m_peerPort = ntohs(address->sin_port);
}

TCPStream::~TCPStream() { close(); }

std::size_t TCPStream::send(const char* buffer, std::size_t len, Error* err) {
  if (m_sd < 0) {
    *err = kConnectionClosed;
    return 0;
  }
  ssize_t rv = write(m_sd, buffer, len);
  if (rv < 0) {
    *err = kConnectionReset;
    return 0;
  }
  return static_cast<std::size_t>(rv);
}

std::size_t TCPStream::receive(char* buffer, std::size_t len, Error* err,
                               int timeout) {
  if (m_sd < 0) {
    *err = kConnectionClosed;
    return 0;
  }
  ssize_t rv;
  if (timeout <= 0)
    rv = read(m_sd, buffer, len);
  else if (WaitForReadEvent(timeout))
    rv = read(m_sd, buffer, len);
  else {
    *err = kConnectionTimedOut;
    return 0;
  }
  if (rv < 0) {
    *err = kConnectionReset;
    return 0;
  }
  return static_cast<std::size_t>(rv);
}

void TCPStream::close() {
  if (m_sd >= 0) {
    ::shutdown(m_sd, SHUT_RDWR);
    ::close(m_sd);
  }
  m_sd = -1;
}

llvm::StringRef TCPStream::getPeerIP() const { return m_peerIP; }

int TCPStream::getPeerPort() const { return m_peerPort; }

bool TCPStream::WaitForReadEvent(int timeout) {
  fd_set sdset;
  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  FD_ZERO(&sdset);
  FD_SET(m_sd, &sdset);
  if (select(m_sd + 1, &sdset, NULL, NULL, &tv) > 0) {
    return true;
  }
  return false;
}
