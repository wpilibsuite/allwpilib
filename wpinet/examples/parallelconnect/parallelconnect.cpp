// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include <wpi/Logger.h>

#include "wpinet/EventLoopRunner.h"
#include "wpinet/ParallelTcpConnector.h"
#include "wpinet/uv/Error.h"
#include "wpinet/uv/Tcp.h"

namespace uv = wpi::uv;

static void logfunc(unsigned int level, const char* file, unsigned int line,
                    const char* msg) {
  std::fprintf(stderr, "(%d) %s:%d: %s\n", level, file, line, msg);
}

int main() {
  wpi::Logger logger{logfunc, 0};

  // Kick off the event loop on a separate thread
  wpi::EventLoopRunner loop;
  std::shared_ptr<wpi::ParallelTcpConnector> connect;
  loop.ExecAsync([&](uv::Loop& loop) {
    connect = wpi::ParallelTcpConnector::Create(
        loop, uv::Timer::Time{2000}, logger, [&](uv::Tcp& tcp) {
          std::fputs("Got connection, accepting!\n", stdout);
          tcp.StartRead();
          connect->Succeeded(tcp);
          tcp.end.connect([&] {
            std::fputs("TCP connection ended, disconnecting!\n", stdout);
            tcp.Close();
            connect->Disconnected();
          });
          tcp.error.connect([&](uv::Error) {
            std::fputs("TCP error, disconnecting!\n", stdout);
            connect->Disconnected();
          });
        });
    connect->SetServers({{{"roborio-294-frc.local", 8080},
                          {"roborio-294-frc.frc-field.local", 8080},
                          {"10.2.94.2", 8080},
                          {"127.0.0.1", 8080}}});
  });

  // wait for a keypress to terminate
  static_cast<void>(std::getchar());
}
