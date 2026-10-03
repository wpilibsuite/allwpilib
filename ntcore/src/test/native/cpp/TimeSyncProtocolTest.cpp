// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <net/TimeSyncClient.h>
#include <net/TimeSyncServer.h>

#include <limits>
#include <print>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "TimeSyncTestPeer.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

class TimeSyncProtoTest {
 protected:
  wpi::util::Logger logger;
};

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest Smoketest",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  wpi::util::Logger msglog;

  auto startTimeUs = wpi::nt::Now() / 1000;
  TimeSyncServer server{logger, "", 5812};
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(100ms);
    TimeSyncClient::Metadata m = client.GetMetadata();
    std::println("Offset={} rtt={}", m.offset, m.rtt2);
  }

  auto metadata = client.GetMetadata();
  REQUIRE(metadata.pongsReceived > 1);
  CHECK(metadata.pingsSent >= metadata.pongsReceived);
  CHECK(metadata.lastPongTime >= static_cast<uint64_t>(startTimeUs));
  CHECK(metadata.lastPongTime <= static_cast<uint64_t>(wpi::nt::Now() / 1000));
  // Both clocks are local, so the server offset should be near zero.
  CHECK(std::abs(metadata.offset) < 1'000'000);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest CalculateZero",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  // GIVEN a fresh client
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  // AND a ping-pong sent with no delay
  // client -> server -> client
  uint64_t ping_client_time{100};
  uint64_t pong_server_time{100};
  uint64_t pong_client_time{100};

  // setup our ping/pong packets
  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_client_time};
  TspPong pong{ping, pong_server_time};

  // WHEN we update statistics
  client.UpdateStatistics(pong_client_time, ping, pong);

  // THEN the statistics will reflect no delay
  CHECK(0 == client.GetMetadata().offset);
  CHECK(0 == client.GetMetadata().rtt2);
  CHECK(1u == client.GetMetadata().pongsReceived);
  CHECK(pong_client_time == client.GetMetadata().lastPongTime);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest CalculateZeroOffset",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  // GIVEN a fresh client
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  // AND a ping-pong sent with 10ms delay each way
  // client -> server -> client
  uint64_t ping_client_time{100};
  uint64_t pong_server_time{110};
  uint64_t pong_client_time{120};

  // setup our ping/pong packets
  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_client_time};
  TspPong pong{ping, pong_server_time};

  // WHEN we update statistics
  client.UpdateStatistics(pong_client_time, ping, pong);

  // THEN the statistics will reflect no offset, and the expected rtt2
  // (client-to-client) latency
  CHECK(0 == client.GetMetadata().offset);
  CHECK(20 == client.GetMetadata().rtt2);
  CHECK(1u == client.GetMetadata().pongsReceived);
  CHECK(pong_client_time == client.GetMetadata().lastPongTime);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest CalculateZeroRtt",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  // GIVEN a fresh client
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  // AND a ping-pong sent with no delay
  // client -> server -> client
  uint64_t ping_client_time{100};
  uint64_t pong_server_time{123};
  uint64_t pong_client_time{100};

  // setup our ping/pong packets
  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_client_time};
  TspPong pong{ping, pong_server_time};

  // WHEN we update statistics
  client.UpdateStatistics(pong_client_time, ping, pong);

  // THEN the statistics will reflect the expected 23ms offset
  CHECK(23 == client.GetMetadata().offset);
  CHECK(0 == client.GetMetadata().rtt2);
  CHECK(1u == client.GetMetadata().pongsReceived);
  CHECK(pong_client_time == client.GetMetadata().lastPongTime);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest CalculateBoth",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  // GIVEN a fresh client
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  // AND a ping-pong sent with no delay
  // client -> server -> client
  int64_t offset{-234};
  int64_t network_latency{23};

  uint64_t ping_client_time{1000};
  uint64_t pong_server_time{ping_client_time + offset + network_latency};
  uint64_t pong_client_time{ping_client_time + 2 * network_latency};

  // setup our ping/pong packets
  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_client_time};
  TspPong pong{ping, pong_server_time};

  // WHEN we update statistics
  client.UpdateStatistics(pong_client_time, ping, pong);

  // THEN the statistics will reflect the expected latency and RTT
  CHECK(offset == client.GetMetadata().offset);
  CHECK(network_latency * 2 == client.GetMetadata().rtt2);
  CHECK(1u == client.GetMetadata().pongsReceived);
  CHECK(pong_client_time == client.GetMetadata().lastPongTime);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest FilterMedian",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;

  TimeMedianFilter<8> filter;

  // push 1, 2, 4, 4, 6, 99
  CHECK(1 == filter.Calculate(1));
  CHECK(2 == filter.Calculate(2));
  CHECK(2 == filter.Calculate(4));
  CHECK(3 == filter.Calculate(4));
  CHECK(4 == filter.Calculate(6));
  CHECK(4 == filter.Calculate(99));
  // push 2. new state will be
  // 1, 2, 2, 4, 4, 6, 99
  CHECK(4 == filter.Calculate(2));
}

TEST_CASE_METHOD(TimeSyncProtoTest,
                 "TimeSyncProtoTest FilterMedianAlternatingValues",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  TimeMedianFilter<4> filter;
  filter.Calculate(1);
  filter.Calculate(1000);
  filter.Calculate(1);
  filter.Calculate(1000);
  // buffer state: [1, 1, 1000, 1000], median = 500
  CHECK(501 == filter.Calculate(1));
}

TEST_CASE_METHOD(TimeSyncProtoTest,
                 "TimeSyncProtoTest FilterMedianNegativeValues",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  TimeMedianFilter<4> filter;
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-5 == filter.Calculate(0));   // average of [-10, 0]
  CHECK(-3 == filter.Calculate(-3));  // sorted: [-10, -3, 0]
}

TEST_CASE_METHOD(TimeSyncProtoTest,
                 "TimeSyncProtoTest FilterWindowRollsOverOdd",
                 "[ntcore][time-sync-protocol]") {
  // Odd window size, so never need to average
  using namespace wpi::tsp;
  TimeMedianFilter<3> filter;
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-10 == filter.Calculate(-10));
  // buffer state: [-10, -10, 12]
  CHECK(-10 == filter.Calculate(12));
  CHECK(12 == filter.Calculate(12));
  CHECK(12 == filter.Calculate(12));
}

TEST_CASE_METHOD(TimeSyncProtoTest,
                 "TimeSyncProtoTest FilterWindowRollsOverEven",
                 "[ntcore][time-sync-protocol]") {
  // Even window size, so need to average
  using namespace wpi::tsp;
  TimeMedianFilter<4> filter;
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-10 == filter.Calculate(-10));
  CHECK(-10 == filter.Calculate(-10));
  // buffer state: [-10, -10, -10, 13]
  CHECK(-10 == filter.Calculate(13));
  // buffer state: [-10, -10, 13, 13]
  // 1.5 should round to 2.0
  CHECK(2 == filter.Calculate(13));
  // 12.5 round to 13
  CHECK(13 == filter.Calculate(12));
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest RejectMalformedPings",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;
  TimeSyncServer server{logger, "127.0.0.1", 5813};
  TimeSyncTestPeer peer;
  std::array<uint8_t, 11> bytes{};
  wpi::util::PackStruct(bytes, TspPing{1, 1, 123});
  for (size_t size : {2u, 9u, 11u}) {
    REQUIRE(peer.Send(std::span{bytes}.first(size), 5813) == static_cast<int>(size));
  }
  wpi::util::PackStruct(bytes, TspPing{1, 1, 456});
  REQUIRE(peer.Send(std::span{bytes}.first(10), 5813) == 10);
  auto packet = peer.Receive(2s);
  REQUIRE(packet);
  REQUIRE(packet->data.size() == wpi::util::Struct<TspPong>::GetSize());
  auto pong = wpi::util::UnpackStruct<TspPong>(packet->data);
  CHECK(pong.client_time == 456);
  CHECK(pong.message_id == 2);
  CHECK_FALSE(peer.Receive(100ms));
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest AccumulatesMetadata",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;
  TimeSyncClient* instance = nullptr;
  size_t callbacks = 0;
  TimeSyncClient client{logger, "127.0.0.1", 5812, 1h,
                        [&](TimeSyncClient::Metadata metadata) {
                          ++callbacks;
                          CHECK(metadata.pongsReceived == callbacks);
                          CHECK(instance->GetMetadata().pongsReceived == callbacks);
                        }};
  instance = &client;
  TspPing ping{1, 1, 100};
  TspPong pong{ping, 110};
  client.UpdateStatistics(120, ping, pong);
  client.UpdateStatistics(140, ping, pong);
  CHECK(client.GetMetadata().pongsReceived == 2u);
  CHECK(client.GetMetadata().lastPongTime == 140u);
  CHECK(callbacks == 2u);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest FilterIntegerLimits",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  constexpr auto MAX = std::numeric_limits<int64_t>::max();
  constexpr auto MIN = std::numeric_limits<int64_t>::min();
  TimeMedianFilter<2> filter;
  CHECK(filter.Calculate(MAX) == MAX);
  CHECK(filter.Calculate(MAX) == MAX);
  CHECK(filter.Calculate(MIN) == -1);
  CHECK(filter.Calculate(MIN) == MIN);
  CHECK(filter.Calculate(MIN + 1) == MIN);
  CHECK(filter.Calculate(MAX) == 0);

  constexpr int64_t LARGE = (int64_t{1} << 53) + 1;
  TimeMedianFilter<2> precise;
  CHECK(precise.Calculate(LARGE) == LARGE);
  CHECK(precise.Calculate(LARGE + 2) == LARGE + 1);
  CHECK(precise.Calculate(-LARGE - 3) == -1);
}

TEST_CASE_METHOD(TimeSyncProtoTest, "TimeSyncProtoTest RejectInvalidTimestamps",
                 "[ntcore][time-sync-protocol]") {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;
  size_t callbacks = 0;
  TimeSyncClient client{logger, "127.0.0.1", 5812, 1h,
                        [&](auto) { ++callbacks; }};
  TspPing ping{1, 1, 100};
  TspPong pong{ping, 110};
  client.UpdateStatistics(99, ping, pong);
  client.UpdateStatistics(UINT64_MAX, ping, pong);
  client.UpdateStatistics(120, ping, TspPong{ping, UINT64_MAX});
  CHECK(callbacks == 0u);
  CHECK(client.GetMetadata().pongsReceived == 0u);
  client.UpdateStatistics(120, ping, pong);
  CHECK(callbacks == 1u);
  CHECK(client.GetMetadata().offset == 0);
  CHECK(client.GetMetadata().rtt2 == 20);
}
