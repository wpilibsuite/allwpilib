// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#if defined(__APPLE__)

#include "ResolverThread.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <wpi/mutex.h>

using namespace wpi;

ResolverThread::ResolverThread(const private_init&) {}

ResolverThread::~ResolverThread() noexcept {
  running = false;
  if (thread.joinable()) {
    thread.join();
  }
}

void ResolverThread::AddServiceRef(DNSServiceRef serviceRef,
                                   dnssd_sock_t socket) {
  std::scoped_lock lock{serviceRefMutex};
  serviceRefs.emplace_back(
      std::pair<DNSServiceRef, dnssd_sock_t>{serviceRef, socket});
  if (serviceRefs.size() == 1) {
    running = false;
    if (thread.joinable()) {
      thread.join();
    }
    running = true;
    thread = std::thread([this] { ThreadMain(); });
  }
}

void ResolverThread::RemoveServiceRefInThread(DNSServiceRef serviceRef) {
  std::scoped_lock lock{serviceRefMutex};
  serviceRefs.erase(
      std::find_if(serviceRefs.begin(), serviceRefs.end(),
                   [=](auto& a) { return a.first == serviceRef; }));
  DNSServiceRefDeallocate(serviceRef);
}

WPI_EventHandle ResolverThread::RemoveServiceRefOutsideThread(
    DNSServiceRef serviceRef) {
  std::scoped_lock lock{serviceRefMutex};
  WPI_EventHandle handle = CreateEvent(true);
  serviceRefsToRemove.push_back({serviceRef, handle});
  return handle;
}

bool ResolverThread::CleanupRefs() {
  std::scoped_lock lock{serviceRefMutex};
  for (auto&& r : serviceRefsToRemove) {
    serviceRefs.erase(
        std::find_if(serviceRefs.begin(), serviceRefs.end(),
                     [=](auto& a) { return a.first == r.first; }));
    DNSServiceRefDeallocate(r.first);
    wpi::SetEvent(r.second);
  }
  serviceRefsToRemove.clear();
  return serviceRefs.empty();
}

void ResolverThread::ThreadMain() {
  std::vector<pollfd> readSockets;
  std::vector<DNSServiceRef> serviceRefs;

  while (running) {
    readSockets.clear();
    serviceRefs.clear();

    for (auto&& i : this->serviceRefs) {
      readSockets.emplace_back(pollfd{i.second, POLLIN, 0});
      serviceRefs.emplace_back(i.first);
    }

    int res = poll(readSockets.begin().base(), readSockets.size(), 100);

    if (res > 0) {
      for (size_t i = 0; i < readSockets.size(); i++) {
        if (readSockets[i].revents == POLLIN) {
          DNSServiceProcessResult(serviceRefs[i]);
        }
      }
    } else if (res == 0) {
      if (!running) {
        CleanupRefs();
        break;
      }
    }

    if (CleanupRefs()) {
      break;
    }
  }
}

static wpi::mutex ThreadLoopLock;
static std::weak_ptr<ResolverThread> ThreadLoop;

std::shared_ptr<ResolverThread> ResolverThread::Get() {
  std::scoped_lock lock{ThreadLoopLock};
  auto locked = ThreadLoop.lock();
  if (!locked) {
    locked = std::make_unique<ResolverThread>(private_init{});
    ThreadLoop = locked;
  }
  return locked;
}

#endif  // defined(__APPLE__)
