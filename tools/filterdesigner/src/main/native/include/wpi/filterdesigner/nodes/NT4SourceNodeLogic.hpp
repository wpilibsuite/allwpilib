// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/nt/ntcore_c.h"

namespace wpi::filterdesigner {

/**
 * Pure (UI-free, ntcore-free) state of an @ref NT4SourceNode: server
 * settings, the currently subscribed topic name, ring-buffer config, and the
 * @ref NT4Source whose @ref Signal pointer the node's output pin exposes.
 *
 * @ref NT4SourceNode owns the subscription itself; this class only consumes
 * samples already drained out of NetworkTables, so a stub @ref SetDrain drives
 * it without ntcore.
 */
class NT4SourceNodeLogic {
 public:
  enum class ServerMode { Team, Host };

  NT4SourceNodeLogic();

  NT4SourceNodeLogic(const NT4SourceNodeLogic&) = delete;
  NT4SourceNodeLogic& operator=(const NT4SourceNodeLogic&) = delete;

  /** Installs the function that yields fresh samples each frame. */
  void SetDrain(NT4Source::DrainFn fn);

  /**
   * Pulls new samples, trims the ring buffer and rebuilds the cached Signal.
   * Called once per frame; a no-op with no drain set.
   */
  void Update();

  /**
   * The cached Signal once at least one sample is buffered, else nullptr. The
   * pointer is stable for the logic's lifetime; @ref Update rewrites what it
   * points at.
   */
  const wpi::filterdesigner::Signal* Signal() const;

  /** Drops all buffered samples. Leaves server / topic settings unchanged. */
  void Clear();

  // --- Server settings (saved into .fdsgn v2) ---------------------------

  ServerMode serverMode = ServerMode::Host;
  std::string host = "127.0.0.1";
  int team = 0;
  int port = static_cast<int>(NT_DEFAULT_PORT);

  /** Currently subscribed topic name, empty when nothing is subscribed. */
  const std::string& TopicName() const { return m_topicName; }

  /**
   * Sets the topic name, and with it the produced Signal's name. Leaves the
   * ring buffer alone; the node calls @ref Clear when it re-subscribes.
   */
  void SetTopicName(std::string_view name);

  // --- Buffer config (saved) — delegates to NT4Source so the two views
  // can't drift out of sync. ---------------------------------------------

  double BufferSeconds() const { return m_source.BufferSeconds(); }
  void SetBufferSeconds(double seconds) { m_source.SetBufferSeconds(seconds); }

  /**
   * Marks the topic as discrete, so the grid holds its values rather than
   * interpolating. @ref Clear leaves it alone: it describes the topic, not the
   * samples.
   */
  bool Discrete() const { return m_source.Discrete(); }
  void SetDiscrete(bool discrete) { m_source.SetDiscrete(discrete); }

  bool Frozen() const { return m_source.Frozen(); }
  void SetFrozen(bool frozen) { m_source.SetFrozen(frozen); }

  std::size_t SampleCount() const { return m_source.SampleCount(); }

  /** Direct access to the underlying buffer — used by tests + the UI. */
  const NT4Source& Source() const { return m_source; }

  // --- Input sanitization shared between UI + deserialize paths ---------

  /**
   * Clamps @p t to the team numbers ntcore can turn into a 10.TE.AM.2
   * address, 0 through 25599. ntcore quietly adds no team address for a
   * larger value, and the node would sit at "connecting..." for good.
   */
  static int SanitizeTeam(int t) {
    constexpr int kMaxTeam = 25599;
    return std::clamp(t, 0, kMaxTeam);
  }
  /**
   * Clamps @p p to a valid TCP port: non-positive falls back to the ntcore
   * default, anything past 65535 caps there. ntcore itself casts to unsigned
   * and would accept nonsense.
   */
  static int SanitizePort(int p) {
    if (p < 1) {
      return static_cast<int>(NT_DEFAULT_PORT);
    }
    constexpr int kMaxPort = 65535;
    return p > kMaxPort ? kMaxPort : p;
  }

 private:
  // Drain held separately so the Node can swap it in at construction (or
  // tests can supply a stub). The NT4Source itself wraps a stable trampoline
  // that delegates to whatever m_drain currently holds.
  NT4Source::DrainFn m_drain;
  NT4Source m_source;
  std::string m_topicName;
};

}  // namespace wpi::filterdesigner
