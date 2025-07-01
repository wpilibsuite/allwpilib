// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <optional>
#include <string>

#include "wpi/util/json.hpp"

namespace wpi {

class ComplexTunable;

namespace detail {
class TunableBase;
}  // namespace detail

/** Additional optional configuration for tunables. */
struct TunableConfig {
  /** Additional properties for the tunable. */
  wpi::util::json properties = wpi::util::json::object();

  /**
   * Indicates the tunable should separately echo back its value after setting.
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
   */
  std::function<void(detail::TunableBase& tunable, ComplexTunable* parent)>
      onTune{};

  /**
   * Parent object to pass to onTune callback.  Note since this is
   * move-tracked by the framework, the pointer passed to the onTune callback
   * might not the same pointer provided here.
   */
  ComplexTunable* parent = nullptr;

  /**
   * Indicates whether the tunable should always get its value on every loop.
   * Normally, the backend will only call Get() if the value has changed (set
   * by Set() getting called).
   */
  bool alwaysGet = false;
};

}  // namespace wpi
