// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/Synchronization.h"

#ifdef __cplusplus
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/ConcurrentQueue.h"
#include "wpi/span.h"
namespace wpi {
class MulticastServiceResolver {
 public:
  explicit MulticastServiceResolver(std::string_view serviceType);
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
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int WPI_MulticastServiceResolverHandle;  // NOLINT

WPI_MulticastServiceResolverHandle WPI_CreateMulticastServiceResolver(
    const char* serviceType);

void WPI_FreeMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle);

void WPI_StartMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle);

void WPI_StopMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle);

int32_t WPI_GetMulticastServiceResolverHasImplementation(
    WPI_MulticastServiceResolverHandle handle);

WPI_EventHandle WPI_GetMulticastServiceResolverEventHandle(
    WPI_MulticastServiceResolverHandle handle);

typedef struct WPI_ServiceData {  // NOLINT
  uint32_t ipv4Address;
  int32_t port;
  const char* serviceName;
  const char* hostName;
  int32_t txtCount;
  const char** txtKeys;
  const char** txtValues;
} WPI_ServiceData;

WPI_ServiceData* WPI_GetMulticastServiceResolverData(
    WPI_MulticastServiceResolverHandle handle);

void WPI_FreeServiceData(WPI_ServiceData* serviceData);

#ifdef __cplusplus
}  // extern "C"
#endif
