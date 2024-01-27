// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
namespace wpi {
class MulticastServiceAnnouncer {
 public:
  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param txt txt
   */
  MulticastServiceAnnouncer(
      std::string_view serviceName, std::string_view serviceType, int port,
      std::span<const std::pair<std::string, std::string>> txt);
  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param txt txt
   */
  MulticastServiceAnnouncer(
      std::string_view serviceName, std::string_view serviceType, int port,
      std::span<const std::pair<std::string_view, std::string_view>> txt);
  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   */
  MulticastServiceAnnouncer(std::string_view serviceName,
                            std::string_view serviceType, int port);
  ~MulticastServiceAnnouncer() noexcept;
  /**
   * Starts multicast service announcer.
   */
  void Start();
  /**
   * Stops multicast service announcer.
   */
  void Stop();
  /**
   * Returns true if there's a multicast service announcer implementation.
   *
   * @return True if there's a multicast service announcer implementation.
   */
  bool HasImplementation() const;
  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
extern "C" {
#endif

typedef unsigned int WPI_MulticastServiceAnnouncerHandle;  // NOLINT

WPI_MulticastServiceAnnouncerHandle WPI_CreateMulticastServiceAnnouncer(
    const char* serviceName, const char* serviceType, int32_t port,
    int32_t txtCount, const char** keys, const char** values);

void WPI_FreeMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

void WPI_StartMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

void WPI_StopMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

int32_t WPI_GetMulticastServiceAnnouncerHasImplementation(
    WPI_MulticastServiceAnnouncerHandle handle);

#ifdef __cplusplus
}  // extern "C"
#endif
