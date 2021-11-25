// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/mDNSResolver.h"

#include <netinet/in.h>
#include <poll.h>

#include <atomic>
#include <thread>
#include <vector>

#include "dns_sd.h"

using namespace wpi;

struct DnsResolveState {
  DnsResolveState(mDNSResolver::Impl* impl, std::string_view serviceNameView)
      : pImpl{impl}, serviceName{serviceNameView} {}
  ~DnsResolveState() {
    if (ResolveRef != nullptr) {
      DNSServiceRefDeallocate(ResolveRef);
    }
  }

  DNSServiceRef ResolveRef = nullptr;
  dnssd_sock_t ResolveSocket;
  mDNSResolver::Impl* pImpl;

  std::string serviceName;
  std::vector<std::pair<std::string, std::string>> txts;
};

struct mDNSResolver::Impl {
  std::string serviceType;
  mDnsRevolveCompletionFunc onFound;
  DNSServiceRef ServiceRef = nullptr;
  dnssd_sock_t ServiceQuerySocket;
  std::vector<std::unique_ptr<DnsResolveState>> ResolveStates;
  std::thread Thread;
  std::atomic_bool running;

  void ThreadMain() {
    std::vector<pollfd> readSockets;
    std::vector<DNSServiceRef> serviceRefs;
    while (true) {
      readSockets.clear();
      serviceRefs.clear();

      readSockets.emplace_back(pollfd{ServiceQuerySocket, POLLIN, 0});
      serviceRefs.emplace_back(ServiceRef);

      for (auto&& i : ResolveStates) {
        readSockets.emplace_back(pollfd{i->ResolveSocket, POLLIN, 0});
        serviceRefs.emplace_back(i->ResolveRef);
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
          break;
        }
      } else {
        break;
      }
    }
    ResolveStates.clear();
    DNSServiceRefDeallocate(ServiceRef);
  }
};

mDNSResolver::mDNSResolver(std::string_view serviceType,
                           mDnsRevolveCompletionFunc onFound) {
  pImpl = std::make_unique<Impl>();
  pImpl->serviceType = serviceType;
  pImpl->onFound = std::move(onFound);
}

mDNSResolver::~mDNSResolver() noexcept {
  Stop();
}

void ServiceGetAddrInfoReply(DNSServiceRef sdRef, DNSServiceFlags flags,
                             uint32_t interfaceIndex,
                             DNSServiceErrorType errorCode,
                             const char* hostname,
                             const struct sockaddr* address, uint32_t ttl,
                             void* context) {
  if (errorCode != kDNSServiceErr_NoError) {
    return;
  }

  DnsResolveState* resolveState = static_cast<DnsResolveState*>(context);

  resolveState->pImpl->onFound(
      reinterpret_cast<const struct sockaddr_in*>(address)->sin_addr.s_addr,
      resolveState->serviceName, hostname, resolveState->txts);

  resolveState->pImpl->ResolveStates.erase(std::find_if(
      resolveState->pImpl->ResolveStates.begin(),
      resolveState->pImpl->ResolveStates.end(),
      [resolveState](auto& a) { return a.get() == resolveState; }));
}

void ServiceResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags,
                         uint32_t interfaceIndex, DNSServiceErrorType errorCode,
                         const char* fullname, const char* hosttarget,
                         uint16_t port, /* In network byte order */
                         uint16_t txtLen, const unsigned char* txtRecord,
                         void* context) {
  if (errorCode != kDNSServiceErr_NoError) {
    return;
  }

  DnsResolveState* resolveState = static_cast<DnsResolveState*>(context);
  DNSServiceRefDeallocate(resolveState->ResolveRef);
  resolveState->ResolveRef = nullptr;
  resolveState->ResolveSocket = 0;

  int txtCount = TXTRecordGetCount(txtLen, txtRecord);
  char keyBuf[256];
  uint8_t valueLen;
  const void* value;

  for (int i = 0; i < txtCount; i++) {
    errorCode = TXTRecordGetItemAtIndex(txtLen, txtRecord, i, sizeof(keyBuf),
                                        keyBuf, &valueLen, &value);
    if (errorCode == kDNSServiceErr_NoError) {
      if (valueLen == 0) {
        // No value
        resolveState->txts.emplace_back(
            std::pair<std::string, std::string>{std::string{keyBuf}, {}});
      } else {
        resolveState->txts.emplace_back(std::pair<std::string, std::string>{
            std::string{keyBuf},
            std::string{reinterpret_cast<const char*>(value), valueLen}});
      }
    }
  }

  errorCode = DNSServiceGetAddrInfo(
      &resolveState->ResolveRef, flags, interfaceIndex,
      kDNSServiceProtocol_IPv4, hosttarget, ServiceGetAddrInfoReply, context);

  if (errorCode == kDNSServiceErr_NoError) {
    resolveState->ResolveSocket = DNSServiceRefSockFD(resolveState->ResolveRef);
  } else {
    resolveState->pImpl->ResolveStates.erase(std::find_if(
        resolveState->pImpl->ResolveStates.begin(),
        resolveState->pImpl->ResolveStates.end(),
        [resolveState](auto& a) { return a.get() == resolveState; }));
  }
}

static void DnsCompletion(DNSServiceRef sdRef, DNSServiceFlags flags,
                          uint32_t interfaceIndex,
                          DNSServiceErrorType errorCode,
                          const char* serviceName, const char* regtype,
                          const char* replyDomain, void* context) {
  if (errorCode != kDNSServiceErr_NoError) {
    return;
  }
  if (!(flags & kDNSServiceFlagsAdd)) {
    return;
  }

  mDNSResolver::Impl* impl = static_cast<mDNSResolver::Impl*>(context);
  auto& resolveState = impl->ResolveStates.emplace_back(
      std::make_unique<DnsResolveState>(impl, serviceName));

  errorCode = DNSServiceResolve(&resolveState->ResolveRef, 0, interfaceIndex,
                                serviceName, regtype, replyDomain,
                                ServiceResolveReply, resolveState.get());

  if (errorCode == kDNSServiceErr_NoError) {
    resolveState->ResolveSocket = DNSServiceRefSockFD(resolveState->ResolveRef);
  } else {
    resolveState->pImpl->ResolveStates.erase(std::find_if(
        resolveState->pImpl->ResolveStates.begin(),
        resolveState->pImpl->ResolveStates.end(),
        [r = resolveState.get()](auto& a) { return a.get() == r; }));
  }
}

void mDNSResolver::Start() {
  if (pImpl->ServiceRef) {
    return;
  }

  DNSServiceErrorType status =
      DNSServiceBrowse(&pImpl->ServiceRef, 0, 0, pImpl->serviceType.c_str(),
                       "local", DnsCompletion, pImpl.get());
  if (status == kDNSServiceErr_NoError) {
    pImpl->ServiceQuerySocket = DNSServiceRefSockFD(pImpl->ServiceRef);
    pImpl->running = true;
    pImpl->Thread = std::thread([&] { pImpl->ThreadMain(); });
  }
}

void mDNSResolver::Stop() {
  if (!pImpl->ServiceRef) {
    return;
  }

  pImpl->running = false;
  if (pImpl->Thread.joinable()) {
    pImpl->Thread.join();
  }
}
