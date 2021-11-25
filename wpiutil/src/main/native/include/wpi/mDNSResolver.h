#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <wpi/span.h>

namespace wpi {
class mDNSResolver {
public:
  typedef
  void
  mDnsRevolveCompletion(
    unsigned int ipv4Address,
    std::string_view name,
    wpi::span<std::pair<std::string, std::string>> txt
  );
  typedef mDnsRevolveCompletion *mDnsRevolveCompletionFunc;

  mDNSResolver(std::string_view serviceType, mDnsRevolveCompletionFunc onFound);
  ~mDNSResolver() noexcept;

  void Start();
  void Stop();

  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};
}
