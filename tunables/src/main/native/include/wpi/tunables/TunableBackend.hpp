// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <vector>

#include "wpi/tunables/detail/TunableTypeValue.hpp"

namespace wpi::tunables {

struct TunableConfig;

namespace detail {
class TunableBase;
}  // namespace detail

/** Backend interface for publishing and updating tunables. */
class TunableBackend {
 public:
  /**
   * A tunable removed from a backend.
   *
   * @param path normalized path
   * @param uid tunable uid
   */
  struct PublishedTunable {
    std::string path;
    uint32_t uid;
  };

  /** Destroys this tunable backend. */
  virtual ~TunableBackend() = default;

  /**
   * Retires this backend after it is removed from TunableRegistry.
   *
   * This stops external backend effects while the backend object may still be
   * alive. Destructors remain the final RAII cleanup path and may call this
   * method.
   *
   * Implementations must not throw from this method.
   */
  virtual void Retire() {}

  /**
   * Publishes a tunable.
   *
   * Backend implementations must not throw from this method. Recoverable
   * publication failures should be reported through
   * TunableRegistry::ReportWarning() and skipped.
   *
   * @param path normalized path
   * @param uid the tunable uid
   * @param tunable the tunable
   * @param config the tunable config
   * @param type the tunable type
   * @return true if the tunable was published, false otherwise
   */
  virtual bool Publish(std::string_view path, uint32_t uid,
                       detail::TunableBase& tunable,
                       const TunableConfig* config,
                       detail::TunableTypeValue type) = 0;

  /**
   * Marks a published tunable dirty after its local value changed.
   *
   * Backend implementations must not throw from this method.
   *
   * @param uid tunable uid
   */
  virtual void MarkDirty(uint32_t uid) = 0;

  /**
   * Removes a tunable.
   *
   * Backend implementations must not throw from this method.
   *
   * @param path normalized path
   */
  virtual void Remove(std::string_view path) = 0;

  /**
   * Removes all tunables under a prefix.
   *
   * Backend implementations must not throw from this method. The returned list
   * must describe the tunables actually removed from the backend.
   *
   * @param prefix normalized path prefix
   * @return removed tunables
   */
  virtual std::vector<PublishedTunable> RemovePrefix(
      std::string_view prefix) = 0;

  /**
   * Unregisters a tunable.
   *
   * Backend implementations must not throw from this method.
   *
   * @param uid tunable uid
   */
  virtual void UnregisterTunable(uint32_t uid) = 0;

  /**
   * Updates all tunable values and calls callbacks where appropriate.
   *
   * Backend implementations must not throw from this method. Recoverable update
   * failures should be reported through TunableRegistry::ReportWarning() and
   * skipped so the robot loop and registry state can continue updating.
   */
  virtual void Update() = 0;
};

}  // namespace wpi::tunables
