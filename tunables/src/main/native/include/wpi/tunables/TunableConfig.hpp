// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <optional>
#include <string>

#include "wpi/util/json.hpp"

namespace wpi::tunables {

class ComplexTunable;

namespace detail {
class TunableBase;
}  // namespace detail

/** Additional optional configuration for tunables. */
struct TunableConfig {
  /** Defines when the backend should poll the tunable's local value. */
  enum class Polling {
    /** Use the framework default polling behavior. */
    DEFAULT,

    /** Poll the tunable's local value on every loop. */
    ALWAYS_GET,

    /** Poll the tunable's local value only when it is marked changed. */
    GET_ON_CHANGE
  };

  /**
   * Creates a config where the tunable gets its value only when it is changed.
   *
   * @return TunableConfig
   */
  static TunableConfig GetOnChange() {
    TunableConfig config;
    config.polling = Polling::GET_ON_CHANGE;
    return config;
  }

  /**
   * Creates a config where the tunable always gets its value on every loop.
   *
   * @return TunableConfig
   */
  static TunableConfig AlwaysGet() {
    TunableConfig config;
    config.polling = Polling::ALWAYS_GET;
    return config;
  }

  /** Additional properties for the tunable. */
  wpi::util::json properties = wpi::util::json::object();

  /**
   * Indicates the tunable should separately echo back its new value after being
   * set.
   */
  bool robust = false;

  /** The type string for the tunable. */
  std::optional<std::string> typeString{};

  /**
   * Indicates whether the tunable is mutable. Setting this to
   * false makes the tunable immutable, meaning that the Tunable's Set()
   * function will not be called. This is useful for tunables that should only
   * be changed locally, such as ones that represent a constant or a tunable
   * that is only meant to be changed programmatically.
   */
  bool isMutable = true;

  /**
   * Callback to call when the tunable is tuned. This is called immediately
   * after the tunable's Tune() function is called. The callback will be called
   * by TunableRegistry::Update().
   *
   * Note that it is not safe to this-capture the tunable itself in the
   * callback, as the tunable may be moved. Instead, use the callback parameter
   * to access the tunable.
   *
   * Similarly, if the tunable is a member of a ComplexTunable, the
   * ComplexTunable this pointer should not be captured by the
   * callback--instead, set the parent value and it will be move-tracked and
   * passed to the callback via the parent parameter.
   *
   * This callback must not throw. If an exception escapes, registry and
   * backend state is not guaranteed to be restored.
   */
  std::function<void(detail::TunableBase& tunable, ComplexTunable* parent)>
      onTune{};

  /**
   * Callback to call after a backend applies a remote value, but before the
   * backend publishes the resulting value. This is intended for language
   * bindings that need to reconcile getter/setter-backed values before a robust
   * backend echoes the tune. User code should prefer onTune. This callback
   * must not throw.
   */
  std::function<void(detail::TunableBase& tunable, ComplexTunable* parent)>
      onRemoteSet{};

  /**
   * Parent object to pass to callback functions. Note since this is
   * move-tracked by the framework, the pointer passed to a callback
   * might not be the same pointer provided here.
   */
  ComplexTunable* parent = nullptr;

  /**
   * Defines when the backend should poll the tunable's local value.
   */
  Polling polling = Polling::DEFAULT;
};

}  // namespace wpi::tunables
