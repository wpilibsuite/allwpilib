/*
   TCPConnector.h

   TCPConnector class definition. TCPConnector provides methods to actively
   establish TCP/IP connections with a server.

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
   limitations under the License
*/

#include "TCPConnector.h"

#include <errno.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Log.h"

static int ResolveHostName(const char* hostname, struct in_addr* addr) {
  struct addrinfo* res;

  int result = getaddrinfo(hostname, nullptr, nullptr, &res);
  if (result == 0) {
    std::memcpy(addr, &((struct sockaddr_in*)res->ai_addr)->sin_addr,
                sizeof(struct in_addr));
    freeaddrinfo(res);
  }
  return result;
}

std::unique_ptr<TCPStream> TCPConnector::connect(const char* server, int port) {
  struct sockaddr_in address;

  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (ResolveHostName(server, &(address.sin_addr)) != 0) {
    inet_pton(PF_INET, server, &(address.sin_addr));
  }
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (::connect(sd, (struct sockaddr*)&address, sizeof(address)) != 0) {
    DEBUG("connect() failed: " << strerror(errno));
    return nullptr;
  }
  return std::unique_ptr<TCPStream>(new TCPStream(sd, &address));
}

std::unique_ptr<TCPStream> TCPConnector::connect(const char* server, int port,
                                                 int timeout) {
  if (timeout == 0) return connect(server, port);

  struct sockaddr_in address;

  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (ResolveHostName(server, &(address.sin_addr)) != 0) {
    inet_pton(PF_INET, server, &(address.sin_addr));
  }

  long arg;
  fd_set sdset;
  struct timeval tv;
  socklen_t len;
  int result = -1, valopt, sd = socket(AF_INET, SOCK_STREAM, 0);

  // Set socket to non-blocking
  arg = fcntl(sd, F_GETFL, nullptr);
  arg |= O_NONBLOCK;
  fcntl(sd, F_SETFL, arg);

  // Connect with time limit
  std::string message;
  if ((result = ::connect(sd, (struct sockaddr*)&address, sizeof(address))) <
      0) {
    if (errno == EINPROGRESS) {
      tv.tv_sec = timeout;
      tv.tv_usec = 0;
      FD_ZERO(&sdset);
      FD_SET(sd, &sdset);
      if (select(sd + 1, nullptr, &sdset, nullptr, &tv) > 0) {
        len = sizeof(int);
        getsockopt(sd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &len);
        if (valopt) {
          DEBUG("connect() error " << valopt << " - " << strerror(valopt));
        }
        // connection established
        else
          result = 0;
      } else
        DEBUG("connect() timed out");
    } else
      DEBUG("connect() error " << errno << " - " << strerror(errno));
  }

  // Return socket to blocking mode
  arg = fcntl(sd, F_GETFL, nullptr);
  arg &= (~O_NONBLOCK);
  fcntl(sd, F_SETFL, arg);

  // Create stream object if connected
  if (result == -1) return nullptr;
  return std::unique_ptr<TCPStream>(new TCPStream(sd, &address));
}
