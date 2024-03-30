// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

namespace wpi2 {

/** Sendable publish/subscribe options. */
struct SendableOptions {
  /**
   * Default value of periodic.
   */
  static constexpr double kDefaultPeriodic = 0.1;

  /**
   * Type string.  Default is used for the data type if this is empty.
   * Not used for raw values (pass the type string directly to the functions).
   */
  std::string typeString;

  /**
   * Polling storage size for a subscription. Specifies the maximum number of
   * updates the backend should store. If zero, defaults to 1 if sendAll is
   * false, 20 if sendAll is true.
   */
  unsigned int pollStorage = 0;

  /**
   * How frequently changes will be sent over the network, in seconds.
   * The backend may send more frequently than this (e.g. use a combined
   * minimum period for all values) or apply a restricted range to this value.
   * The default is 100 ms.
   */
  double periodic = kDefaultPeriodic;

  /**
   * Send all value changes.
   */
  bool sendAll = false;

  /**
   * Preserve duplicate value changes (rather than ignoring them).
   */
  bool keepDuplicates = false;

  /**
   * For subscriptions, if remote value updates should be ignored. See also
   * disableLocal.
   */
  bool disableRemote = false;

  /**
   * For subscriptions, if local value updates should be ignored. See also
   * disableRemote.
   */
  bool disableLocal = false;
};

constexpr SendableOptions kDefaultSendableOptions;

}  // namespace wpi2
