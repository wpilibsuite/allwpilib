// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/XRPConnectionStatus.hpp"

#include <chrono>
#include <cstdint>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace wpilibxrp;
using namespace std::chrono_literals;

TEST_CASE("XRP latency samples survive a pause in GUI snapshots",
          "[xrp][latency]") {
  XRPConnectionStatus status;
  auto start = std::chrono::steady_clock::time_point{100s};
  status.RecordLatencySample(start, 0, 4.0, 1.0);
  auto beforeDrag = status;

  // Five seconds of echoes arrive with no GUI frames or status snapshots.
  for (uint16_t seq = 1; seq <= 250; ++seq) {
    status.RecordLatencySample(start + seq * 20ms, seq, 4.0 + seq, 1.0 + seq);
  }
  auto afterDrag = status;
  REQUIRE(afterDrag.latencySamples.size() == 251);
  for (size_t i = 0; i < afterDrag.latencySamples.size(); ++i) {
    const auto& sample = afterDrag.latencySamples[i];
    CHECK(sample.time == Catch::Approx(100.0 + i * 0.02));
    CHECK(sample.roundTripLatencyMs == 4.0 + i);
    CHECK(sample.xrpControlRxAgeMs == 1.0 + i);
  }
  CHECK(afterDrag.latencyAvailable);
  CHECK(afterDrag.latencyControlSeq == 250);
  CHECK(afterDrag.roundTripLatencyMs == 254.0);
  CHECK(afterDrag.xrpControlRxAgeMs == 251.0);
  REQUIRE(beforeDrag.latencySamples.size() == 1);
  CHECK(beforeDrag.latencySamples.front().time == 100.0);
  CHECK(beforeDrag.roundTripLatencyMs == 4.0);
}

TEST_CASE("XRP latency retains only the latest ten seconds", "[xrp][latency]") {
  XRPConnectionStatus status;
  auto start = std::chrono::steady_clock::time_point{100s};
  for (uint16_t seq = 0; seq <= 1000; ++seq) {
    status.RecordLatencySample(start + seq * 20ms, seq, 4.0, 1.0);
  }
  REQUIRE(status.latencySamples.size() == 501);
  CHECK(status.latencySamples.front().time == 110.0);
  CHECK(status.latencySamples.back().time == 120.0);

  // After a long gap, none of the earlier points belongs to the visible window.
  status.RecordLatencySample(start + 31s, 1001, 5.0, 2.0);
  REQUIRE(status.latencySamples.size() == 1);
  CHECK(status.latencySamples.front().time == 131.0);
}

TEST_CASE("XRP latency history has a fixed sample limit", "[xrp][latency]") {
  XRPConnectionStatus status;
  auto start = std::chrono::steady_clock::time_point{100s};
  for (uint16_t seq = 0; seq < 2000; ++seq) {
    status.RecordLatencySample(start + seq * 1ms, seq, seq, 1.0);
  }
  REQUIRE(status.latencySamples.size() == LATENCY_MAX_SAMPLES);
  CHECK(status.latencySamples.front().roundTripLatencyMs == 500.0);
  CHECK(status.latencySamples.back().roundTripLatencyMs == 1999.0);
}

TEST_CASE("XRP latency history follows the connection lifetime",
          "[xrp][latency]") {
  XRPConnectionStatus status;
  XRPConnectionStatus::Base transport;
  transport.connected = true;
  transport.targetAddress = "first";
  status = transport;
  auto start = std::chrono::steady_clock::time_point{100s};
  status.RecordLatencySample(start, 42, 4.0, 1.0);

  SECTION("Connected transport updates preserve samples") {
    transport.packetsReceived = 7;
    status = transport;
    CHECK(status.packetsReceived == 7);
    CHECK(status.latencyAvailable);
    REQUIRE(status.latencySamples.size() == 1);
    CHECK(status.latencySamples.front().time == 100.0);
  }

  SECTION("Disconnect and reconnect between GUI frames resets samples") {
    transport.connected = false;
    status = transport;
    CHECK_FALSE(status.latencyAvailable);
    CHECK(status.latencySamples.empty());
    transport.connected = true;
    status = transport;
    CHECK_FALSE(status.latencyAvailable);
    CHECK(status.latencySamples.empty());
    status.RecordLatencySample(start + 1s, 42, 5.0, 2.0);
    REQUIRE(status.latencySamples.size() == 1);
    CHECK(status.latencySamples.front().time == 101.0);
  }

  SECTION("A different target resets samples") {
    transport.targetAddress = "second";
    status = transport;
    CHECK_FALSE(status.latencyAvailable);
    CHECK(status.latencySamples.empty());
  }
}
