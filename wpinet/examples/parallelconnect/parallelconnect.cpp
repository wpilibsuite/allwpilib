// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/ParallelTcpConnector.hpp"
#include "wpi/net/uv/Error.hpp"
#include "wpi/net/uv/Tcp.hpp"
#include "wpi/util/Logger.hpp"

namespace uv = wpi::net::uv;

static void logfunc(unsigned int level, const char* file, unsigned int line,
                    const char* msg) {
  std::fprintf(stderr, "(%d) %s:%d: %s\n", level, file, line, msg);
}

int main() {
  wpi::util::Logger logger{logfunc, 0};

  // Kick off the event loop on a separate thread
  wpi::net::EventLoopRunner loop;
  std::shared_ptr<wpi::net::ParallelTcpConnector> connect;
  loop.ExecAsync([&](uv::Loop& loop) {
    connect = wpi::net::ParallelTcpConnector::Create(
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
