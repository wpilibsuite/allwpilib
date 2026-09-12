// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <exception>
#include <optional>
#include <string_view>
#include <vector>

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

  // Take our options out of argv. Everything else is AOS's, and InitGoogle
  // rejects flags it does not know.
  std::vector<char*> aos_args{argv[0]};
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg != "--config" && arg != "--server" && arg != "--port" &&
        arg != "--identity") {
      aos_args.push_back(argv[i]);
      continue;
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
      if (!parsed) {
        wpi::util::print(stderr, "--port needs an integer\n");
        return 1;
      }
      port = *parsed;
    }
  }

  int aos_argc = static_cast<int>(aos_args.size());
  char** aos_argv = aos_args.data();
  aos::InitGoogle(&aos_argc, &aos_argv);
  if (aos_argc > 1) {
    wpi::util::print(stderr,
                     "usage: {} [--config PATH] [--server HOST] [--port N] "
                     "[--identity NAME]\n",
                     argv[0]);
    return 1;
  }

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
