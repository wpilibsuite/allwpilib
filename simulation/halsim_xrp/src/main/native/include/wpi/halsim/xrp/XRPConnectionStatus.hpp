// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "wpi/net/BluetoothLEPacketClient.hpp"

namespace wpilibxrp {

constexpr double LATENCY_HISTORY_SECONDS = 10.0;
constexpr size_t LATENCY_MAX_SAMPLES = 1500;

struct XRPLatencySample {
  // Seconds since the steady_clock epoch, measured when the echo is received.
  double time;
  double roundTripLatencyMs;
  double xrpControlRxAgeMs;
};

struct XRPConnectionStatus
    : public wpi::net::BluetoothLEPacketConnectionStatus {
  using Base = wpi::net::BluetoothLEPacketConnectionStatus;

  XRPConnectionStatus() = default;

  XRPConnectionStatus& operator=(const Base& status) {
    if (!status.connected || targetAddress != status.targetAddress) {
      latencyAvailable = false;
      latencySamples.clear();
    }
    static_cast<Base&>(*this) = status;
    return *this;
  }

  // Called on the communications thread while holding HALSimXRP's status mutex.
  // Keeping the history here lets GUI snapshots recover all recent samples
  // even when rendering pauses (e.g. during a Windows window drag).
  void RecordLatencySample(std::chrono::steady_clock::time_point received,
                           uint16_t controlSeq, double roundTripMs,
                           double controlRxAgeMs) {
    latencyAvailable = true;
    latencyControlSeq = controlSeq;
    roundTripLatencyMs = roundTripMs;
    xrpControlRxAgeMs = controlRxAgeMs;
    double now =
        std::chrono::duration<double>(received.time_since_epoch()).count();
    latencySamples.push_back({now, roundTripMs, controlRxAgeMs});

    auto firstVisible =
        std::lower_bound(latencySamples.begin(), latencySamples.end(),
                         now - LATENCY_HISTORY_SECONDS,
                         [](const XRPLatencySample& sample, double time) {
                           return sample.time < time;
                         });
    auto eraseCount =
        static_cast<size_t>(firstVisible - latencySamples.begin());
    if (latencySamples.size() - eraseCount > LATENCY_MAX_SAMPLES) {
      eraseCount = latencySamples.size() - LATENCY_MAX_SAMPLES;
    }
    latencySamples.erase(latencySamples.begin(),
                         latencySamples.begin() + eraseCount);
  }

  bool latencyAvailable = false;
  std::string targetName;
  uint16_t latencyControlSeq = 0;
  double roundTripLatencyMs = 0.0;
  double xrpControlRxAgeMs = 0.0;
  std::vector<XRPLatencySample> latencySamples;
};

}  // namespace wpilibxrp
