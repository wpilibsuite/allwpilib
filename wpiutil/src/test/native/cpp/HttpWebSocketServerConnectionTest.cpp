/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/HttpWebSocketServerConnection.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

namespace wpi {

class HttpWebSocketServerConnectionTest
    : public HttpWebSocketServerConnection<HttpWebSocketServerConnectionTest> {
 public:
  HttpWebSocketServerConnectionTest(std::shared_ptr<uv::Stream> stream,
                                    ArrayRef<StringRef> protocols)
      : HttpWebSocketServerConnection{stream, protocols} {}

  void ProcessRequest() override { ++gotRequest; }
  void ProcessWsUpgrade() override { ++gotUpgrade; }

  int gotRequest = 0;
  int gotUpgrade = 0;
};

}  // namespace wpi
