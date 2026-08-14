// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Main.h"

#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "WebServerClientTest.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/simulation/DIOData.h"
#include "wpi/halsim/ws_server/HALSimWSServer.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/util/print.hpp"

namespace uv = wpi::net::uv;

using namespace wpilibws;

static const int POLLING_SPEED = 10;  // 10 ms polling

struct WebServerIntegrationTest {
  WebServerIntegrationTest() {
    // Initialize server
    m_server.Initialize();

    // Create and initialize client
    m_server.runner.ExecSync([=, this](auto& loop) {
      m_webserverClient = std::make_shared<WebServerClientTest>(loop);
      m_webserverClient->Initialize();
    });
  }

  bool IsConnectedClientWS() { return m_webserverClient->IsConnectedWS(); }

  std::shared_ptr<WebServerClientTest> m_webserverClient;
  HALSimWSServer m_server;
};

namespace {

bool IsCatchListCommand(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == "--list-tests" || arg == "--list-tags" ||
        arg == "--list-reporters" || arg == "--list-listeners") {
      return true;
    }
  }
  return false;
}

}  // namespace

TEST_CASE("WebServerIntegrationTest DigitalOutput",
          "[simulation][halsim_ws_server][.]") {
  WebServerIntegrationTest test;

  // Create expected results
  const bool EXPECTED_VALUE = false;
  const int PIN = 0;
  bool done = false;

  // Attach timer to loop for test function
  test.m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::net::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (test.IsConnectedClientWS()) {
        wpi::util::print("***** Setting DIO value for pin {} to {}\n", PIN,
                         (EXPECTED_VALUE ? "true" : "false"));
        HALSIM_SetDIOValue(PIN, EXPECTED_VALUE);
        done = true;
      }
    });
    timer->Start(uv::Timer::Time(POLLING_SPEED),
                 uv::Timer::Time(POLLING_SPEED));
    timer->Unreference();
  });

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // Get values from JSON message
  std::string test_type;
  std::string test_device;
  bool test_value = true;  // Default value from HAL initialization
  try {
    auto& msg = test.m_webserverClient->GetLastMessage();
    test_type = msg.at("type").get_string();
    test_device = msg.at("device").get_string();
    auto& data = msg.at("data");
    if (auto val = data.lookup("<>value"); val && val->is_bool()) {
      test_value = val->get_bool();
    }
  } catch (std::logic_error& e) {
    wpi::util::print(stderr, "Error with incoming message: {}\n", e.what());
  }

  // Compare results
  CHECK("DIO" == test_type);
  CHECK(std::to_string(PIN) == test_device);
  CHECK(EXPECTED_VALUE == test_value);
}

TEST_CASE("WebServerIntegrationTest DigitalInput",
          "[simulation][halsim_ws_server][.]") {
  WebServerIntegrationTest test;

  // Create expected results
  const bool EXPECTED_VALUE = false;
  const int PIN = 0;
  bool done = false;

  // Attach timer to loop for test function
  test.m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::net::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (test.IsConnectedClientWS()) {
        wpi::util::json msg = wpi::util::json::object();
        msg["type"] = "DIO";
        msg["device"] = std::to_string(PIN);
        msg["data"] = wpi::util::json::object("<>value", EXPECTED_VALUE);
        wpi::util::print("***** Input JSON: {}\n", msg.to_string());
        test.m_webserverClient->SendMessage(msg);
        done = true;
      }
    });
    timer->Start(uv::Timer::Time(POLLING_SPEED),
                 uv::Timer::Time(POLLING_SPEED));
    timer->Unreference();
  });

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  // Compare results
  bool test_value = HALSIM_GetDIOValue(PIN);
  CHECK(EXPECTED_VALUE == test_value);
}

TEST_CASE("WebServerIntegrationTest DriverStation",
          "[simulation][halsim_ws_server]") {
  WebServerIntegrationTest test;

  // Create expected results
  const bool EXPECTED_VALUE = true;
  bool done = false;

  // Attach timer to loop for test function
  test.m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::net::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (test.IsConnectedClientWS()) {
        auto data = wpi::util::json::object();
        data[">enabled"] = EXPECTED_VALUE;
        data[">new_data"] = true;
        wpi::util::json msg = wpi::util::json::object();
        msg["type"] = "DriverStation";
        msg["device"] = "";
        msg["data"] = std::move(data);
        wpi::util::print("***** Input JSON: {}\n", msg.to_string());
        test.m_webserverClient->SendMessage(msg);
        done = true;
      }
    });
    timer->Start(uv::Timer::Time(POLLING_SPEED),
                 uv::Timer::Time(POLLING_SPEED));
    timer->Unreference();
  });

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);

  HAL_RefreshDSData();

  // Compare results
  HAL_ControlWord cw;
  HAL_GetControlWord(&cw);
  bool test_value = HAL_ControlWord_IsEnabled(cw);
  CHECK(EXPECTED_VALUE == test_value);
}

int main(int argc, char* argv[]) {
  if (!IsCatchListCommand(argc, argv)) {
    HAL_Initialize();
  }
  return Catch::Session().run(argc, argv);
}
