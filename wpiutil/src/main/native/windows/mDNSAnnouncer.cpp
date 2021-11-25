// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#include "wpi/mDNSAnnouncer.h"

#include <string>
#include <vector>

#include "DynamicDns.h"
#include "wpi/ConvertUTF.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringExtras.h"

using namespace wpi;

struct ImplBase {
  wpi::DynamicDns& dynamicDns = wpi::DynamicDns::GetDynamicDns();
  PDNS_SERVICE_INSTANCE serviceInstance = nullptr;
  HANDLE event = nullptr;
};

struct mDNSAnnouncer::Impl : ImplBase {
  std::wstring serviceType;
  std::wstring serviceInstanceName;
  std::wstring hostName;
  std::vector<std::wstring> keys;
  std::vector<PCWSTR> keyPtrs;
  std::vector<std::wstring> values;
  std::vector<PCWSTR> valuePtrs;
};

mDNSAnnouncer::mDNSAnnouncer(
    std::string_view serviceName, std::string_view serviceType,
    std::string_view hostName,
    wpi::span<std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>();

  if (!pImpl->dynamicDns.CanDnsAnnounce) {
    return;
  }

  wpi::SmallVector<wpi::UTF16, 128> wideStorage;

  for (auto&& i : txt) {
    wideStorage.clear();
    wpi::convertUTF8ToUTF16String(i.first, wideStorage);
    pImpl->keys.emplace_back(
        std::wstring{reinterpret_cast<const wchar_t*>(wideStorage.data()),
                     wideStorage.size()});
    wideStorage.clear();
    wpi::convertUTF8ToUTF16String(i.second, wideStorage);
    pImpl->values.emplace_back(
        std::wstring{reinterpret_cast<const wchar_t*>(wideStorage.data()),
                     wideStorage.size()});
  }

  for (size_t i = 0; i < pImpl->keys.size(); i++) {
    pImpl->keyPtrs.emplace_back(pImpl->keys[i].c_str());
    pImpl->valuePtrs.emplace_back(pImpl->values[i].c_str());
  }

  wpi::SmallString<128> storage;

  wideStorage.clear();
  if (wpi::ends_with_lower(hostName, ".local")) {
    wpi::convertUTF8ToUTF16String(hostName, wideStorage);
  } else {
    storage.append(hostName);
    storage.append(".local");
    wpi::convertUTF8ToUTF16String(storage.str(), wideStorage);
  }

  pImpl->hostName = std::wstring{
      reinterpret_cast<const wchar_t*>(wideStorage.data()), wideStorage.size()};

  wideStorage.clear();
  if (wpi::ends_with_lower(serviceType, ".local")) {
    wpi::convertUTF8ToUTF16String(serviceType, wideStorage);
  } else {
    storage.clear();
    storage.append(serviceType);
    storage.append(".local");
    wpi::convertUTF8ToUTF16String(storage.str(), wideStorage);
  }
  pImpl->serviceType = std::wstring{
      reinterpret_cast<const wchar_t*>(wideStorage.data()), wideStorage.size()};

  wideStorage.clear();
  storage.clear();
  storage.append(serviceName);
  storage.append(".");
  storage.append(serviceType);
  if (!wpi::ends_with_lower(serviceType, ".local")) {
    storage.append(".local");
  }

  wpi::convertUTF8ToUTF16String(storage.str(), wideStorage);
  pImpl->serviceInstanceName = std::wstring{
      reinterpret_cast<const wchar_t*>(wideStorage.data()), wideStorage.size()};
}

mDNSAnnouncer::~mDNSAnnouncer() noexcept {
  Stop();
}

static void WINAPI DnsServiceRegisterCallback(DWORD /*Status*/,
                                              PVOID pQueryContext,
                                              PDNS_SERVICE_INSTANCE pInstance) {
  ImplBase* impl = reinterpret_cast<ImplBase*>(pQueryContext);

  impl->serviceInstance = pInstance;

  SetEvent(impl->event);
}

void mDNSAnnouncer::Start() {
  if (pImpl->serviceInstance) {
    return;
  }

  if (!pImpl->dynamicDns.CanDnsAnnounce) {
    return;
  }

  PDNS_SERVICE_INSTANCE serviceInst =
      pImpl->dynamicDns.DnsServiceConstructInstancePtr(
          pImpl->serviceInstanceName.c_str(), pImpl->hostName.c_str(), nullptr,
          nullptr, 5000, 0, 0, static_cast<DWORD>(pImpl->keyPtrs.size()),
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

void mDNSAnnouncer::Stop() {
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
