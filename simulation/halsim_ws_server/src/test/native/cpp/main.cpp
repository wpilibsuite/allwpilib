// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>
#include <thread>

#include <gtest/gtest.h>
#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>
#include <hal/simulation/DIOData.h>
#include <wpi/print.h>
#include <wpinet/uv/Loop.h>

#include "HALSimWSServer.h"
#include "WebServerClientTest.h"

namespace uv = wpi::uv;

using namespace wpilibws;

static const int POLLING_SPEED = 10;  // 10 ms polling

class WebServerIntegrationTest : public ::testing::Test {
 public:
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

 protected:
  std::shared_ptr<WebServerClientTest> m_webserverClient;
  HALSimWSServer m_server;
};

TEST_F(WebServerIntegrationTest, DISABLED_DigitalOutput) {
  // Create expected results
  const bool EXPECTED_VALUE = false;
  const int PIN = 0;
  bool done = false;

  // Attach timer to loop for test function
  m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (IsConnectedClientWS()) {
        wpi::print("***** Setting DIO value for pin {} to {}\n", PIN,
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
    auto& msg = m_webserverClient->GetLastMessage();
    test_type = msg.at("type").get_ref<const std::string&>();
    test_device = msg.at("device").get_ref<const std::string&>();
    auto& data = msg.at("data");
    wpi::json::const_iterator it = data.find("<>value");
    if (it != data.end()) {
      test_value = it.value();
    }
  } catch (wpi::json::exception& e) {
    wpi::print(stderr, "Error with incoming message: {}\n", e.what());
  }

  // Compare results
  EXPECT_EQ("DIO", test_type);
  EXPECT_EQ(std::to_string(PIN), test_device);
  EXPECT_EQ(EXPECTED_VALUE, test_value);
}

TEST_F(WebServerIntegrationTest, DISABLED_DigitalInput) {
  // Create expected results
  const bool EXPECTED_VALUE = false;
  const int PIN = 0;
  bool done = false;

  // Attach timer to loop for test function
  m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (IsConnectedClientWS()) {
        wpi::json msg = {{"type", "DIO"},
                         {"device", std::to_string(PIN)},
                         {"data", {{"<>value", EXPECTED_VALUE}}}};
        wpi::print("***** Input JSON: {}\n", msg.dump());
        m_webserverClient->SendMessage(msg);
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
  EXPECT_EQ(EXPECTED_VALUE, test_value);
}

TEST_F(WebServerIntegrationTest, DriverStation) {
  // Create expected results
  const bool EXPECTED_VALUE = true;
  bool done = false;

  // Attach timer to loop for test function
  m_server.runner.ExecSync([&](auto& loop) {
    auto timer = wpi::uv::Timer::Create(loop);
    timer->timeout.connect([&] {
      if (done) {
        return;
      }
      if (IsConnectedClientWS()) {
        wpi::json msg = {
            {"type", "DriverStation"},
            {"device", ""},
            {"data", {{">enabled", EXPECTED_VALUE}, {">new_data", true}}}};
        wpi::print("***** Input JSON: {}\n", msg.dump());
        m_webserverClient->SendMessage(msg);
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
  bool test_value = cw.enabled;
  EXPECT_EQ(EXPECTED_VALUE, test_value);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  HAL_Initialize(500, 0);
  int ret = RUN_ALL_TESTS();
  return ret;
}
