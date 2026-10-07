// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothLEPacketClient.hpp"

#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Timer.hpp"

using namespace wpi::net;

TEST_CASE("Bluetooth teardown after loop destruction", "[bluetooth]") {
  auto loop = uv::Loop::Create();
  REQUIRE(loop);
  int callbacks = 0;
  auto client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {}, [&](const auto&) { ++callbacks; });
  REQUIRE(client);

  loop->SetClosing();
  loop->Walk([](uv::Handle& handle) { handle.Close(); });
  loop->Run();
  loop.reset();

  // Explicit disconnect and destruction can both outlive the loop. Neither
  // may publish a callback or access the destroyed loop.
  client->Disconnect();
  client.reset();
  CHECK(callbacks == 0);
}

TEST_CASE("Bluetooth creation on a closing loop", "[bluetooth]") {
  auto loop = uv::Loop::Create();
  REQUIRE(loop);
  loop->SetClosing();
  int callbacks = 0;
  auto client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {}, [&](const auto&) { ++callbacks; });
  CHECK_FALSE(client);
  CHECK(callbacks == 0);
}

TEST_CASE("Bluetooth destruction releases loop handles", "[bluetooth]") {
  auto loop = uv::Loop::Create();
  REQUIRE(loop);
  auto client = BluetoothLEPacketClient::Create(*loop, [](auto) {});
  REQUIRE(client);
  std::shared_ptr<uv::Timer> timer;

  SECTION("Before running the loop") {
    client.reset();
  }
  SECTION("From another thread") {
    std::thread worker{
        [client = std::move(client)]() mutable { client.reset(); }};
    worker.join();
  }
  SECTION("From a loop callback") {
    timer = uv::Timer::Create(loop);
    REQUIRE(timer);
    timer->timeout.connect([&] {
      client.reset();
      timer->Close();
    });
    timer->Start(uv::Timer::Time{0});
  }

  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }
  CHECK_FALSE(loop->IsAlive());
  int handles = 0;
  loop->Walk([&](auto& handle) {
    ++handles;
    if (!handle.IsClosing()) {
      handle.Close();
    }
  });
  CHECK(handles == 0);
  loop->Run();
}

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

#if defined(_WIN32) || defined(__APPLE__)
TEST_CASE("Bluetooth initial status callback can supersede a connection",
          "[bluetooth]") {
  bool replace = false;
  SECTION("Cancel") {}
  SECTION("Replace and cancel") {
    replace = true;
  }

  auto loop = uv::Loop::Create();
  REQUIRE(loop);
  BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:01";
  config.preferL2CAP = false;
  config.gattServiceUuid = "00000000-0000-0000-0000-000000000001";
  config.gattControlCharacteristicUuid = "00000000-0000-0000-0000-000000000002";
  config.gattStatusCharacteristicUuid = "00000000-0000-0000-0000-000000000003";
  bool firstAccepted = false;
  bool replacementAccepted = false;
  bool canceled = false;
  int connectingCallbacks = 0;
  std::shared_ptr<BluetoothLEPacketClient> client;
  client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) {
        if (status.connecting) {
          ++connectingCallbacks;
          if (replace && connectingCallbacks == 1) {
            auto next = config;
            next.address = "AA:BB:CC:DD:EE:02";
            replacementAccepted = client->Connect(next);
          } else {
            client->Disconnect("Cancelled");
          }
        } else if (status.status == "Cancelled") {
          canceled = true;
          loop->Stop();
        }
      });
  REQUIRE(client);

  auto starter = uv::Timer::Create(loop);
  REQUIRE(starter);
  starter->timeout.connect([&] {
    firstAccepted = client->Connect(config);
    starter->Close();
  });
  starter->Start(uv::Timer::Time{0});
  auto deadline = uv::Timer::Create(loop);
  REQUIRE(deadline);
  deadline->timeout.connect([&] { loop->Stop(); });
  deadline->Start(uv::Timer::Time{1000});
  loop->Run();
  auto status = client->GetStatus();

  client.reset();
  loop->Walk([](auto& handle) {
    if (!handle.IsClosing()) {
      handle.Close();
    }
  });
  loop->Run();

  CHECK(firstAccepted);
  CHECK(replacementAccepted == replace);
  CHECK(connectingCallbacks == (replace ? 2 : 1));
  CHECK(canceled);
  CHECK_FALSE(status.connected);
  CHECK_FALSE(status.connecting);
  CHECK(status.status == "Cancelled");
}
#endif
