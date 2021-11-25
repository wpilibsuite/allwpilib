#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "wpi/span.h"

namespace wpi {
class mDNSAnnouncer {
 public:
  mDNSAnnouncer(std::string_view serviceName, std::string_view serviceType,
                std::string_view machineName, wpi::span<std::pair<std::string, std::string>> txt);
  ~mDNSAnnouncer() noexcept;

  void Start();
  void Stop();

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
