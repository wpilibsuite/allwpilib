// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/UsbDeviceDetector.hpp"

#include <stdexcept>
#include <utility>

#include "UsbDeviceDetectorImpl.hpp"

using namespace wpi::net;

UsbDeviceDetector::UsbDeviceDetector(int port, Callback connected,
                                     Callback disconnected) {
  if (port < 0 || port > 3) {
    throw std::out_of_range{"USB port must be in the range 0-3"};
  }
  m_impl = CreateImpl(port, std::move(connected), std::move(disconnected));
}

UsbDeviceDetector::~UsbDeviceDetector() {
  Stop();
}

bool UsbDeviceDetector::Start() {
  return m_impl && m_impl->Start();
}

void UsbDeviceDetector::Stop() {
  if (m_impl) {
    m_impl->Stop();
  }
}

bool UsbDeviceDetector::IsRunning() const {
  return m_impl && m_impl->IsRunning();
}

bool UsbDeviceDetector::HasImplementation() const {
  return m_impl != nullptr;
}

std::optional<int> wpi::net::detail::GetSystemCoreUsbPort(
    std::string_view sysname) {
  constexpr std::string_view prefix = "3-1.";
  if (!sysname.starts_with(prefix) || sysname.size() <= prefix.size()) {
    return std::nullopt;
  }

  char portChar = sysname[prefix.size()];
  if (portChar < '1' || portChar > '4') {
    return std::nullopt;
  }
  if (sysname.size() > prefix.size() + 1 && sysname[prefix.size() + 1] != '.') {
    return std::nullopt;
  }
  return portChar - '1';
}

#if !defined(__FIRST_SYSTEMCORE__)
std::unique_ptr<UsbDeviceDetector::Impl> UsbDeviceDetector::CreateImpl(
    int, Callback, Callback) {
  return nullptr;
}
#endif
