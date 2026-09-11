// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace wpi::cmd {

/**
 * Helper class for fetching information about the current opmode. This is a
 * package-private class so tests for this library don't need to hook into
 * driverstation simulation and the HAL.
 */
class OpModeFetcher {
 public:
  virtual ~OpModeFetcher() = default;

  /**
   * Gets the current opmode ID.
   *
   * @return The unique ID of the current operating mode.
   */
  virtual int64_t GetOpModeId() = 0;

  /**
   * Gets the current opmode name.
   *
   * @return The name of the current operating mode.
   */
  virtual std::string GetOpModeName() = 0;

  /**
   * Gets the current fetcher implementation. If SetFetcher() has not already
   * been called to set an implementation, this will default to a
   * DriverStationOpModeFetcher instance.
   *
   * @return The fetcher instance to use to get opmode information.
   */
  static OpModeFetcher& GetFetcher();

  /**
   * Sets the fetcher to use. In tests, this is reset before every test with an
   * implementation that always returns an ID of 0 and an empty string for the
   * opmode name.
   *
   * @param fetcher The fetcher implementation to set.
   */
  static void SetFetcher(std::unique_ptr<OpModeFetcher> fetcher);

 private:
  static std::unique_ptr<OpModeFetcher> s_fetcher;
};

/**
 * Default implementation that fetches opmode information from the Driver
 * Station.
 */
class DriverStationOpModeFetcher : public OpModeFetcher {
 public:
  int64_t GetOpModeId() override;
  std::string GetOpModeName() override;
};

}  // namespace wpi::cmd
