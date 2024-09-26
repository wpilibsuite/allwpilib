// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#include "wpinet/MulticastServiceAnnouncer.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/ConvertUTF.h>
#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

#include "DynamicDns.h"
#include "wpinet/hostname.h"

using namespace wpi;

struct ImplBase {
  wpi::DynamicDns& dynamicDns = wpi::DynamicDns::GetDynamicDns();
  PDNS_SERVICE_INSTANCE serviceInstance = nullptr;
  HANDLE event = nullptr;
};

struct MulticastServiceAnnouncer::Impl : ImplBase {
  std::wstring serviceType;
  std::wstring serviceInstanceName;
  std::wstring hostName;
  int port;
  std::vector<std::wstring> keys;
  std::vector<PCWSTR> keyPtrs;
  std::vector<std::wstring> values;
  std::vector<PCWSTR> valuePtrs;

  template <typename T>
  Impl(std::string_view serviceName, std::string_view serviceType, int port,
       std::span<const std::pair<T, T>> txt);
};

template <typename T>
MulticastServiceAnnouncer::Impl::Impl(std::string_view serviceName,
                                      std::string_view serviceType, int port,
                                      std::span<const std::pair<T, T>> txt) {
  if (!dynamicDns.CanDnsAnnounce) {
    return;
  }

  this->port = port;

  wpi::SmallVector<wchar_t, 128> wideStorage;
  std::string hostName = wpi::GetHostname() + ".local";

  for (auto&& i : txt) {
    wideStorage.clear();
    wpi::sys::windows::UTF8ToUTF16(i.first, wideStorage);
    this->keys.emplace_back(
        std::wstring{wideStorage.data(), wideStorage.size()});
    wideStorage.clear();
    wpi::sys::windows::UTF8ToUTF16(i.second, wideStorage);
    this->values.emplace_back(
        std::wstring{wideStorage.data(), wideStorage.size()});
  }

  for (size_t i = 0; i < this->keys.size(); i++) {
    this->keyPtrs.emplace_back(this->keys[i].c_str());
    this->valuePtrs.emplace_back(this->values[i].c_str());
  }

  wpi::SmallString<128> storage;

  wideStorage.clear();
  wpi::sys::windows::UTF8ToUTF16(hostName, wideStorage);

  this->hostName = std::wstring{wideStorage.data(), wideStorage.size()};

  wideStorage.clear();
  if (wpi::ends_with_lower(serviceType, ".local")) {
    wpi::sys::windows::UTF8ToUTF16(serviceType, wideStorage);
  } else {
    storage.clear();
    storage.append(serviceType);
    storage.append(".local");
    wpi::sys::windows::UTF8ToUTF16(storage.str(), wideStorage);
  }
  this->serviceType = std::wstring{wideStorage.data(), wideStorage.size()};

  wideStorage.clear();
  storage.clear();
  storage.append(serviceName);
  storage.append(".");
  storage.append(serviceType);
  if (!wpi::ends_with_lower(serviceType, ".local")) {
    storage.append(".local");
  }

  wpi::sys::windows::UTF8ToUTF16(storage.str(), wideStorage);
  this->serviceInstanceName =
      std::wstring{wideStorage.data(), wideStorage.size()};
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port) {
  std::span<const std::pair<std::string_view, std::string_view>> txt;
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    std::span<const std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    std::span<const std::pair<std::string_view, std::string_view>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::~MulticastServiceAnnouncer() noexcept {
  Stop();
}

bool MulticastServiceAnnouncer::HasImplementation() const {
  return pImpl->dynamicDns.CanDnsAnnounce;
}

static void WINAPI DnsServiceRegisterCallback(DWORD /*Status*/,
                                              PVOID pQueryContext,
                                              PDNS_SERVICE_INSTANCE pInstance) {
  ImplBase* impl = reinterpret_cast<ImplBase*>(pQueryContext);

  impl->serviceInstance = pInstance;

  SetEvent(impl->event);
}

void MulticastServiceAnnouncer::Start() {
  if (pImpl->serviceInstance) {
    return;
  }

  if (!pImpl->dynamicDns.CanDnsAnnounce) {
    return;
  }

  PDNS_SERVICE_INSTANCE serviceInst =
      pImpl->dynamicDns.DnsServiceConstructInstancePtr(
          pImpl->serviceInstanceName.c_str(), pImpl->hostName.c_str(), nullptr,
          nullptr, pImpl->port, 0, 0, static_cast<DWORD>(pImpl->keyPtrs.size()),
          pImpl->keyPtrs.data(), pImpl->valuePtrs.data());
  if (serviceInst == nullptr) {
    return;
  }

  DNS_SERVICE_REGISTER_REQUEST registerRequest = {};
  registerRequest.pQueryContext = static_cast<ImplBase*>(pImpl.get());
  registerRequest.pRegisterCompletionCallback = DnsServiceRegisterCallback;
  registerRequest.Version = DNS_QUERY_REQUEST_VERSION1;
  registerRequest.unicastEnabled = false;
  registerRequest.pServiceInstance = serviceInst;
  registerRequest.InterfaceIndex = 0;

  pImpl->event = CreateEvent(NULL, true, false, NULL);

  if (pImpl->dynamicDns.DnsServiceRegisterPtr(&registerRequest, nullptr) ==
      DNS_REQUEST_PENDING) {
    WaitForSingleObject(pImpl->event, INFINITE);
  }

  pImpl->dynamicDns.DnsServiceFreeInstancePtr(serviceInst);
  CloseHandle(pImpl->event);
  pImpl->event = nullptr;
}

static void WINAPI DnsServiceDeRegisterCallback(
    DWORD /*Status*/, PVOID pQueryContext, PDNS_SERVICE_INSTANCE pInstance) {
  ImplBase* impl = reinterpret_cast<ImplBase*>(pQueryContext);

  if (pInstance != nullptr) {
    impl->dynamicDns.DnsServiceFreeInstancePtr(pInstance);
    pInstance = nullptr;
  }

  SetEvent(impl->event);
}

void MulticastServiceAnnouncer::Stop() {
  if (!pImpl->dynamicDns.CanDnsAnnounce) {
    return;
  }

  if (pImpl->serviceInstance == nullptr) {
    return;
  }

  pImpl->event = CreateEvent(NULL, true, false, NULL);
  DNS_SERVICE_REGISTER_REQUEST registerRequest = {};
  registerRequest.pQueryContext = static_cast<ImplBase*>(pImpl.get());
  registerRequest.pRegisterCompletionCallback = DnsServiceDeRegisterCallback;
  registerRequest.Version = DNS_QUERY_REQUEST_VERSION1;
  registerRequest.unicastEnabled = false;
  registerRequest.pServiceInstance = pImpl->serviceInstance;
  registerRequest.InterfaceIndex = 0;

  if (pImpl->dynamicDns.DnsServiceDeRegisterPtr(&registerRequest, nullptr) ==
      DNS_REQUEST_PENDING) {
    WaitForSingleObject(pImpl->event, INFINITE);
  }

  pImpl->dynamicDns.DnsServiceFreeInstancePtr(pImpl->serviceInstance);
  pImpl->serviceInstance = nullptr;
  CloseHandle(pImpl->event);
  pImpl->event = nullptr;
}
