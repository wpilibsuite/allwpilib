// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DynamicDns.h"

using namespace wpi;

DynamicDns& DynamicDns::GetDynamicDns() {
  static DynamicDns dns;
  return dns;
}

DynamicDns::DynamicDns() {
  HMODULE library = LoadLibraryW(L"dnsapi");

  if (library == nullptr) {
    return;
  }

  DnsServiceFreeInstancePtr = (DnsServiceFreeInstanceFunc)GetProcAddress(
      library, "DnsServiceFreeInstance");
  DnsServiceConstructInstancePtr =
      (DnsServiceConstructInstanceFunc)GetProcAddress(
          library, "DnsServiceConstructInstance");
  DnsServiceRegisterPtr =
      (DnsServiceRegisterFunc)GetProcAddress(library, "DnsServiceRegister");
  DnsServiceDeRegisterPtr =
      (DnsServiceDeRegisterFunc)GetProcAddress(library, "DnsServiceDeRegister");

  CanDnsAnnounce = DnsServiceFreeInstancePtr &&
                   DnsServiceConstructInstancePtr && DnsServiceRegisterPtr &&
                   DnsServiceDeRegisterPtr;

  DnsServiceBrowsePtr =
      (DnsServiceBrowseFunc)GetProcAddress(library, "DnsServiceBrowse");
  DnsServiceBrowseCancelPtr = (DnsServiceBrowseCancelFunc)GetProcAddress(
      library, "DnsServiceBrowseCancel");

  CanDnsResolve = DnsServiceBrowsePtr && DnsServiceBrowseCancelPtr;
}
