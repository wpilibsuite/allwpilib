// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#include <WinDNS.h>

namespace wpi {
class DynamicDns {
 public:
  using DnsServiceFreeInstanceFunc =
      VOID(WINAPI*)(_In_ PDNS_SERVICE_INSTANCE pInstance);

  using DnsServiceConstructInstanceFunc = PDNS_SERVICE_INSTANCE(WINAPI*)(
      _In_ PCWSTR pServiceName, _In_ PCWSTR pHostName,
      _In_opt_ PIP4_ADDRESS pIp4, _In_opt_ PIP6_ADDRESS pIp6, _In_ WORD wPort,
      _In_ WORD wPriority, _In_ WORD wWeight, _In_ DWORD dwPropertiesCount,
      _In_reads_(dwPropertiesCount) PCWSTR* keys,
      _In_reads_(dwPropertiesCount) PCWSTR* values);

  using DnsServiceRegisterFunc =
      DWORD(WINAPI*)(_In_ PDNS_SERVICE_REGISTER_REQUEST pRequest,
                     _Inout_opt_ PDNS_SERVICE_CANCEL pCancel);

  using DnsServiceDeRegisterFunc =
      DWORD(WINAPI*)(_In_ PDNS_SERVICE_REGISTER_REQUEST pRequest,
                     _Inout_opt_ PDNS_SERVICE_CANCEL pCancel);

  using DnsServiceBrowseFunc =
      DNS_STATUS(WINAPI*)(_In_ PDNS_SERVICE_BROWSE_REQUEST pRequest,
                          _Inout_ PDNS_SERVICE_CANCEL pCancel);

  using DnsServiceBrowseCancelFunc =
      DNS_STATUS(WINAPI*)(_In_ PDNS_SERVICE_CANCEL pCancelHandle);

  DnsServiceBrowseFunc DnsServiceBrowsePtr{nullptr};
  DnsServiceBrowseCancelFunc DnsServiceBrowseCancelPtr{nullptr};

  DnsServiceFreeInstanceFunc DnsServiceFreeInstancePtr{nullptr};
  DnsServiceConstructInstanceFunc DnsServiceConstructInstancePtr{nullptr};
  DnsServiceRegisterFunc DnsServiceRegisterPtr{nullptr};
  DnsServiceDeRegisterFunc DnsServiceDeRegisterPtr{nullptr};

  bool CanDnsAnnounce{false};
  bool CanDnsResolve{false};

  static DynamicDns& GetDynamicDns();

 private:
  DynamicDns();
};
}  // namespace wpi
