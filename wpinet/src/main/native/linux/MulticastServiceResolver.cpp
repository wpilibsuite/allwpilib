// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/MulticastServiceResolver.h"

#include <memory>
#include <string>
#include <utility>

#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/mutex.h>

#include "AvahiClient.h"

using namespace wpi;

struct MulticastServiceResolver::Impl {
  AvahiFunctionTable& table = AvahiFunctionTable::Get();
  std::shared_ptr<AvahiThread> thread = AvahiThread::Get();
  AvahiClient* client;
  AvahiServiceBrowser* browser;
  std::string serviceType;
  MulticastServiceResolver* resolver;

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

bool MulticastServiceResolver::HasImplementation() const {
  return pImpl->table.IsValid();
}

static void ResolveCallback(AvahiServiceResolver* r, AvahiIfIndex interface,
                            AvahiProtocol protocol, AvahiResolverEvent event,
                            const char* name, const char* type,
                            const char* domain, const char* host_name,
                            const AvahiAddress* address, uint16_t port,
                            AvahiStringList* txt, AvahiLookupResultFlags flags,
                            void* userdata) {
  MulticastServiceResolver::Impl* impl =
      reinterpret_cast<MulticastServiceResolver::Impl*>(userdata);

  if (event == AVAHI_RESOLVER_FOUND) {
    if (address->proto == AVAHI_PROTO_INET) {
      AvahiStringList* strLst = txt;
      MulticastServiceResolver::ServiceData data;
      while (strLst != nullptr) {
        std::string_view value{reinterpret_cast<const char*>(strLst->text),
                               strLst->size};
        strLst = strLst->next;
        size_t splitIndex = value.find('=');
        if (splitIndex == value.npos) {
          // Todo make this just do key
          continue;
        }
        std::string_view key = wpi::substr(value, 0, splitIndex);
        value =
            wpi::substr(value, splitIndex + 1, value.size() - splitIndex - 1);
        data.txt.emplace_back(std::pair<std::string, std::string>{key, value});
      }
      wpi::SmallString<256> outputHostName;
      char label[256];
      do {
        impl->table.unescape_label(&host_name, label, sizeof(label));
        if (label[0] == '\0') {
          break;
        }
        outputHostName.append(label);
        outputHostName.append(".");
      } while (true);

      data.ipv4Address = address->data.ipv4.address;
      data.port = port;
      data.serviceName = name;
      data.hostName = std::string{outputHostName};

      impl->onFound(std::move(data));
    }
  }

  impl->table.service_resolver_free(r);
}

static void BrowseCallback(AvahiServiceBrowser* b, AvahiIfIndex interface,
                           AvahiProtocol protocol, AvahiBrowserEvent event,
                           const char* name, const char* type,
                           const char* domain, AvahiLookupResultFlags flags,
                           void* userdata) {
  MulticastServiceResolver::Impl* impl =
      reinterpret_cast<MulticastServiceResolver::Impl*>(userdata);

  if (event == AVAHI_BROWSER_NEW) {
    impl->table.service_resolver_new(
        impl->table.service_browser_get_client(b), interface, protocol, name,
        type, domain, AVAHI_PROTO_UNSPEC, AVAHI_LOOKUP_USE_MULTICAST,
        ResolveCallback, userdata);
  }
}

static void ClientCallback(AvahiClient* client, AvahiClientState state,
                           void* userdata) {
  MulticastServiceResolver::Impl* impl =
      reinterpret_cast<MulticastServiceResolver::Impl*>(userdata);

  if (state == AVAHI_CLIENT_S_RUNNING) {
    impl->browser = impl->table.service_browser_new(
        client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, impl->serviceType.c_str(),
        "local", AvahiLookupFlags::AVAHI_LOOKUP_USE_MULTICAST, BrowseCallback,
        userdata);
  }
}

void MulticastServiceResolver::Start() {
  if (!pImpl->table.IsValid()) {
    return;
  }
  std::scoped_lock lock{*pImpl->thread};
  if (pImpl->client) {
    return;
  }

  pImpl->client =
      pImpl->table.client_new(pImpl->thread->GetPoll(), AVAHI_CLIENT_NO_FAIL,
                              ClientCallback, pImpl.get(), nullptr);
}

void MulticastServiceResolver::Stop() {
  if (!pImpl->table.IsValid()) {
    return;
  }
  std::scoped_lock lock{*pImpl->thread};
  if (pImpl->client) {
    if (pImpl->browser) {
      pImpl->table.service_browser_free(pImpl->browser);
      pImpl->browser = nullptr;
    }
    pImpl->table.client_free(pImpl->client);
    pImpl->client = nullptr;
  }
}
