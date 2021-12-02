// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/MulticastServiceAnnouncer.h"

#include <wpi/SmallString.h>

#include "dns_sd.h"

using namespace wpi;

struct MulticastServiceAnnouncer::Impl {
  std::string serviceName;
  std::string serviceType;
  int port;
  DNSServiceRef serviceRef{nullptr};
  TXTRecordRef txtRecord;

  template <typename T>
  Impl(std::string_view serviceName, std::string_view serviceType, int port,
       wpi::span<const std::pair<T, T>> txt) {}

  ~Impl() noexcept { TXTRecordDeallocate(&txtRecord); }
};

template <typename T>
MulticastServiceAnnouncer::Impl::Impl(std::string_view serviceName,
                                      std::string_view serviceType, int port,
                                      wpi::span<const std::pair<T, T>> txt) {
  this->serviceName = serviceName;
  this->serviceType = serviceType;
  this->port = port;
  TXTRecordCreate(&txtRecord, 0, nullptr);

  for (auto&& i : txt) {
    TXTRecordSetValue(&this->txtRecord, i.first.c_str(), i.second.length(),
                      i.second.c_str());
  }
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    wpi::span<const std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    wpi::span<const std::pair<std::string_view, std::string_view>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::~MulticastServiceAnnouncer() noexcept {
  Stop();
}

bool MulticastServiceAnnouncer::HasImplementation() const {
  return true;
}

void MulticastServiceAnnouncer::Start() {
  if (pImpl->serviceRef) {
    return;
  }

  uint16_t len = TXTRecordGetLength(&pImpl->txtRecord);
  const void* ptr = TXTRecordGetBytesPtr(&pImpl->txtRecord);

  (void)DNSServiceRegister(&pImpl->serviceRef, 0, 0, pImpl->serviceName.c_str(),
                           pImpl->serviceType.c_str(), "local", nullptr,
                           htons(pImpl->port), len, ptr, nullptr, nullptr);
}

void MulticastServiceAnnouncer::Stop() {
  if (!pImpl->serviceRef) {
    return;
  }
  DNSServiceRefDeallocate(pImpl->serviceRef);
  pImpl->serviceRef = nullptr;
}
