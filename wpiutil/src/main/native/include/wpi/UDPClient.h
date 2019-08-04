/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UDPCLIENT_H_
#define WPIUTIL_WPI_UDPCLIENT_H_

#include <string>

#include "wpi/ArrayRef.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"
#include "wpi/mutex.h"

namespace wpi {

class Logger;

class UDPClient {
  int m_lsd;
  int m_port;
  std::string m_address;
  Logger& m_logger;

 public:
  explicit UDPClient(Logger& logger);
  UDPClient(const Twine& address, Logger& logger);
  UDPClient(const UDPClient& other) = delete;
  UDPClient(UDPClient&& other);
  ~UDPClient();

  UDPClient& operator=(const UDPClient& other) = delete;
  UDPClient& operator=(UDPClient&& other);

  int start();
  int start(int port);
  void shutdown();
  // The passed in address MUST be a resolved IP address.
  int send(ArrayRef<uint8_t> data, const Twine& server, int port);
  int send(StringRef data, const Twine& server, int port);
  int receive(uint8_t* data_received, int receive_len);
  int receive(uint8_t* data_received, int receive_len,
              SmallVectorImpl<char>* addr_received, int* port_received);
  int set_timeout(double timeout);
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_UDPCLIENT_H_
