/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>
#include <hal/simulation/DIOData.h>
#include <wpi/raw_ostream.h>
#include <wpi/uv/Loop.h>

#include "HALSimWSServer.h"
#include "WebServerClientTest.h"
#include "gtest/gtest.h"

namespace uv = wpi::uv;

using namespace wpilibws;

static const int POLLING_SPEED = 10;  // 10 ms polling

class WebServerIntegrationTest : public ::testing::Test {
 public:
  WebServerIntegrationTest() {
    // Initialize server
    m_server.Initialize();

    // Create and initialize client
    m_server.runner.ExecSync([=](auto& loop) {
      m_webserverClient = std::make_shared<WebServerClientTest>(loop);
      m_webserverClient->Initialize();
    });
  }

  bool IsConnectedClientWS() { return m_webserverClient->IsConnectedWS(); }

 protected:
  std::shared_ptr<WebServerClientTest> m_webserverClient;
  HALSimWSServer m_server;
};

TEST_F(WebServerIntegrationTest, DigitalOutput) {
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
        wpi::outs() << "***** Setting DIO value for pin " << PIN << " to "
                    << (EXPECTED_VALUE ? "true" : "false") << "\n";
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
    wpi::errs() << "Error with incoming message: " << e.what() << "\n";
  }

  // Compare results
  EXPECT_EQ("DIO", test_type);
  EXPECT_EQ(std::to_string(PIN), test_device);
  EXPECT_EQ(EXPECTED_VALUE, test_value);
}

TEST_F(WebServerIntegrationTest, DigitalInput) {
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
        wpi::outs() << "***** Input JSON: " << msg.dump() << "\n";
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
        wpi::outs() << "***** Input JSON: " << msg.dump() << "\n";
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
