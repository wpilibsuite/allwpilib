// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UDPCLIENT_H_
#define WPINET_UDPCLIENT_H_

#include <span>
#include <string>
#include <string_view>

#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

namespace wpi {

class Logger;

class UDPClient {
  int m_lsd;
  int m_port;
  std::string m_address;
  Logger& m_logger;

 public:
  explicit UDPClient(Logger& logger);
  UDPClient(std::string_view address, Logger& logger);
  UDPClient(const UDPClient& other) = delete;
  UDPClient(UDPClient&& other);
  ~UDPClient();

  UDPClient& operator=(const UDPClient& other) = delete;
  UDPClient& operator=(UDPClient&& other);

  int start();
  int start(int port);
  void shutdown();
  // The passed in address MUST be a resolved IP address.
  int send(std::span<const uint8_t> data, std::string_view server, int port);
  int send(std::string_view data, std::string_view server, int port);
  int receive(uint8_t* data_received, int receive_len);
  int receive(uint8_t* data_received, int receive_len,
              SmallVectorImpl<char>* addr_received, int* port_received);
  int set_timeout(double timeout);
};

}  // namespace wpi

#endif  // WPINET_UDPCLIENT_H_
