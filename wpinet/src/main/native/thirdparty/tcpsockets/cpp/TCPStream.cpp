/*
   TCPStream.h

   TCPStream class definition. TCPStream provides methods to transfer
   data between peers over a TCP/IP connection.

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

#include "wpinet/TCPStream.h"

#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

#include <cerrno>

#include <wpi/StringExtras.h>

using namespace wpi;

TCPStream::TCPStream(int sd, sockaddr_in* address)
    : m_sd(sd), m_blocking(true) {
  char ip[50];
#ifdef _WIN32
  InetNtop(PF_INET, &(address->sin_addr.s_addr), ip, sizeof(ip) - 1);
#else
  inet_ntop(PF_INET, reinterpret_cast<in_addr*>(&(address->sin_addr.s_addr)),
            ip, sizeof(ip) - 1);
#ifdef SO_NOSIGPIPE
  // disable SIGPIPE on Mac OS X
  int set = 1;
  setsockopt(m_sd, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<char*>(&set),
             sizeof set);
#endif
#endif
  m_peerIP = ip;
  m_peerPort = ntohs(address->sin_port);
}

TCPStream::~TCPStream() {
  close();
}

size_t TCPStream::send(const char* buffer, size_t len, Error* err) {
  if (m_sd < 0) {
    *err = CONNECTION_CLOSED;
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
    if (!m_blocking) {
      *err = WOULD_BLOCK;
      return 0;
    }
    Sleep(1);
  }
  if (!result) {
    char Buffer[128];
    wpi::format_to_n_c_str(Buffer, sizeof(Buffer),
                           "Send() failed: WSA error={}\n", WSAGetLastError());

    OutputDebugStringA(Buffer);
    *err = CONNECTION_RESET;
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
    if (!m_blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      *err = WOULD_BLOCK;
    } else {
      *err = CONNECTION_RESET;
    }
    return 0;
  }
#endif
  return static_cast<size_t>(rv);
}

size_t TCPStream::receive(char* buffer, size_t len, Error* err, int timeout) {
  if (m_sd < 0) {
    *err = CONNECTION_CLOSED;
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
  } else if (WaitForReadEvent(timeout)) {
#ifdef _WIN32
    rv = recv(m_sd, buffer, len, 0);
#else
    rv = read(m_sd, buffer, len);
#endif
  } else {
    *err = CONNECTION_TIMED_OUT;
    return 0;
  }
  if (rv < 0) {
#ifdef _WIN32
    if (!m_blocking && WSAGetLastError() == WSAEWOULDBLOCK) {
#else
    if (!m_blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
#endif
      *err = WOULD_BLOCK;
    } else {
      *err = CONNECTION_RESET;
    }
    return 0;
  }
  return static_cast<size_t>(rv);
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

std::string_view TCPStream::getPeerIP() const {
  return m_peerIP;
}

int TCPStream::getPeerPort() const {
  return m_peerPort;
}

void TCPStream::setNoDelay() {
  if (m_sd < 0) {
    return;
  }
  int optval = 1;
  setsockopt(m_sd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&optval),
             sizeof optval);
}

bool TCPStream::setBlocking(bool enabled) {
  if (m_sd < 0) {
    return true;  // silently accept
  }
#ifdef _WIN32
  u_long mode = enabled ? 0 : 1;
  if (ioctlsocket(m_sd, FIONBIO, &mode) == SOCKET_ERROR) {
    return false;
  }
#else
  int flags = fcntl(m_sd, F_GETFL, nullptr);
  if (flags < 0) {
    return false;
  }
  if (enabled) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }
  if (fcntl(m_sd, F_SETFL, flags) < 0) {
    return false;
  }
#endif
  return true;
}

int TCPStream::getNativeHandle() const {
  return m_sd;
}

bool TCPStream::WaitForReadEvent(int timeout) {
  fd_set sdset;
  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  FD_ZERO(&sdset);
  FD_SET(m_sd, &sdset);
  if (select(m_sd + 1, &sdset, nullptr, nullptr, &tv) > 0) {
    return true;
  }
  return false;
}
