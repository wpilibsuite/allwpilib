// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <net/TimeSyncClient.h>
#include <net/TimeSyncServer.h>

#include <gtest/gtest.h>

class TimeSyncProtoTest : public ::testing::Test {
 protected:
  wpi::util::Logger logger;
};

TEST_F(TimeSyncProtoTest, Smoketest) {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  wpi::util::Logger msglog;

  TimeSyncServer server{logger, "", 5812};
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(100ms);
    TimeSyncClient::Metadata m = client.GetMetadata();
    fmt::println("Offset={} rtt={}", m.offset, m.rtt2);
  }
}

TEST_F(TimeSyncProtoTest, CalculateZero) {
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
  EXPECT_EQ(0, client.GetMetadata().offset);
  EXPECT_EQ(0, client.GetMetadata().rtt2);
  EXPECT_EQ(1u, client.GetMetadata().pongsReceived);
  EXPECT_EQ(pong_client_time, client.GetMetadata().lastPongTime);
}

TEST_F(TimeSyncProtoTest, CalculateZeroOffset) {
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
  EXPECT_EQ(0, client.GetMetadata().offset);
  EXPECT_EQ(20, client.GetMetadata().rtt2);
  EXPECT_EQ(1u, client.GetMetadata().pongsReceived);
  EXPECT_EQ(pong_client_time, client.GetMetadata().lastPongTime);
}

TEST_F(TimeSyncProtoTest, CalculateZeroRtt) {
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
  EXPECT_EQ(23, client.GetMetadata().offset);
  EXPECT_EQ(0, client.GetMetadata().rtt2);
  EXPECT_EQ(1u, client.GetMetadata().pongsReceived);
  EXPECT_EQ(pong_client_time, client.GetMetadata().lastPongTime);
}

TEST_F(TimeSyncProtoTest, CalculateBoth) {
  using namespace wpi::tsp;
  using namespace std::chrono_literals;

  // GIVEN a fresh client
  TimeSyncClient client{logger, "127.0.0.1", 5812, 100ms, nullptr};

  // AND a ping-pong sent with no delay
  // client -> server -> client
  int64_t offset{-234};
  int64_t network_latency{23};

  uint64_t ping_client_time{100};
  uint64_t pong_server_time{ping_client_time + offset + network_latency};
  uint64_t pong_client_time{ping_client_time + 2 * network_latency};

  // setup our ping/pong packets
  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_client_time};
  TspPong pong{ping, pong_server_time};

  // WHEN we update statistics
  client.UpdateStatistics(pong_client_time, ping, pong);

  // THEN the statistics will reflect the expected latency and RTT
  EXPECT_EQ(offset, client.GetMetadata().offset);
  EXPECT_EQ(network_latency * 2, client.GetMetadata().rtt2);
  EXPECT_EQ(1u, client.GetMetadata().pongsReceived);
  EXPECT_EQ(pong_client_time, client.GetMetadata().lastPongTime);
}

TEST_F(TimeSyncProtoTest, FilterMedian) {
  using namespace wpi::tsp;

  TimeMedianFilter<8> filter;

  // push 1, 2, 4, 4, 6, 99
  EXPECT_EQ(1, filter.Calculate(1));
  EXPECT_EQ(2, filter.Calculate(2));
  EXPECT_EQ(2, filter.Calculate(4));
  EXPECT_EQ(3, filter.Calculate(4));
  EXPECT_EQ(4, filter.Calculate(6));
  EXPECT_EQ(4, filter.Calculate(99));
  // push 2. new state will be
  // push 2, 4, 4, 6, 99, 2
  EXPECT_EQ(4, filter.Calculate(2));
}
