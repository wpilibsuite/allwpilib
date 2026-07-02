// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/net/MulticastServiceResolver.h"
#include "wpi/util/Synchronization.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::net {

class MulticastServiceResolver {
 public:
  explicit MulticastServiceResolver(std::string_view serviceType);

  ~MulticastServiceResolver() noexcept;

  struct ServiceData {
    /// IPv4 address in host order.
    unsigned int ipv4Address;
    /// Port number in host order.
    int port;
    /// Service name.
    std::string serviceName;
    /// Host name.
    std::string hostName;
    /// Service data payload.
    std::vector<std::pair<std::string, std::string>> txt;
  };

  /**
   * Set a copy callback to be called when a service is resolved.
   * Takes precedence over the move callback. Return true to
   * not send the data to the event queue.
   */
  bool SetCopyCallback(std::function<bool(const ServiceData&)> callback);

  /**
   * Set a move callback to be called when a service is resolved.
   * Data is moved into the function and cannot be added to the event queue.
   */
  bool SetMoveCallback(std::function<void(ServiceData&&)> callback);

  /**
   * Starts multicast service resolver.
   */
  void Start();

  /**
   * Stops multicast service resolver.
   */
  void Stop();

  /**
   * Returns event handle.
   *
   * @return Event handle.
   */
  WPI_EventHandle GetEventHandle() const { return event.GetHandle(); }

  /**
   * Returns multicast service resolver data.
   *
   * @return Multicast service resolver data.
   */
  std::vector<ServiceData> GetData() {
    std::scoped_lock lock{mutex};

    event.Reset();
    if (queue.empty()) {
      return {};
    }

    std::vector<ServiceData> ret;
    queue.swap(ret);
    return ret;
  }

  /**
   * Returns true if there's a multicast service resolver implementation.
   *
   * @return True if there's a multicast service resolver implementation.
   */
  bool HasImplementation() const;

  struct Impl;

 private:
  void PushData(ServiceData&& data) {
    std::scoped_lock lock{mutex};

    if (copyCallback) {
      if (!copyCallback(data)) {
        queue.emplace_back(std::forward<ServiceData>(data));
        event.Set();
      }
    } else if (moveCallback) {
      moveCallback(std::move(data));
    } else {
      queue.emplace_back(std::forward<ServiceData>(data));
      event.Set();
    }
  }

  wpi::util::Event event{true};
  std::vector<ServiceData> queue;
  wpi::util::mutex mutex;
  std::function<bool(const ServiceData&)> copyCallback;
  std::function<void(ServiceData&&)> moveCallback;
  std::unique_ptr<Impl> pImpl;
};

}  // namespace wpi::net
