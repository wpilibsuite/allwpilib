// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#if defined(__APPLE__)

#pragma once

#include <netinet/in.h>
#include <poll.h>

#include <atomic>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/Synchronization.h>
#include <wpi/mutex.h>

#include "dns_sd.h"

namespace wpi {
class ResolverThread {
 private:
  struct private_init {};

 public:
  explicit ResolverThread(const private_init&);
  ~ResolverThread() noexcept;

  void AddServiceRef(DNSServiceRef serviceRef, dnssd_sock_t socket);
  void RemoveServiceRefInThread(DNSServiceRef serviceRef);
  WPI_EventHandle RemoveServiceRefOutsideThread(DNSServiceRef serviceRef);

  static std::shared_ptr<ResolverThread> Get();

 private:
  void ThreadMain();
  bool CleanupRefs();

  wpi::mutex serviceRefMutex;
  std::vector<std::pair<DNSServiceRef, WPI_EventHandle>> serviceRefsToRemove;
  std::vector<std::pair<DNSServiceRef, dnssd_sock_t>> serviceRefs;
  std::thread thread;
  std::atomic_bool running;
};
}  // namespace wpi

#endif  // defined(__APPLE__)
