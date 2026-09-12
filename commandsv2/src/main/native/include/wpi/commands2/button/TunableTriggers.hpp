// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/event/EventLoop.hpp"

namespace wpi::cmd {

namespace detail {

/**
 * A type that exposes the non-consuming tunable revision token.
 */
template <typename T>
concept TuneRevisionSource =
    requires(const std::remove_reference_t<T>& tunable) {
      { tunable.GetTuneRevision() } -> std::convertible_to<uint64_t>;
    };

}  // namespace detail

/**
 * Static Trigger factories for reacting to tunable changes.
 *
 * Each call to Tuned() creates an independent observer of the supplied tunable.
 * The returned trigger reports whether that observer detected a tuning revision
 * change during the current event-loop poll. It initially reports false,
 * including when the tunable was tuned before observation began. Multiple
 * backend-applied tunes between polls coalesce into one true poll, and local
 * writes do not activate the trigger.
 *
 * Use Trigger::IfTrue() to react to every poll containing a tune.
 * Trigger::OnTrue() keeps ordinary rising-edge semantics and merges consecutive
 * true polls.
 *
 * <pre>{@code
 * wpi::cmd::TunableTriggers::Tuned(kP).IfTrue(&applyGainsCommand);
 *
 * (wpi::cmd::TunableTriggers::Tuned(kP) ||
 *  wpi::cmd::TunableTriggers::Tuned(kI) ||
 *  wpi::cmd::TunableTriggers::Tuned(kD))
 *     .IfTrue(&applyGainsCommand);
 * }</pre>
 *
 * Triggers created with an explicit EventLoop are only updated when that loop
 * is polled by its owner. Command scheduling still follows CommandScheduler
 * rules.
 */
class TunableTriggers {
 public:
  TunableTriggers() = delete;

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * The trigger is polled by CommandScheduler::GetDefaultButtonLoop().
   *
   * The returned trigger observes the supplied object directly, so the tunable
   * must outlive the returned trigger and any bindings made from it, and its
   * address must remain stable. Temporary tunables are rejected to avoid
   * dangling references.
   *
   * @param tunable the tunable to observe; the returned trigger does not copy
   * it
   * @return a trigger that is true for polls where this observer detected a new
   * tuning revision
   */
  template <detail::TuneRevisionSource Tunable>
  static Trigger Tuned(const Tunable& tunable) {
    return Tuned(CommandScheduler::GetInstance().GetDefaultButtonLoop(),
                 tunable);
  }

  /**
   * Creates a trigger for detecting backend-applied tunes to a tunable.
   *
   * The supplied event loop must be polled by its owner. The returned trigger
   * observes the supplied object directly, so the tunable must outlive the
   * returned trigger and any bindings made from it, and its address must remain
   * stable. Temporary tunables are rejected to avoid dangling references.
   *
   * @param loop the event loop that polls this trigger
   * @param tunable the tunable to observe; the returned trigger does not copy
   * it
   * @return a trigger that is true for polls where this observer detected a new
   * tuning revision
   */
  template <detail::TuneRevisionSource Tunable>
  static Trigger Tuned(wpi::EventLoop* loop, const Tunable& tunable) {
    wpi::BooleanEvent event{
        loop, [&tunable, lastSeen = tunable.GetTuneRevision()]() mutable {
          uint64_t current = tunable.GetTuneRevision();
          bool tuned = current != lastSeen;
          lastSeen = current;
          return tuned;
        }};
    return event.CastTo<Trigger>();
  }

  /**
   * Temporaries are rejected because the returned trigger observes the supplied
   * tunable by address.
   */
  template <detail::TuneRevisionSource Tunable>
  static Trigger Tuned(const Tunable&& tunable) = delete;

  /**
   * Temporaries are rejected because the returned trigger observes the supplied
   * tunable by address.
   */
  template <detail::TuneRevisionSource Tunable>
  static Trigger Tuned(wpi::EventLoop* loop, const Tunable&& tunable) = delete;
};

}  // namespace wpi::cmd
