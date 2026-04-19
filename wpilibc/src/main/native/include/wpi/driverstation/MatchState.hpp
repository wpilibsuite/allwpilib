// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>

#include "wpi/driverstation/Alliance.hpp"
#include "wpi/driverstation/MatchType.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"

namespace wpi {

/**
 * Provides access to match state information from the Driver Station.
 */
class MatchState final {
 public:
  MatchState() = delete;

  /**
   * Return the approximate match time. The FMS does not send an official match
   * time to the robots, but does send an approximate match time. The value will
   * count down the time remaining in the current period (auto or teleop).
   * Warning: This is not an official time (so it cannot be used to dispute ref
   * calls or guarantee that a function will trigger before the match ends).
   *
   * When connected to the real field, this number only changes in full integer
   * increments, and always counts down.
   *
   * When the DS is in practice mode, this number is a floating point number,
   * and counts down.
   *
   * When the DS is in teleop or autonomous mode, this number returns -1.0.
   *
   * Simulation matches DS behavior without an FMS connected.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  static wpi::units::second_t GetMatchTime() {
    return wpi::internal::DriverStationBackend::GetMatchTime();
  }

  /**
   * Get the current alliance from the FMS.
   *
   * If the FMS is not connected, it is set from the team alliance setting on
   * the driver station.
   *
   * @return The alliance (red or blue) or an empty optional if the alliance is
   * invalid
   */
  static std::optional<Alliance> GetAlliance() {
    return wpi::internal::DriverStationBackend::GetAlliance();
  }

  /**
   * Return the driver station location from the FMS.
   *
   * If the FMS is not connected, it is set from the team alliance setting on
   * the driver station.
   *
   * This could return 1, 2, or 3.
   *
   * @return The location of the driver station (1-3, 0 for invalid)
   */
  static std::optional<int> GetLocation() {
    return wpi::internal::DriverStationBackend::GetLocation();
  }

  /**
   * Returns the number of times the current match has been replayed from the
   * FMS.
   *
   * @return The number of replays
   */
  static int GetReplayNumber() {
    return wpi::internal::DriverStationBackend::GetReplayNumber();
  }

  /**
   * Returns the match number provided by the FMS.
   *
   * @return The number of the match
   */
  static int GetMatchNumber() {
    return wpi::internal::DriverStationBackend::GetMatchNumber();
  }

  /**
   * Returns the type of match being played provided by the FMS.
   *
   * @return The match type enum (kNone, kPractice, kQualification,
   *         kElimination)
   */
  static MatchType GetMatchType() {
    return wpi::internal::DriverStationBackend::GetMatchType();
  }

  /**
   * Returns the name of the competition event provided by the FMS.
   *
   * @return A string containing the event name
   */
  static std::string GetEventName() {
    return wpi::internal::DriverStationBackend::GetEventName();
  }

  /**
   * Returns the game specific message provided by the FMS.
   *
   * If the FMS is not connected, it is set from the game data setting on the
   * driver station.
   *
   * @return A string containing the game specific message.
   */
  static std::optional<std::string> GetGameData() {
    return wpi::internal::DriverStationBackend::GetGameData();
  }
};

}  // namespace wpi
