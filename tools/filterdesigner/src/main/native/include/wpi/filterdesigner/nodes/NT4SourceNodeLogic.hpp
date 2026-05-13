// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
 * The actual NT subscription (instance, listener, subscriber) is owned by
 * @ref NT4SourceNode; this class only knows how to consume samples that were
 * already drained out of NetworkTables. Tests can construct + drive a
 * NT4SourceNodeLogic without spinning up ntcore by supplying a stub drain
 * callable via @ref SetDrain.
 */
class NT4SourceNodeLogic {
 public:
  enum class ServerMode { Team, Host };

  NT4SourceNodeLogic();

  NT4SourceNodeLogic(const NT4SourceNodeLogic&) = delete;
  NT4SourceNodeLogic& operator=(const NT4SourceNodeLogic&) = delete;

  /**
   * Inject the function that yields fresh samples each frame. The Node sets
   * this once in its ctor with a lambda that reads from the NT subscriber.
   * Tests inject one-shot stubs.
   */
  void SetDrain(NT4Source::DrainFn fn);

  /**
   * Pull new samples, append + trim the ring buffer, rebuild the cached
   * Signal. Intended to be called once per ImGui frame. Safe to call with no
   * drain set — does nothing in that case.
   */
  void Update();

  /**
   * Returns the cached Signal pointer when there is at least one sample
   * buffered, otherwise nullptr. Pointer is stable for the logic's lifetime;
   * the vectors inside are rewritten by @ref Update.
   */
  const Signal* Signal() const;

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
   * Update the topic name. Also propagates onto the underlying NT4Source so
   * the produced Signal's @c name field tracks the topic identity. Does not
   * touch the ring buffer — the Node calls Clear() separately when it
   * actually re-subscribes.
   */
  void SetTopicName(std::string_view name);

  // --- Buffer config (saved) — delegates to NT4Source so the two views
  // can't drift out of sync. ---------------------------------------------

  double BufferSeconds() const { return m_source.BufferSeconds(); }
  void SetBufferSeconds(double seconds) { m_source.SetBufferSeconds(seconds); }

  bool Frozen() const { return m_source.Frozen(); }
  void SetFrozen(bool frozen) { m_source.SetFrozen(frozen); }

  std::size_t SampleCount() const { return m_source.SampleCount(); }

  /** Direct access to the underlying buffer — used by tests + the UI. */
  const NT4Source& Source() const { return m_source; }

  // --- Input sanitization shared between UI + deserialize paths ---------

  static int SanitizeTeam(int t) { return t < 0 ? 0 : t; }
  /**
   * Clamps @p p to a valid TCP port. Values <= 0 fall back to the ntcore
   * default port (a hand-edited file with `port: 0` shouldn't poison the
   * connect dialog); values > 65535 are capped to 65535 since ntcore casts
   * to `unsigned int` and would happily accept nonsense otherwise.
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
