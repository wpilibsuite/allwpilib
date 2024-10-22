// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/StringExtras.h>
#include <wpi/fmt/raw_ostream.h>
#include <wpi/json.h>
#include <wpi/print.h>
#include <wpi/raw_ostream.h>

#include "cameraserver/CameraServer.h"

/*
   JSON format:
   {
       "team": <team number>,
       "ntmode": <"client" or "server", "client" if unspecified>
       "cameras": [
           {
               "name": <camera name>
               "path": <path, e.g. "/dev/video0">
               "pixel format": <"MJPEG", "YUYV", etc>   // optional
               "width": <video mode width>              // optional
               "height": <video mode height>            // optional
               "fps": <video mode fps>                  // optional
               "brightness": <percentage brightness>    // optional
               "white balance": <"auto", "hold", value> // optional
               "exposure": <"auto", "hold", value>      // optional
               "properties": [                          // optional
                   {
                       "name": <property name>
                       "value": <property value>
                   }
               ]
           }
       ]
   }
 */

#ifdef __RASPBIAN__
static const char* configFile = "/boot/frc.json";
#else
static const char* configFile = "frc.json";
#endif

namespace {

unsigned int team;
bool server = false;

struct CameraConfig {
  std::string name;
  std::string path;
  wpi::json config;
};

std::vector<CameraConfig> cameras;

bool ReadCameraConfig(const wpi::json& config) {
  CameraConfig c;

  // name
  try {
    c.name = config.at("name").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "config error in '{}': could not read camera name: {}\n",
               configFile, e.what());
    return false;
  }

  // path
  try {
    c.path = config.at("path").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr,
               "config error in '{}': camera '{}': could not read path: {}\n",
               configFile, c.name, e.what());
    return false;
  }

  c.config = config;

  cameras.emplace_back(std::move(c));
  return true;
}

bool ReadConfig() {
  // open config file
  auto fileBuffer = wpi::MemoryBuffer::GetFile(configFile);
  if (!fileBuffer) {
    wpi::print(stderr, "could not open '{}': {}\n", configFile,
               fileBuffer.error().message());
    return false;
  }

  // parse file
  wpi::json j;
  try {
    j = wpi::json::parse(fileBuffer.value()->GetCharBuffer());
  } catch (const wpi::json::parse_error& e) {
    wpi::print(stderr, "config error in '{}': byte {}: {}\n", configFile,
               e.byte, e.what());
    return false;
  }

  // top level must be an object
  if (!j.is_object()) {
    wpi::print(stderr, "config error in '{}': must be JSON object\n",
               configFile);
    return false;
  }

  // team number
  try {
    team = j.at("team").get<unsigned int>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "config error in '{}': could not read team number: {}\n",
               configFile, e.what());
    return false;
  }

  // ntmode (optional)
  if (j.count("ntmode") != 0) {
    try {
      auto str = j.at("ntmode").get<std::string>();
      if (wpi::equals_lower(str, "client")) {
        server = false;
      } else if (wpi::equals_lower(str, "server")) {
        server = true;
      } else {
        wpi::print(
            stderr,
            "config error in '{}': could not understand ntmode value '{}'\n",
            configFile, str);
      }
    } catch (const wpi::json::exception& e) {
      wpi::print(stderr, "config error in '{}': could not read ntmode: {}\n",
                 configFile, e.what());
    }
  }

  // cameras
  try {
    for (auto&& camera : j.at("cameras")) {
      if (!ReadCameraConfig(camera)) {
        return false;
      }
    }
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "config error in '{}': could not read cameras: {}\n",
               configFile, e.what());
    return false;
  }

  return true;
}

void StartCamera(const CameraConfig& config) {
  wpi::print("Starting camera '{}' on {}\n", config.name, config.path);
  auto camera =
      frc::CameraServer::StartAutomaticCapture(config.name, config.path);

  camera.SetConfigJson(config.config);
}
}  // namespace

int main(int argc, char* argv[]) {
  if (argc >= 2) {
    configFile = argv[1];
  }

  // read configuration
  if (!ReadConfig()) {
    return EXIT_FAILURE;
  }

  // start NetworkTables
  auto ntinst = nt::NetworkTableInstance::GetDefault();
  if (server) {
    std::puts("Setting up NetworkTables server");
    ntinst.StartServer();
  } else {
    wpi::print("Setting up NetworkTables client for team {}\n", team);
    ntinst.StartClient4("multicameraserver");
    ntinst.SetServerTeam(team);
  }

  // start cameras
  for (auto&& camera : cameras) {
    StartCamera(camera);
  }

  // loop forever
  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}
