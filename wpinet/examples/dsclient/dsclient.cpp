// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <fmt/format.h>
#include <wpi/Logger.h>

#include "wpinet/DsClient.h"
#include "wpinet/EventLoopRunner.h"
#include "wpinet/uv/Error.h"

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
        [](std::string_view ip) { fmt::print("got IP: {}\n", ip); });
    client->clearIp.connect([] { std::fputs("cleared IP\n", stdout); });
  });

  // wait for a keypress to terminate
  std::getchar();
}
