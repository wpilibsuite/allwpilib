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
  /** A single timestamped sample. Time base matches NT (microseconds). */
  struct Sample {
    int64_t timeMicros;
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

  double BufferSeconds() const { return m_bufferSeconds; }
  void SetBufferSeconds(double seconds);

  bool Frozen() const { return m_frozen; }
  void SetFrozen(bool frozen) { m_frozen = frozen; }

  /** Drops all buffered samples. Leaves freeze state and window unchanged. */
  void Clear();

  size_t SampleCount() const { return m_buffer.size(); }

  const std::string& Name() const { return m_signal.name; }
  void SetName(std::string_view name) { m_signal.name = name; }

 private:
  DrainFn m_drain;
  std::deque<Sample> m_buffer;
  Signal m_signal;
  double m_bufferSeconds = 30.0;
  bool m_frozen = false;
  // Reference timestamp captured from the first buffered sample. Subtracted
  // from every sample so the displayed timeline starts at 0 instead of at
  // wall-clock microseconds. Reset by Clear() so re-subscribing to a topic
  // restarts the timeline from 0. Sentinel: m_haveT0 == false → unset.
  std::int64_t m_t0Micros = 0;
  bool m_haveT0 = false;
};

}  // namespace wpi::filterdesigner
