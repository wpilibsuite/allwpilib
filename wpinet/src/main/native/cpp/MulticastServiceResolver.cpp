// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/MulticastServiceResolver.h"

#include <memory>
#include <utility>
#include <vector>

#include <wpi/MemAlloc.h>

#include "MulticastHandleManager.h"

extern "C" {
WPI_MulticastServiceResolverHandle WPI_CreateMulticastServiceResolver(
    const char* serviceType)

{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};

  auto resolver = std::make_unique<wpi::MulticastServiceResolver>(serviceType);

  size_t index = manager.handleIds.emplace_back(2);
  manager.resolvers[index] = std::move(resolver);

  return index;
}

void WPI_FreeMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.resolvers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

void WPI_StartMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Start();
}

void WPI_StopMulticastServiceResolver(
    WPI_MulticastServiceResolverHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  resolver->Stop();
}

int32_t WPI_GetMulticastServiceResolverHasImplementation(
    WPI_MulticastServiceResolverHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->HasImplementation();
}

WPI_EventHandle WPI_GetMulticastServiceResolverEventHandle(
    WPI_MulticastServiceResolverHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& resolver = manager.resolvers[handle];
  return resolver->GetEventHandle();
}

WPI_ServiceData* WPI_GetMulticastServiceResolverData(
    WPI_MulticastServiceResolverHandle handle, int32_t* dataCount) {
  std::vector<wpi::MulticastServiceResolver::ServiceData> allData;
  {
    auto& manager = wpi::GetMulticastManager();
    std::scoped_lock lock{manager.mutex};
    auto& resolver = manager.resolvers[handle];
    allData = resolver->GetData();
  }
  if (allData.empty()) {
    *dataCount = 0;
    return nullptr;
  }
  size_t allocSize = sizeof(WPI_ServiceData) * allData.size();

  for (auto&& data : allData) {
    // Include space for hostName and serviceType (+ terminators)
    allocSize += data.hostName.size() + data.serviceName.size() + 2;

    size_t keysTotalLength = 0;
    size_t valuesTotalLength = 0;
    // Include space for all keys and values, and pointer array
    for (auto&& t : data.txt) {
      allocSize += sizeof(const char*);
      keysTotalLength += (t.first.size() + 1);
      allocSize += sizeof(const char*);
      valuesTotalLength += (t.second.size() + 1);
    }
    allocSize += keysTotalLength;
    allocSize += valuesTotalLength;
  }

  uint8_t* cDataRaw = reinterpret_cast<uint8_t*>(wpi::safe_malloc(allocSize));
  if (!cDataRaw) {
    return nullptr;
  }
  WPI_ServiceData* rootArray = reinterpret_cast<WPI_ServiceData*>(cDataRaw);
  cDataRaw += (sizeof(WPI_ServiceData) + allData.size());
  WPI_ServiceData* currentData = rootArray;

  for (auto&& data : allData) {
    currentData->ipv4Address = data.ipv4Address;
    currentData->port = data.port;
    currentData->txtCount = data.txt.size();

    std::memcpy(cDataRaw, data.hostName.c_str(), data.hostName.size() + 1);
    currentData->hostName = reinterpret_cast<const char*>(cDataRaw);
    cDataRaw += data.hostName.size() + 1;

    std::memcpy(cDataRaw, data.serviceName.c_str(),
                data.serviceName.size() + 1);
    currentData->serviceName = reinterpret_cast<const char*>(cDataRaw);
    cDataRaw += data.serviceName.size() + 1;

    char** valuesPtrArr = reinterpret_cast<char**>(cDataRaw);
    cDataRaw += (sizeof(char**) * data.txt.size());
    char** keysPtrArr = reinterpret_cast<char**>(cDataRaw);
    cDataRaw += (sizeof(char**) * data.txt.size());

    currentData->txtKeys = const_cast<const char**>(keysPtrArr);
    currentData->txtValues = const_cast<const char**>(valuesPtrArr);

    for (size_t i = 0; i < data.txt.size(); i++) {
      keysPtrArr[i] = reinterpret_cast<char*>(cDataRaw);
      std::memcpy(keysPtrArr[i], data.txt[i].first.c_str(),
                  data.txt[i].first.size() + 1);
      cDataRaw += (data.txt[i].first.size() + 1);

      valuesPtrArr[i] = reinterpret_cast<char*>(cDataRaw);
      std::memcpy(valuesPtrArr[i], data.txt[i].second.c_str(),
                  data.txt[i].second.size() + 1);
      cDataRaw += (data.txt[i].second.size() + 1);
    }
    currentData++;
  }

  return rootArray;
}

void WPI_FreeServiceData(WPI_ServiceData* serviceData, int32_t length) {
  std::free(serviceData);
}
}  // extern "C"
