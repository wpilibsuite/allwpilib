// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "wpi/tunable/detail/TunableTypeValue.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi {

class ComplexTunable;
class TunableBackend;
struct TunableConfig;

namespace detail {
class TunableBase;
class TunableMemberBase;
}  // namespace detail

class TunableRegistry final {
 public:
  TunableRegistry() = delete;

  struct TunableInfo {
    detail::TunableBase* tunable;
    const TunableConfig* config;
    detail::TunableTypeValue type;

    explicit operator bool() const { return tunable != nullptr; }

    bool IsChanged() const;
    void ResetChanged();
  };

  /**
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function; parameters are path and message; pass null
   * to use default
   */
  static void SetReportWarning(std::function<void(std::string_view)> func);

  /**
   * Get function used for reporting warning messages.
   *
   * @return reporting function
   */
  static std::function<void(std::string_view)> GetReportWarning();

  /**
   * Report a warning message (e.g. type mismatch).
   *
   * @param msg warning message
   */
  static void ReportWarning(std::string_view msg);

  /**
   * Registers a backend for creating tunables. When calling GetBackend(), the
   * longest prefix match is used.
   *
   * @param prefix prefix for tunables covered by this backend
   * @param backend backend
   */
  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<TunableBackend> backend);

  /**
   * Gets the backend for creating a tunable. Should generally only be used
   * internally or by custom backends.
   *
   * @param path full name
   * @return tunable backend, or nullptr if no match
   */
  static std::shared_ptr<TunableBackend> GetBackend(std::string_view path);

  /**
   * Normalizes a tunable name.
   *
   * @param path input path
   * @return normalized path
   */
  static std::string_view NormalizeName(std::string_view path,
                                        std::string& buf);

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  static void Publish(std::string_view path, detail::TunableBase& tunable);

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param path normalized path
   * @param tunable the complex tunable object
   * @param member the member tunable for the member variable
   */
  static void Publish(std::string_view path, ComplexTunable* tunable,
                      std::unique_ptr<detail::TunableMemberBase> member);

  /**
   * Removes a tunable.
   * @param path normalized path
   */
  static void Remove(std::string_view path);

  /** Updates all tunable values from backends. */
  static void Update();

  /**
   * Get the mutex used during Update() execution.
   *
   * As tunables are not thread-safe, this is designed to allow external
   * synchronization for tunables used in secondary threads outside of the robot
   * main loop.
   *
   * Note the returned mutex is not recursive, so calling other TunableRegistry
   * functions or creating/removing/moving a tunable while holding the mutex may
   * deadlock.
   *
   * @return mutex
   */
  static wpi::util::mutex& GetUpdateMutex();

  /**
   * Clear all registered backends. Should typically only be used by unit test
   * code.
   */
  static void Reset();

  /**
   * Gets tunable and its config and type by uid.
   *
   * @param uid tunable uid
   * @return tuple of tunable, config, and type value, or nullptr tunable if not
   * found
   */
  static TunableInfo GetTunable(uint32_t uid);

  /**
   * Registers tunable and its config. Should generally only be used by
   * TunableBase.
   *
   * @param tunable tunable
   * @param config tunable config
   * @param type tunable type
   * @return tunable uid
   */
  static uint32_t RegisterTunable(detail::TunableBase* tunable,
                                  const TunableConfig* config,
                                  detail::TunableTypeValue type);

  /**
   * Unregisters tunable. Should generally only be used by TunableBase.
   *
   * @param uid tunable uid
   */
  static void UnregisterTunable(uint32_t uid);

  /**
   * Moves tunable to a new pointer. Should generally only be used by
   * TunableBase.
   *
   * @param uid tunable uid
   * @param tunable new tunable pointer
   */
  static void MoveTunable(uint32_t uid, detail::TunableBase* tunable);
};

}  // namespace wpi
