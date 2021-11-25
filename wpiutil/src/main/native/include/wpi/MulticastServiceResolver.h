// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/ConcurrentQueue.h"
#include "wpi/Synchronization.h"
#include "wpi/span.h"

namespace wpi {
class MulticastServiceResolver {
 public:
  MulticastServiceResolver(std::string_view serviceType);
  ~MulticastServiceResolver() noexcept;

  struct ServiceData {
    unsigned int ipv4Address;
    int port;
    std::string serviceName;
    std::string hostName;
    std::vector<std::pair<std::string, std::string>> txt;
  };

  void Start();
  void Stop();

  WPI_EventHandle GetEventHandle() { return event.GetHandle(); }

  ServiceData GetData() { return eventQueue.pop(); }

  bool HasImplementation() const;

  struct Impl;

 private:
  wpi::Event event;
  wpi::ConcurrentQueue<ServiceData> eventQueue;

  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
