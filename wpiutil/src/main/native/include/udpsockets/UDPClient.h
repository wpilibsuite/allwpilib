/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_UDPSOCKETS_UDPCLIENT_H_
#define WPIUTIL_UDPSOCKETS_UDPCLIENT_H_

#include <string>

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"
#include "support/mutex.h"

namespace wpi {

class Logger;

class UDPClient {
  int m_lsd;
  std::string m_address;
  Logger& m_logger;

 public:
  explicit UDPClient(Logger& logger);
  UDPClient(llvm::StringRef address, Logger& logger);
  UDPClient(const UDPClient& other) = delete;
  UDPClient(UDPClient&& other);
  ~UDPClient();

  UDPClient& operator=(const UDPClient& other) = delete;
  UDPClient& operator=(UDPClient&& other);

  int start();
  void shutdown();
  // The passed in address MUST be a resolved IP address.
  int send(llvm::ArrayRef<uint8_t> data, llvm::StringRef server, int port);
  int send(llvm::StringRef data, llvm::StringRef server, int port);
};

}  // namespace wpi

#endif  // WPIUTIL_UDPSOCKETS_UDPCLIENT_H_
