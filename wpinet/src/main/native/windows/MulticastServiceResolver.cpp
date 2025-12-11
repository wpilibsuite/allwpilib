// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#include "wpi/net/MulticastServiceResolver.h"

#include <Windows.h>
#include <WinDNS.h>

#include <memory>
#include <string>
#include <utility>

#include "wpi/util/ConvertUTF.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringExtras.hpp"

#pragma comment(lib, "dnsapi")

using namespace wpi::net;

struct MulticastServiceResolver::Impl {
  std::wstring serviceType;
  DNS_SERVICE_CANCEL serviceCancel{nullptr};

  MulticastServiceResolver* resolver;

  void onFound(ServiceData&& data) {
    resolver->PushData(std::forward<ServiceData>(data));
  }
};

MulticastServiceResolver::MulticastServiceResolver(
    std::string_view serviceType) {
  pImpl = std::make_unique<Impl>();
  pImpl->resolver = this;

  wpi::util::SmallVector<wchar_t, 128> wideStorage;

  if (wpi::util::ends_with_lower(serviceType, ".local")) {
    wpi::util::sys::windows::UTF8ToUTF16(serviceType, wideStorage);
  } else {
    wpi::util::SmallString<128> storage;
    storage.append(serviceType);
    storage.append(".local");
    wpi::util::sys::windows::UTF8ToUTF16(storage.str(), wideStorage);
  }
  pImpl->serviceType = std::wstring{wideStorage.data(), wideStorage.size()};
}

MulticastServiceResolver::~MulticastServiceResolver() noexcept {
  Stop();
}

bool MulticastServiceResolver::HasImplementation() const {
  return true;
}

bool MulticastServiceResolver::SetCopyCallback(
    std::function<bool(const ServiceData&)> callback) {
  if (pImpl->serviceCancel.reserved != nullptr) {
    return false;
  }
  copyCallback = std::move(callback);
  return true;
}

bool MulticastServiceResolver::SetMoveCallback(
    std::function<void(ServiceData&&)> callback) {
  if (pImpl->serviceCancel.reserved != nullptr) {
    return false;
  }
  moveCallback = std::move(callback);
  return true;
}

static _Function_class_(DNS_QUERY_COMPLETION_ROUTINE) VOID WINAPI
    DnsCompletion(_In_ PVOID pQueryContext,
                  _Inout_ PDNS_QUERY_RESULT pQueryResults) {
  if (pQueryResults->QueryStatus != ERROR_SUCCESS) {
    return;
  }

  MulticastServiceResolver::Impl* impl =
      reinterpret_cast<MulticastServiceResolver::Impl*>(pQueryContext);

  wpi::util::SmallVector<DNS_RECORDW*, 4> PtrRecords;
  wpi::util::SmallVector<DNS_RECORDW*, 4> SrvRecords;
  wpi::util::SmallVector<DNS_RECORDW*, 4> TxtRecords;
  wpi::util::SmallVector<DNS_RECORDW*, 4> ARecords;

  {
    DNS_RECORDW* current = pQueryResults->pQueryRecords;
    while (current != nullptr) {
      switch (current->wType) {
        case DNS_TYPE_PTR:
          PtrRecords.push_back(current);
          break;
        case DNS_TYPE_SRV:
          SrvRecords.push_back(current);
          break;
        case DNS_TYPE_TEXT:
          TxtRecords.push_back(current);
          break;
        case DNS_TYPE_A:
          ARecords.push_back(current);
          break;
      }
      current = current->pNext;
    }
  }

  for (DNS_RECORDW* Ptr : PtrRecords) {
    if (std::wstring_view{Ptr->pName} != impl->serviceType) {
      continue;
    }

    std::wstring_view nameHost = Ptr->Data.Ptr.pNameHost;
    DNS_RECORDW* foundSrv = nullptr;
    for (DNS_RECORDW* Srv : SrvRecords) {
      if (std::wstring_view{Srv->pName} == nameHost) {
        foundSrv = Srv;
        break;
      }
    }

    if (!foundSrv) {
      continue;
    }

    for (DNS_RECORDW* A : ARecords) {
      if (std::wstring_view{A->pName} ==
          std::wstring_view{foundSrv->Data.Srv.pNameTarget}) {
        MulticastServiceResolver::ServiceData data;
        wpi::util::SmallString<128> storage;
        for (DNS_RECORDW* Txt : TxtRecords) {
          if (std::wstring_view{Txt->pName} == nameHost) {
            for (DWORD i = 0; i < Txt->Data.Txt.dwStringCount; i++) {
              std::wstring_view wideView = Txt->Data.TXT.pStringArray[i];
              size_t splitIndex = wideView.find(L'=');
              if (splitIndex == wideView.npos) {
                // Todo make this just do key
                continue;
              }
              storage.clear();
              std::span<const wpi::util::UTF16> wideStr{
                  reinterpret_cast<const wpi::util::UTF16*>(wideView.data()),
                  splitIndex};
              wpi::util::convertUTF16ToUTF8String(wideStr, storage);
              auto& pair =
                  data.txt.emplace_back(std::pair<std::string, std::string>{
                      std::string{storage}, {}});
              storage.clear();
              wideStr = std::span<const wpi::util::UTF16>{
                  reinterpret_cast<const wpi::util::UTF16*>(wideView.data() +
                                                            splitIndex + 1),
                  wideView.size() - splitIndex - 1};
              wpi::util::convertUTF16ToUTF8String(wideStr, storage);
              pair.second = std::string{storage};
            }
          }
        }

        storage.clear();
        std::span<const wpi::util::UTF16> wideHostName{
            reinterpret_cast<const wpi::util::UTF16*>(A->pName),
            wcslen(A->pName)};
        wpi::util::convertUTF16ToUTF8String(wideHostName, storage);
        storage.append(".");

        data.hostName = std::string{storage};
        storage.clear();

        int len = nameHost.find(impl->serviceType.c_str());
        std::span<const wpi::util::UTF16> wideServiceName{
            reinterpret_cast<const wpi::util::UTF16*>(nameHost.data()),
            nameHost.size()};
        if (len != nameHost.npos) {
          wideServiceName = wideServiceName.subspan(0, len - 1);
        }
        wpi::util::convertUTF16ToUTF8String(wideServiceName, storage);

        data.serviceName = std::string{storage};
        data.port = ntohs(foundSrv->Data.Srv.wPort);
        data.ipv4Address = ntohl(A->Data.A.IpAddress);

        impl->onFound(std::move(data));
      }
    }
  }
  DnsFree(pQueryResults->pQueryRecords, DNS_FREE_TYPE::DnsFreeRecordList);
}

void MulticastServiceResolver::Start() {
  if (pImpl->serviceCancel.reserved != nullptr) {
    return;
  }

  DNS_SERVICE_BROWSE_REQUEST request = {};
  request.InterfaceIndex = 0;
  request.pQueryContext = pImpl.get();
  request.QueryName = pImpl->serviceType.c_str();
  request.Version = 2;
  request.pBrowseCallbackV2 = DnsCompletion;
  DnsServiceBrowse(&request, &pImpl->serviceCancel);
}

void MulticastServiceResolver::Stop() {
  if (pImpl->serviceCancel.reserved == nullptr) {
    return;
  }

  DnsServiceBrowseCancel(&pImpl->serviceCancel);
  pImpl->serviceCancel.reserved = nullptr;
}
