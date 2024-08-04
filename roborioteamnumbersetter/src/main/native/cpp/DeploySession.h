// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <wpi/Logger.h>

namespace rtns {
// Define an integer for a successful message in the log (shown in green on the
// GUI).
static constexpr unsigned int kLogSuccess = 31;

struct DeviceStatus {
  bool webServerEnabled = false;
  std::string serialNumber;
  std::string image;
};

/**
 * Represents a single deploy session.
 *
 * An instance of this class must be kept alive in memory until GetStatus()
 * returns kDiscoveryFailure or kDone. Otherwise, the deploy will fail!
 */
class DeploySession {
 public:
  /** Represents the status of the deploy session. */
  enum class Status { kInProgress, kDiscoveryFailure, kDone };

  /**
   * Constructs an instance of the deploy session.
   *
   * @param team   The team number (or an IP address/hostname).
   * @param drive  Whether the drive program should be deployed to the roboRIO.
   *               If this is set to false, the mechanism project will be
   *               deployed.
   * @param config The generation configuration file to be sent to the roboRIO.
   * @param logger A reference to a logger where log messages should be sent.
   */
  explicit DeploySession(wpi::Logger& logger);

  /**
   * Executes the deploy. This can be called from any thread.
   */
  bool ChangeTeamNumber(const std::string& macAddress, int team,
                        unsigned int ipAddress);

  bool Blink(const std::string& macAddress, unsigned int ipAddress);

  bool DisableWebServer(const std::string& macAddress, unsigned int ipAddress);

  bool EnableWebServer(const std::string& macAddress, unsigned int ipAddress);

  bool Reboot(const std::string& macAddress, unsigned int ipAddress);

  bool GetStatus(const std::string& macAddress, unsigned int ipAddress);

  std::future<int>* GetFuture(const std::string& macAddress);
  void DestroyFuture(const std::string& macAddress);

  std::future<DeviceStatus>* GetStatusFuture(const std::string& macAddress);
  void DestroyStatusFuture(const std::string& macAddress);

 private:
  // Logger reference where log messages will be sent.
  wpi::Logger& m_logger;

  // The number of hostnames that have completed their resolution/connection
  // attempts.
  std::atomic_int m_visited = 0;
};
}  // namespace rtns
