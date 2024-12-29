// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#if defined(__APPLE__)

#include "wpinet/MulticastServiceResolver.h"

#include <netinet/in.h>
#include <poll.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>

#include "ResolverThread.h"
#include "dns_sd.h"

using namespace wpi;

struct DnsResolveState {
  DnsResolveState(MulticastServiceResolver::Impl* impl,
                  std::string_view serviceNameView)
      : pImpl{impl} {
    data.serviceName = serviceNameView;
  }

  DNSServiceRef ResolveRef = nullptr;
  MulticastServiceResolver::Impl* pImpl;

  MulticastServiceResolver::ServiceData data;
};

struct MulticastServiceResolver::Impl {
  std::string serviceType;
  MulticastServiceResolver* resolver;
  std::shared_ptr<ResolverThread> thread = ResolverThread::Get();
  std::vector<std::unique_ptr<DnsResolveState>> ResolveStates;
  DNSServiceRef serviceRef = nullptr;

  void onFound(ServiceData&& data) {
    resolver->PushData(std::forward<ServiceData>(data));
  }
};

MulticastServiceResolver::MulticastServiceResolver(
    std::string_view serviceType) {
  pImpl = std::make_unique<Impl>();
  pImpl->serviceType = serviceType;
  pImpl->resolver = this;
}

MulticastServiceResolver::~MulticastServiceResolver() noexcept {
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

  resolveState->data.hostName = hostname;
  resolveState->data.ipv4Address =
      reinterpret_cast<const struct sockaddr_in*>(address)->sin_addr.s_addr;

  resolveState->pImpl->onFound(std::move(resolveState->data));

  resolveState->pImpl->thread->RemoveServiceRefInThread(
      resolveState->ResolveRef);

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
  resolveState->pImpl->thread->RemoveServiceRefInThread(
      resolveState->ResolveRef);
  DNSServiceRefDeallocate(resolveState->ResolveRef);
  resolveState->ResolveRef = nullptr;
  resolveState->data.port = ntohs(port);

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
        resolveState->data.txt.emplace_back(
            std::pair<std::string, std::string>{std::string{keyBuf}, {}});
      } else {
        resolveState->data.txt.emplace_back(std::pair<std::string, std::string>{
            std::string{keyBuf},
            std::string{reinterpret_cast<const char*>(value), valueLen}});
      }
    }
  }

  errorCode = DNSServiceGetAddrInfo(
      &resolveState->ResolveRef, flags, interfaceIndex,
      kDNSServiceProtocol_IPv4, hosttarget, ServiceGetAddrInfoReply, context);

  if (errorCode == kDNSServiceErr_NoError) {
    dnssd_sock_t socket = DNSServiceRefSockFD(resolveState->ResolveRef);
    resolveState->pImpl->thread->AddServiceRef(resolveState->ResolveRef,
                                               socket);
  } else {
    resolveState->pImpl->thread->RemoveServiceRefInThread(
        resolveState->ResolveRef);
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

  MulticastServiceResolver::Impl* impl =
      static_cast<MulticastServiceResolver::Impl*>(context);
  auto& resolveState = impl->ResolveStates.emplace_back(
      std::make_unique<DnsResolveState>(impl, serviceName));

  errorCode = DNSServiceResolve(&resolveState->ResolveRef, 0, interfaceIndex,
                                serviceName, regtype, replyDomain,
                                ServiceResolveReply, resolveState.get());

  if (errorCode == kDNSServiceErr_NoError) {
    dnssd_sock_t socket = DNSServiceRefSockFD(resolveState->ResolveRef);
    resolveState->pImpl->thread->AddServiceRef(resolveState->ResolveRef,
                                               socket);
  } else {
    resolveState->pImpl->ResolveStates.erase(std::find_if(
        resolveState->pImpl->ResolveStates.begin(),
        resolveState->pImpl->ResolveStates.end(),
        [r = resolveState.get()](auto& a) { return a.get() == r; }));
  }
}

bool MulticastServiceResolver::HasImplementation() const {
  return true;
}

void MulticastServiceResolver::Start() {
  if (pImpl->serviceRef) {
    return;
  }

  DNSServiceErrorType status =
      DNSServiceBrowse(&pImpl->serviceRef, 0, 0, pImpl->serviceType.c_str(),
                       "local", DnsCompletion, pImpl.get());
  if (status == kDNSServiceErr_NoError) {
    dnssd_sock_t socket = DNSServiceRefSockFD(pImpl->serviceRef);
    pImpl->thread->AddServiceRef(pImpl->serviceRef, socket);
  }
}

void MulticastServiceResolver::Stop() {
  if (!pImpl->serviceRef) {
    return;
  }
  wpi::SmallVector<WPI_EventHandle, 8> cleanupEvents;
  for (auto&& i : pImpl->ResolveStates) {
    cleanupEvents.push_back(
        pImpl->thread->RemoveServiceRefOutsideThread(i->ResolveRef));
  }
  cleanupEvents.push_back(
      pImpl->thread->RemoveServiceRefOutsideThread(pImpl->serviceRef));
  wpi::SmallVector<WPI_Handle, 8> signaledBuf;
  signaledBuf.resize(cleanupEvents.size());
  while (!cleanupEvents.empty()) {
    auto signaled = wpi::WaitForObjects(cleanupEvents, signaledBuf);
    for (auto&& s : signaled) {
      cleanupEvents.erase(
          std::find(cleanupEvents.begin(), cleanupEvents.end(), s));
    }
  }

  pImpl->ResolveStates.clear();
  pImpl->serviceRef = nullptr;
}

#endif  // defined(__APPLE__)
