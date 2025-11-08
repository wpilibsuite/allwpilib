// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include "wpi/net/DsClient.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/print.hpp"

namespace uv = wpi::uv;

static void logfunc(unsigned int level, const char* file, unsigned int line,
                    const char* msg) {
  std::fprintf(stderr, "(%d) %s:%d: %s\n", level, file, line, msg);
}

int main() {
  wpi::Logger logger{logfunc, 0};

  // Kick off the event loop on a separate thread
  wpi::EventLoopRunner loop;
  std::shared_ptr<wpi::DsClient> client;
  loop.ExecAsync([&](uv::Loop& loop) {
    client = wpi::DsClient::Create(loop, logger);
    client->setIp.connect(
        [](std::string_view ip) { wpi::print("got IP: {}\n", ip); });
    client->clearIp.connect([] { std::fputs("cleared IP\n", stdout); });
  });

  // wait for a keypress to terminate
  static_cast<void>(std::getchar());
}
