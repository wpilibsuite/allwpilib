// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <vector>

#include "wpi/tunable/detail/TunableTypeValue.hpp"

namespace wpi {

struct TunableConfig;

namespace detail {
class TunableBase;
}  // namespace detail

class TunableBackend {
 public:
  struct PublishedTunable {
    std::string path;
    uint32_t uid;
  };

  virtual ~TunableBackend() = default;

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param uid the tunable uid
   * @param tunable the tunable
   * @param config the tunable config
   * @param type the tunable type
   */
  virtual void Publish(std::string_view path, uint32_t uid,
                       detail::TunableBase& tunable,
                       const TunableConfig* config,
                       detail::TunableTypeValue type) = 0;

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  virtual void Remove(std::string_view path) = 0;

  /**
   * Removes all tunables under a prefix.
   *
   * @param prefix normalized path prefix
   * @return removed tunables
   */
  virtual std::vector<PublishedTunable> RemovePrefix(
      std::string_view prefix) = 0;

  /**
   * Unregisters a tunable.
   *
   * @param uid tunable uid
   */
  virtual void UnregisterTunable(uint32_t uid) = 0;

  /** Updates all tunable values and calls callbacks where appropriate. */
  virtual void Update() = 0;
};

}  // namespace wpi
