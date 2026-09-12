// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <exception>
#include <optional>
#include <string_view>

#include "aos/configuration.h"
#include "aos/events/shm_event_loop.h"
#include "aos/init.h"
#include "wpi/aosnt/NtBridge.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/print.hpp"

// The system server's NetworkTables instance, which MrcCommDaemon and MrcLib
// connect to. The channels this bridge publishes are the ones NetworkTables
// carried there before they moved onto AOS.
#define ROBOT_SYSTEM_SERVER_NT_PORT 6810

int main(int argc, char** argv) {
  std::string_view config_path = "aos_config.json";
  std::string_view server = "localhost";
  unsigned int port = ROBOT_SYSTEM_SERVER_NT_PORT;
  std::string_view identity = "aosnt";

  // The only options there are. AOS's flags are not forwarded.
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg != "--config" && arg != "--server" && arg != "--port" &&
        arg != "--identity") {
      wpi::util::print(stderr,
                       "unknown option {}\n"
                       "usage: {} [--config PATH] [--server HOST] [--port N] "
                       "[--identity NAME]\n",
                       arg, argv[0]);
      return 1;
    }
    if (i + 1 >= argc) {
      wpi::util::print(stderr, "{} needs a value\n", arg);
      return 1;
    }
    std::string_view value{argv[++i]};
    if (arg == "--config") {
      config_path = value;
    } else if (arg == "--server") {
      server = value;
    } else if (arg == "--identity") {
      identity = value;
    } else {
      std::optional<unsigned int> parsed =
          wpi::util::parse_integer<unsigned int>(value, 10);
      if (!parsed || *parsed == 0 || *parsed > 65535) {
        wpi::util::print(stderr, "--port needs a number from 1 to 65535\n");
        return 1;
      }
      port = *parsed;
    }
  }

  // AOS requires this before anything creates an event loop.
  aos::InitGoogle(nullptr, nullptr,
                  aos::InitOptions{.parse_command_line = false});

  aos::FlatbufferDetachedBuffer<aos::Configuration> config =
      aos::configuration::ReadConfig(config_path);
  aos::ShmEventLoop event_loop(&config.message());

  wpi::nt::NetworkTableInstance instance =
      wpi::nt::NetworkTableInstance::GetDefault();
  instance.SetServer(server, port);
  instance.StartClient(identity);

  try {
    wpi::aosnt::NtBridge bridge{&event_loop, instance};
    event_loop.Run();
  } catch (const std::exception& e) {
    wpi::util::print(stderr, "{}\n", e.what());
    return 1;
  }

  instance.StopClient();
  return 0;
}
