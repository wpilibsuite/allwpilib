// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#include "wpi/mDNSResolver.h"

#include <string>

#include "DynamicDns.h"
#include "wpi/ConvertUTF.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringExtras.h"

#pragma comment(lib, "dnsapi")

using namespace wpi;

struct mDNSResolver::Impl {
  wpi::DynamicDns& dynamicDns = wpi::DynamicDns::GetDynamicDns();
  std::wstring serviceType;
  DNS_SERVICE_CANCEL serviceCancel{nullptr};
  mDnsRevolveCompletionFunc onFound;
};

mDNSResolver::mDNSResolver(std::string_view serviceType,
                           mDnsRevolveCompletionFunc onFound) {
  pImpl = std::make_unique<Impl>();
  pImpl->onFound = onFound;

  if (!pImpl->dynamicDns.CanDnsResolve) {
    return;
  }

  wpi::SmallVector<wpi::UTF16, 128> wideStorage;

  if (wpi::ends_with_lower(serviceType, ".local")) {
    wpi::convertUTF8ToUTF16String(serviceType, wideStorage);
  } else {
    wpi::SmallString<128> storage;
    storage.append(serviceType);
    storage.append(".local");
    wpi::convertUTF8ToUTF16String(storage.str(), wideStorage);
  }
  pImpl->serviceType = std::wstring{
      reinterpret_cast<const wchar_t*>(wideStorage.data()), wideStorage.size()};
}

mDNSResolver::~mDNSResolver() noexcept {}

static _Function_class_(DNS_QUERY_COMPLETION_ROUTINE) VOID WINAPI
    DnsCompletion(_In_ PVOID pQueryContext,
                  _Inout_ PDNS_QUERY_RESULT pQueryResults) {
  mDNSResolver::Impl* impl =
      reinterpret_cast<mDNSResolver::Impl*>(pQueryContext);

  wpi::SmallVector<DNS_RECORDW*, 4> PtrRecords;
  wpi::SmallVector<DNS_RECORDW*, 4> SrvRecords;
  wpi::SmallVector<DNS_RECORDW*, 4> TxtRecords;
  wpi::SmallVector<DNS_RECORDW*, 4> ARecords;

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

    wpi::SmallVector<std::pair<std::string, std::string>, 4> extraTxt;

    for (DNS_RECORDW* A : ARecords) {
      if (std::wstring_view{A->pName} ==
          std::wstring_view{foundSrv->Data.Srv.pNameTarget}) {
        extraTxt.clear();
        for (DNS_RECORDW* Txt : TxtRecords) {
          if (std::wstring_view{Txt->pName} == nameHost) {
            for (DWORD i = 0; i < Txt->Data.Txt.dwStringCount; i++) {
              std::wstring_view wideView = Txt->Data.TXT.pStringArray[i];
              size_t splitIndex = wideView.find(L'=');
              if (splitIndex == wideView.npos) {
                continue;
              }
              std::string key;
              wpi::SmallString<64> str;
              wpi::span<const wpi::UTF16> wideStr{
                  reinterpret_cast<const wpi::UTF16*>(wideView.data()),
                  splitIndex};
              wpi::convertUTF16ToUTF8String(wideStr, str);
              key = str.string();
              str.clear();
              wideStr = wpi::span<const wpi::UTF16>{
                  reinterpret_cast<const wpi::UTF16*>(wideView.data() +
                                                      splitIndex + 1),
                  wideView.size() - splitIndex - 1};
              wpi::convertUTF16ToUTF8String(wideStr, str);
              extraTxt.emplace_back(std::pair<std::string, std::string>{
                  std::move(key), str.string()});
            }
          }
        }

        wpi::SmallString<128> hostName;
        wpi::span<const wpi::UTF16> wideHostName{
            reinterpret_cast<const wpi::UTF16*>(A->pName), wcslen(A->pName)};
        wpi::convertUTF16ToUTF8String(wideHostName, hostName);

        wpi::SmallString<128> serviceName;
        int len = nameHost.find(impl->serviceType.c_str());
        wpi::span<const wpi::UTF16> wideServiceName{
            reinterpret_cast<const wpi::UTF16*>(nameHost.data()),
            nameHost.size()};
        if (len != nameHost.npos) {
          wideServiceName = wideServiceName.subspan(0, len - 1);
        }
        wpi::convertUTF16ToUTF8String(wideServiceName, serviceName);

        impl->onFound(A->Data.A.IpAddress, serviceName.str(), hostName.str(),
                      extraTxt);
      }
    }
  }
  DnsFree(pQueryResults->pQueryRecords, DNS_FREE_TYPE::DnsFreeRecordList);
}

void mDNSResolver::Start() {
  if (pImpl->serviceCancel.reserved != nullptr) {
    return;
  }

  if (!pImpl->dynamicDns.CanDnsResolve) {
    return;
  }

  DNS_SERVICE_BROWSE_REQUEST request = {};
  request.InterfaceIndex = 0;
  request.pQueryContext = pImpl.get();
  request.QueryName = pImpl->serviceType.c_str();
  request.Version = 2;
  request.pBrowseCallbackV2 = DnsCompletion;
  pImpl->dynamicDns.DnsServiceBrowsePtr(&request, &pImpl->serviceCancel);
}

void mDNSResolver::Stop() {
  if (!pImpl->dynamicDns.CanDnsResolve) {
    return;
  }

  if (pImpl->serviceCancel.reserved == nullptr) {
    return;
  }

  pImpl->dynamicDns.DnsServiceBrowseCancelPtr(&pImpl->serviceCancel);
  pImpl->serviceCancel.reserved = nullptr;
}
