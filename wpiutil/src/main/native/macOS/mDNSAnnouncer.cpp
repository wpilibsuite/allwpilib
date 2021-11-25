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
  TXTRecordRef txtRecord;

  Impl() {
    TXTRecordCreate(&txtRecord, 0, nullptr);
  }

  ~Impl() noexcept {
    TXTRecordDeallocate(&txtRecord);
  }
};

mDNSAnnouncer::mDNSAnnouncer(
    std::string_view serviceName, std::string_view serviceType,
    wpi::span<std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>();
  pImpl->serviceName = serviceName;
  pImpl->serviceType = serviceType;

  for (auto&& i : txt) {
    TXTRecordSetValue(&pImpl->txtRecord, i.first.c_str(), i.second.length(), i.second.c_str());
  }
}

mDNSAnnouncer::~mDNSAnnouncer() noexcept {
  Stop();
}

void mDNSAnnouncer::Start() {
  if (pImpl->serviceRef) {
    return;
  }

  uint16_t len = TXTRecordGetLength(&pImpl->txtRecord);
  const void* ptr = TXTRecordGetBytesPtr(&pImpl->txtRecord);

  (void)DNSServiceRegister(&pImpl->serviceRef, 0, 0, pImpl->serviceName.c_str(),
                           pImpl->serviceType.c_str(), "local", nullptr, htons(5000),
                           len, ptr, nullptr, nullptr);
}

void mDNSAnnouncer::Stop() {
  if (!pImpl->serviceRef) {
    return;
  }
  DNSServiceRefDeallocate(pImpl->serviceRef);
  pImpl->serviceRef = nullptr;
}
