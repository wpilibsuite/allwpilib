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

#include "tcpsockets/TCPStream.h"

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

using namespace wpi;

TCPStream::TCPStream(int sd, sockaddr_in* address) : m_sd(sd) {
  char ip[50];
#ifdef _WIN32
  unsigned long size = sizeof(ip) - 1;
  WSAAddressToString((sockaddr*)address, sizeof sockaddr_in, nullptr, ip, &size);
#else
  inet_ntop(PF_INET, (in_addr*)&(address->sin_addr.s_addr), ip,
            sizeof(ip) - 1);
#ifdef SO_NOSIGPIPE
  // disable SIGPIPE on Mac OS X
  int set = 1;
  setsockopt(m_sd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&set, sizeof set);
#endif
#endif
  m_peerIP = ip;
  m_peerPort = ntohs(address->sin_port);
}

TCPStream::~TCPStream() { close(); }

std::size_t TCPStream::send(const char* buffer, std::size_t len, Error* err) {
  if (m_sd < 0) {
    *err = kConnectionClosed;
    return 0;
  }
#ifdef _WIN32
  WSABUF wsaBuf;
  wsaBuf.buf = const_cast<char*>(buffer);
  wsaBuf.len = (ULONG)len;
  DWORD rv;
  bool result = true;
  while (WSASend(m_sd, &wsaBuf, 1, &rv, 0, nullptr, nullptr) == SOCKET_ERROR) {
    if (WSAGetLastError() != WSAEWOULDBLOCK) {
      result = false;
      break;
    }
    Sleep(1);
  }
  if (!result) {
    char Buffer[128];
#ifdef _MSC_VER
    sprintf_s(Buffer, "Send() failed: WSA error=%d\n", WSAGetLastError());
#else
    std::snprintf(Buffer, 128, "Send() failed: WSA error=%d\n", WSAGetLastError());
#endif
    OutputDebugStringA(Buffer);
    *err = kConnectionReset;
    return 0;
  }
#else
#ifdef MSG_NOSIGNAL
  // disable SIGPIPE on Linux
  ssize_t rv = ::send(m_sd, buffer, len, MSG_NOSIGNAL);
#else
  ssize_t rv = ::send(m_sd, buffer, len, 0);
#endif
  if (rv < 0) {
    *err = kConnectionReset;
    return 0;
  }
#endif
  return static_cast<std::size_t>(rv);
}

std::size_t TCPStream::receive(char* buffer, std::size_t len, Error* err,
                               int timeout) {
  if (m_sd < 0) {
    *err = kConnectionClosed;
    return 0;
  }
#ifdef _WIN32
  int rv;
#else
  ssize_t rv;
#endif
  if (timeout <= 0) {
#ifdef _WIN32
    rv = recv(m_sd, buffer, len, 0);
#else
    rv = read(m_sd, buffer, len);
#endif
  }
  else if (WaitForReadEvent(timeout)) {
#ifdef _WIN32
    rv = recv(m_sd, buffer, len, 0);
#else
    rv = read(m_sd, buffer, len);
#endif
  } else {
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
#ifdef _WIN32
    ::shutdown(m_sd, SD_BOTH);
    closesocket(m_sd);
#else
    ::shutdown(m_sd, SHUT_RDWR);
    ::close(m_sd);
#endif
  }
  m_sd = -1;
}

llvm::StringRef TCPStream::getPeerIP() const { return m_peerIP; }

int TCPStream::getPeerPort() const { return m_peerPort; }

void TCPStream::setNoDelay() {
  int optval = 1;
  setsockopt(m_sd, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof optval);
}

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
