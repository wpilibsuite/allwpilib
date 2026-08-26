// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "wpi/tunables/detail/TunableTypeValue.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::tunables {

class ComplexTunable;
class TunableBackend;
struct TunableConfig;

namespace detail {
class TunableBase;
class TunableMemberBase;
}  // namespace detail

class TunableRegistry final {
  friend class ComplexTunable;

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
   * The reporting function must not throw.
   *
   * @param func reporting function; pass null to use default
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
   * @return tunable backend, or a no-op backend if no match
   */
  static std::shared_ptr<TunableBackend> GetBackend(std::string_view path);

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  static bool Publish(std::string_view path, detail::TunableBase& tunable);

  /**
   * Publishes a complex tunable.
   *
   * @param path normalized path
   * @param tunable the complex tunable
   * @return true if the backend accepted the tunable
   */
  static bool Publish(std::string_view path, ComplexTunable& tunable);

  /**
   * Publishes a tunable for a member variable of a complex tunable.
   *
   * @param path normalized path
   * @param tunable the complex tunable object
   * @param member the member tunable for the member variable
   * @return true if the backend accepted the tunable
   */
  static bool Publish(std::string_view path, ComplexTunable* tunable,
                      std::unique_ptr<detail::TunableMemberBase> member);

  /**
   * Removes a tunable and any descendant tunables.
   * @param path normalized path
   */
  static void Remove(std::string_view path);

  /** Updates all tunable values from backends. */
  static void Update();

  /**
   * Notifies registered backends that a tunable has become dirty.
   *
   * @param uid tunable uid
   */
  static void NotifyChanged(uint32_t uid);

  /**
   * Resets a tunable's changed flag after the current update cycle finishes.
   *
   * Backends should call this after publishing a changed tunable so every alias
   * in every backend can observe the shared changed flag before it is cleared.
   *
   * @param uid tunable uid
   */
  static void ResetChangedAfterUpdate(uint32_t uid);

  /**
   * Runs a callback after the current update cycle finishes resetting changed
   * flags.
   *
   * Backends should use this for callbacks that react to tuned values so
   * callback mutations are observed as new changes in the next update cycle.
   *
   * @param callback callback to run; must not throw
   */
  static void RunAfterUpdate(std::function<void()> callback);

  /**
   * Get the mutex used during Update() execution.
   *
   * As tunables are not thread-safe, this is designed to allow external
   * synchronization for tunables used in secondary threads outside of the robot
   * main loop.
   *
   * Every competing access, including main-loop access, must use this mutex or
   * another safe handoff. The mutex is held while backends and user callbacks
   * run, so critical sections should be short. References returned by a
   * tunable must not be used after releasing the mutex if another thread can
   * access the value.
   *
   * Note the returned mutex is recursive, so registry operations invoked from
   * an update callback on the same thread can re-enter it. It still does not
   * make tunable objects themselves thread-safe.
   *
   * @return mutex
   */
  static wpi::util::recursive_mutex& GetUpdateMutex();

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
   * Moves config parent pointers from one ComplexTunable object to another.
   * Should generally only be used by ComplexTunable.
   *
   * @param oldParent old parent object
   * @param newParent new parent object
   */
  static void MoveTunableCallbackParent(ComplexTunable* oldParent,
                                        ComplexTunable* newParent);

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

 private:
  /**
   * Publishes a tunable and reports whether the backend accepted it.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  static bool PublishImpl(std::string_view path, detail::TunableBase& tunable);

  /**
   * Prepares a complex tunable for move assignment.
   *
   * If both source and destination are registered, the destination's current
   * registration is unregistered so base move assignment transfers the source
   * registration instead of retaining the destination registration.
   *
   * @param destination destination tunable
   * @param source source tunable
   */
  static void PrepareComplexMoveAssignment(detail::TunableBase& destination,
                                           detail::TunableBase& source);

  /**
   * Publishes a child tunable under every currently published path for a
   * complex tunable. Should generally only be used by ComplexTunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   * @param tunable the child tunable
   */
  static void PublishChild(ComplexTunable& parent, std::string_view name,
                           detail::TunableBase& tunable);

  /**
   * Publishes a child complex tunable under every currently published path for
   * a complex tunable. Should generally only be used by ComplexTunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   * @param tunable the child complex tunable
   */
  static void PublishChild(ComplexTunable& parent, std::string_view name,
                           ComplexTunable& tunable);

  /**
   * Removes a child tunable from every currently published path for a complex
   * tunable. Should generally only be used by ComplexTunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   */
  static void RemoveChild(ComplexTunable& parent, std::string_view name);

  /**
   * Marks a child tunable as locally changed. Should generally only be used by
   * ComplexTunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   */
  static void SetChildChanged(ComplexTunable& parent, std::string_view name);
};

namespace detail {

/**
 * Sets a callback that runs at the start of TunableRegistry::Update().
 *
 * <p>This is intended for language bindings that own tunable value state
 * outside the native registry and need to copy that state into native tunables
 * before backends publish values.
 *
 * @param callback callback to run before each update; pass null to clear
 */
void SetTunableRegistryPreUpdateCallback(std::function<void()> callback);

}  // namespace detail

}  // namespace wpi::tunables
