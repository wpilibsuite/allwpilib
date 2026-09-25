// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothLEPacketClient.hpp"

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Timer.hpp"

using namespace wpi::net;

TEST_CASE("Bluetooth status callbacks discard stale snapshots", "[bluetooth]") {
  auto loop = uv::Loop::Create();
  std::vector<std::string> errors;
  bool callbacksOnLoop = true;
  std::shared_ptr<BluetoothLEPacketClient> client;
  client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) {
        callbacksOnLoop &= loop->GetThreadId() == std::this_thread::get_id();
        // Status callbacks must also permit querying the current status.
        client->GetStatus();
        errors.emplace_back(status.error);
      });
  REQUIRE(client);

  bool firstAccepted = true;
  bool secondAccepted = true;
  std::string expectedError;
  auto timer = uv::Timer::Create(loop);
  REQUIRE(timer);
  timer->timeout.connect([&] {
    // Queue an error from a caller thread while the loop is busy.
    std::thread worker{[&] { firstAccepted = client->Connect({}); }};
    worker.join();

    // This error is delivered inline on the loop before the queued error.
    // Neither invalid configuration attempts a Bluetooth connection.
    BluetoothLEPacketClientConfig config;
    config.address = "AA:BB:CC:DD:EE:FF";
    secondAccepted = client->Connect(config);
    expectedError = client->GetStatus().error;
    timer->Close();
  });
  timer->Start(uv::Timer::Time{0});
  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }

  client.reset();
  loop->Run(uv::Loop::Mode::NO_WAIT);
  loop->Walk([](uv::Handle& handle) {
    if (!handle.IsClosing()) {
      handle.Close();
    }
  });
  loop->Run();

  CHECK_FALSE(firstAccepted);
  CHECK_FALSE(secondAccepted);
  CHECK(callbacksOnLoop);
  REQUIRE(errors.size() == 1);
  CHECK_FALSE(expectedError.empty());
  CHECK(errors[0] == expectedError);
}
