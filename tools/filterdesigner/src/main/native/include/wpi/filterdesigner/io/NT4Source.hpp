// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

/**
 * Core state for a live NetworkTables data source. Holds a ring buffer of
 * recent samples trimmed to a fixed window of seconds, and rebuilds a
 * pointer-stable @ref Signal from the buffer each @ref Update call.
 *
 * The real NT subscription is owned by @ref NT4SourceNode. Taking the queue
 * drain as a callable keeps this class pure and testable without ntcore.
 */
class NT4Source {
 public:
  /** A single timestamped sample. Time base matches NT (nanoseconds). */
  struct Sample {
    int64_t timeNanos;
    double value;
  };

  /** Returns samples observed since the last call, in chronological order. */
  using DrainFn = std::function<std::vector<Sample>()>;

  explicit NT4Source(DrainFn drain);

  /**
   * Pull new samples from the drain callable, append to the ring buffer,
   * drop anything older than @ref BufferSeconds, and rebuild the Signal.
   * Intended to be called once per ImGui frame. When @ref Frozen is true,
   * drained samples are discarded rather than buffered, so unfreezing
   * resumes from real-time instead of replaying backlog.
   */
  void Update();

  /**
   * Signal view of the ring buffer. Pointer remains stable for the lifetime
   * of the NT4Source; the vectors inside are rewritten by @ref Update.
   */
  const Signal* GetSignal() const { return &m_signal; }

  /**
   * Longest window @ref SetBufferSeconds will store, matching the top of the
   * node's slider. @ref Update scales the window to nanoseconds, so an
   * unbounded value from a saved graph would overflow the int64_t cutoff.
   */
  static constexpr double kMaxBufferSeconds = 120.0;

  double BufferSeconds() const { return m_bufferSeconds; }

  /**
   * Sets the length of the sliding window, in seconds. A value that is not
   * finite and positive is ignored, leaving the window unchanged; one above
   * @ref kMaxBufferSeconds is clamped to it.
   */
  void SetBufferSeconds(double seconds);

  bool Frozen() const { return m_frozen; }
  void SetFrozen(bool frozen) { m_frozen = frozen; }

  /** Drops all buffered samples. Leaves freeze state and window unchanged. */
  void Clear();

  size_t SampleCount() const { return m_buffer.size(); }

  const std::string& Name() const { return m_signal.name; }
  void SetName(std::string_view name) { m_signal.name = name; }

  /**
   * Marks the subscribed topic as taking only discrete values, so the grid
   * holds them rather than interpolating between them. Set from the topic
   * type when subscribing; survives @ref Clear, which only drops samples.
   * A change rebuilds the Signal from the buffer, since the grid already
   * built reflects the old setting and nothing else would redo it while the
   * topic stays quiet.
   */
  bool Discrete() const { return m_signal.discrete; }
  void SetDiscrete(bool discrete);

 private:
  /** Regrids the Signal from the buffer and bumps its revision. */
  void Rebuild();

  DrainFn m_drain;
  std::deque<Sample> m_buffer;
  Signal m_signal;
  double m_bufferSeconds = 30.0;
  bool m_frozen = false;
  // Timestamp of the first buffered sample, subtracted from every sample so
  // the timeline starts at 0 rather than at wall clock. Reset by Clear().
  std::int64_t m_t0Nanos = 0;
  bool m_haveT0 = false;
};

}  // namespace wpi::filterdesigner
