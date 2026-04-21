// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#if defined(__APPLE__)

#include "wpi/net/MulticastServiceResolver.h"

#include <netinet/in.h>
#include <poll.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "dns_sd.h"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::net;

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
  dispatch_queue_t queue;
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
  DnsResolveState* resolveState = static_cast<DnsResolveState*>(context);
  if (errorCode == kDNSServiceErr_NoError) {
    resolveState->data.hostName = hostname;
    resolveState->data.ipv4Address = ntohl(
        reinterpret_cast<const struct sockaddr_in*>(address)->sin_addr.s_addr);

    resolveState->pImpl->onFound(std::move(resolveState->data));
  }

  DNSServiceRefDeallocate(resolveState->ResolveRef);

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
  DnsResolveState* resolveState = static_cast<DnsResolveState*>(context);
  DNSServiceRefDeallocate(resolveState->ResolveRef);
  resolveState->ResolveRef = nullptr;
  if (errorCode != kDNSServiceErr_NoError) {
    resolveState->pImpl->ResolveStates.erase(std::find_if(
        resolveState->pImpl->ResolveStates.begin(),
        resolveState->pImpl->ResolveStates.end(),
        [resolveState](auto& a) { return a.get() == resolveState; }));
  }

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
    errorCode = DNSServiceSetDispatchQueue(resolveState->ResolveRef,
                                           resolveState->pImpl->queue);
  } else {
    resolveState->ResolveRef = nullptr;
  }

  if (errorCode != kDNSServiceErr_NoError) {
    if (resolveState->ResolveRef) {
      DNSServiceRefDeallocate(resolveState->ResolveRef);
    }
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
    errorCode =
        DNSServiceSetDispatchQueue(resolveState->ResolveRef, impl->queue);
  } else {
    resolveState->ResolveRef = nullptr;
  }

  if (errorCode != kDNSServiceErr_NoError) {
    if (resolveState->ResolveRef) {
      DNSServiceRefDeallocate(resolveState->ResolveRef);
    }
    resolveState->pImpl->ResolveStates.erase(std::find_if(
        resolveState->pImpl->ResolveStates.begin(),
        resolveState->pImpl->ResolveStates.end(),
        [r = resolveState.get()](auto& a) { return a.get() == r; }));
  }
}

bool MulticastServiceResolver::HasImplementation() const {
  return true;
}

bool MulticastServiceResolver::SetCopyCallback(
    std::function<bool(const ServiceData&)> callback) {
  if (pImpl->serviceRef) {
    return false;
  }
  copyCallback = std::move(callback);
  return true;
}

bool MulticastServiceResolver::SetMoveCallback(
    std::function<void(ServiceData&&)> callback) {
  if (pImpl->serviceRef) {
    return false;
  }
  moveCallback = std::move(callback);
  return true;
}

void MulticastServiceResolver::Start() {
  if (pImpl->serviceRef) {
    return;
  }

  pImpl->queue = dispatch_queue_create(nullptr, DISPATCH_QUEUE_SERIAL);

  DNSServiceErrorType status =
      DNSServiceBrowse(&pImpl->serviceRef, 0, 0, pImpl->serviceType.c_str(),
                       "local", DnsCompletion, pImpl.get());
  if (status == kDNSServiceErr_NoError) {
    status = DNSServiceSetDispatchQueue(pImpl->serviceRef, pImpl->queue);
    if (status != kDNSServiceErr_NoError) {
      DNSServiceRefDeallocate(pImpl->serviceRef);
      pImpl->serviceRef = nullptr;
      dispatch_release(pImpl->queue);
      pImpl->queue = nullptr;
    }
  }
}

void MulticastServiceResolver::Stop() {
  if (!pImpl->serviceRef) {
    return;
  }

  dispatch_sync_f(pImpl->queue, pImpl.get(), [](void* context) {
    MulticastServiceResolver::Impl* impl =
        static_cast<MulticastServiceResolver::Impl*>(context);
    DNSServiceRefDeallocate(impl->serviceRef);
    impl->serviceRef = nullptr;

    for (auto&& i : impl->ResolveStates) {
      if (i->ResolveRef) {
        DNSServiceRefDeallocate(i->ResolveRef);
        i->ResolveRef = nullptr;
      }
    }
    impl->ResolveStates.clear();
  });

  dispatch_release(pImpl->queue);
  pImpl->queue = nullptr;
}

#endif  // defined(__APPLE__)
