// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/HttpWebSocketServerConnection.h"  // NOLINT(build/include_order)

#include <memory>

#include <gtest/gtest.h>

namespace wpi {

class HttpWebSocketServerConnectionTest
    : public HttpWebSocketServerConnection<HttpWebSocketServerConnectionTest> {
 public:
  HttpWebSocketServerConnectionTest(std::shared_ptr<uv::Stream> stream,
                                    std::span<const std::string_view> protocols)
      : HttpWebSocketServerConnection{stream, protocols} {}

  void ProcessRequest() override { ++gotRequest; }
  void ProcessWsUpgrade() override { ++gotUpgrade; }

  int gotRequest = 0;
  int gotUpgrade = 0;
};

}  // namespace wpi
