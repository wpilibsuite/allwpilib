// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <string>

#include "net/TimeSyncStructs.h"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Buffer.hpp"
#include "wpi/net/uv/Timer.hpp"
#include "wpi/net/uv/Udp.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/static_circular_buffer.hpp"

namespace wpi::tsp {

template <size_t MaxBuffer>
class TimeMedianFilter {
 public:
  /**
   * Adds a measurement and returns the window median, rounding half integers
   * away from zero.
   *
   * @param measurement Time offset in nanoseconds.
   * @return Filtered time offset in nanoseconds.
   */
  int64_t Calculate(int64_t measurement) {
    m_buffer.push_back(measurement);

    size_t n = m_buffer.size();
    // N is never 0, but be defensive
    if (n == 0) {
      return 0;
    }

    // Is there no better way to copy out of m_buffer?
    std::copy(m_buffer.begin(), std::next(m_buffer.begin(), n),
              m_sorted.begin());
    // And sort
    std::sort(m_sorted.begin(), m_sorted.begin() + n);

    // Midpoint index. If odd, this is to the right of the midpoint
    size_t mid = n / 2;

    if (n % 2 == 1) {
      // Odd number of total elements -- index directly in
      return m_sorted[mid];
    } else {
      // midpoint avoids overflow and rounds toward the lower value.
      auto lower = m_sorted[mid - 1];
      auto upper = m_sorted[mid];
      auto median = std::midpoint(lower, upper);
      if (median >= 0 && (lower % 2 == 0) != (upper % 2 == 0)) {
        ++median;
      }
      return median;
    }
  }

 private:
  wpi::util::static_circular_buffer<int64_t, MaxBuffer> m_buffer;
  std::array<int64_t, MaxBuffer> m_sorted;
};

class TimeSyncClient {
 public:
  struct Metadata {
    /** Offset added to local timestamps to get server time, in nanoseconds. */
    int64_t offset{0};
    /** Full round-trip time in nanoseconds. */
    int64_t rtt2{0};
    size_t pingsSent{0};
    size_t pongsReceived{0};
    /** Local receive timestamp of the last pong, in nanoseconds. */
    uint64_t lastPongTime{0};
  };

  TimeSyncClient(wpi::util::Logger& logger, std::string_view server,
                 unsigned int remote_port, std::chrono::milliseconds ping_delay,
                 std::function<void(Metadata)> callback);
  /** Stops the UDP loop before destroying callback state. */
  ~TimeSyncClient();

  /**
   * Gets the current filtered time offset.
   *
   * @return Offset in nanoseconds, added to local time to get server time.
   */
  int64_t GetOffset();
  /**
   * Gets a snapshot of the time-sync statistics.
   *
   * @return Metadata with all time values expressed in nanoseconds.
   */
  Metadata GetMetadata();

  /**
   * Updates the estimate from a ping/pong pair. Invalid timestamp ranges are
   * ignored. Must be called on the client's UDP loop, or while it is idle.
   *
   * @param pong_local_time Local receive time in nanoseconds.
   * @param ping Sent ping.
   * @param pong Received pong.
   */
  void UpdateStatistics(uint64_t pong_local_time, wpi::tsp::TspPing ping,
                        wpi::tsp::TspPong pong);

 private:
  using SharedUdpPtr = std::shared_ptr<wpi::net::uv::Udp>;
  using SharedTimerPtr = std::shared_ptr<wpi::net::uv::Timer>;

  std::function<uint64_t()> m_timeProvider;
  wpi::util::Logger& m_logger;

  std::string m_serverIP;
  unsigned int m_serverPort;

  std::chrono::milliseconds m_loopDelay;

  std::function<void(Metadata)> m_callback;

  std::mutex m_metadataMutex;
  Metadata m_metadata;

  // We only allow the most recent ping to stay alive, so only keep track of it
  std::optional<TspPing> m_lastPing;

  // 30s is a reasonable guess
  TimeMedianFilter<30> m_lastOffsets{};

  wpi::net::EventLoopRunner m_loopRunner;
  SharedUdpPtr m_udp;
  SharedTimerPtr m_pingTimer;

  void Tick();

  void UdpCallback(wpi::net::uv::Buffer& buf, size_t nbytes,
                   const sockaddr& sender, unsigned flags);
};

}  // namespace wpi::tsp
