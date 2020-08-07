/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <hal/HALBase.h>
#include <hal/Main.h>
#include <hal/simulation/DIOData.h>
#include <wpi/raw_ostream.h>
#include <wpi/uv/Loop.h>

#include "HALSimWSServer.h"
#include "WebServerClientTest.h"
#include "gtest/gtest.h"

using namespace wpilibws;

extern "C" int HALSIM_InitExtension(
    void);  // from simulation/halsim_ws_server/src/main/native/cpp/main.cpp

class WebServerIntegrationTest : public ::testing::Test {
 public:
  WebServerIntegrationTest() {
    // Initialize HAL layer including webserver
    HALSIM_InitExtension();

    // Create and initialize client
    m_webserver_client =
        std::shared_ptr<WebServerClientTest>((new WebServerClientTest()));
    WebServerClientTest::SetInstance(m_webserver_client);
    auto webserver = HALSimWeb::GetInstance();
    auto loop = webserver->GetLoop();
    m_webserver_client->Initialize(loop);

    // Create timer for loop run
    m_testTimer = wpi::uv::Timer::Create(loop);
    m_testTimer->timeout.connect([loop] { loop->Stop(); });
    m_testTimer->Start(uv::Timer::Time{4000});  // End test after 4 seconds
  }

  ~WebServerIntegrationTest() {
    // Exit HAL layer which exits webserver
    HAL_ExitMain();
    HALSimWeb::Exit(nullptr);

    // Exit client
    m_webserver_client->Exit();
    WebServerClientTest::SetInstance(nullptr);

    // Unreference test timer from loop
    m_testTimer->Unreference();
  }

 private:
  std::shared_ptr<WebServerClientTest> m_webserver_client;
  std::shared_ptr<wpi::uv::Timer> m_testTimer;
};

TEST_F(WebServerIntegrationTest, DigitalOutput) {
  // Create expected results
  const bool EXPECTED_VALUE = false;
  const int PIN = 0;

  // Get loop, attach timer, run loop
  auto ws = HALSimWeb::GetInstance();
  auto loop = ws->GetLoop();
  auto timer = wpi::uv::Timer::Create(loop);
  timer->timeout.connect([&] {
    wpi::outs() << "***** Setting DIO value for pin " << PIN << " to "
                << (EXPECTED_VALUE ? "true" : "false") << "\n";
    HALSIM_SetDIOValue(PIN, EXPECTED_VALUE);
  });
  timer->Start(uv::Timer::Time(3000));  // Wait 3 second before sending message
  HAL_RunMain();
  timer->Unreference();

  // Get values from JSON message
  std::string test_type;
  std::string test_device;
  bool test_value = true;  // Default value from HAL initialization
  try {
    auto& msg = WebServerClientTest::GetInstance()->GetLastMessage();
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

  // Get loop, attach timer, run loop
  auto ws = HALSimWeb::GetInstance();
  auto loop = ws->GetLoop();
  auto timer = wpi::uv::Timer::Create(loop);
  timer->timeout.connect([&] {
    wpi::json msg = {{"type", "DIO"},
                     {"device", std::to_string(PIN)},
                     {"data", {{"<>value", EXPECTED_VALUE}}}};
    wpi::outs() << "***** Input JSON: " << msg.dump() << "\n";
    WebServerClientTest::GetInstance()->SendMessage(msg);
  });
  timer->Start(uv::Timer::Time(3000));  // Wait 3 second before checking message
  HAL_RunMain();
  timer->Unreference();

  // Compare results
  bool test_value = HALSIM_GetDIOValue(PIN);
  EXPECT_EQ(EXPECTED_VALUE, test_value);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  HAL_Initialize(500, 0);
  int ret = RUN_ALL_TESTS();
  return ret;
}
