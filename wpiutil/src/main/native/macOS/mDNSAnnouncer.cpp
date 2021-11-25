// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/mDNSAnnouncer.h"

#include "dns_sd.h"

using namespace wpi;

struct mDNSAnnouncer::Impl {
  std::string serviceName;
  std::string serviceType;
  DNSServiceRef serviceRef{nullptr};
};

mDNSAnnouncer::mDNSAnnouncer(
    std::string_view serviceName, std::string_view serviceType,
    std::string_view hostName,
    wpi::span<std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>();
  pImpl->serviceName = serviceName;
  pImpl->serviceType = serviceType;
}

mDNSAnnouncer::~mDNSAnnouncer() noexcept {}

void mDNSAnnouncer::Start() {
  if (pImpl->serviceRef) {
    return;
  }

  (void)DNSServiceRegister(&pImpl->serviceRef, 0, 0, pImpl->serviceName.c_str(),
                           pImpl->serviceType.c_str(), "local", nullptr, 5000,
                           0, nullptr, nullptr, nullptr);
}

void mDNSAnnouncer::Stop() {
  if (!pImpl->serviceRef) {
    return;
  }
  DNSServiceRefDeallocate(pImpl->serviceRef);
  pImpl->serviceRef = nullptr;
}
