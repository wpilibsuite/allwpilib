// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/MulticastServiceAnnouncer.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/mutex.h>

#include "AvahiClient.h"

using namespace wpi;

struct MulticastServiceAnnouncer::Impl {
  AvahiFunctionTable& table = AvahiFunctionTable::Get();
  std::shared_ptr<AvahiThread> thread = AvahiThread::Get();
  AvahiClient* client = nullptr;
  AvahiEntryGroup* group = nullptr;
  std::string serviceName;
  std::string serviceType;
  int port;
  AvahiStringList* stringList = nullptr;

  ~Impl() noexcept {
    if (stringList != nullptr && table.IsValid()) {
      table.string_list_free(stringList);
    }
  }

  template <typename T>
  Impl(std::string_view serviceName, std::string_view serviceType, int port,
       std::span<const std::pair<T, T>> txt);
};

template <typename T>
MulticastServiceAnnouncer::Impl::Impl(std::string_view serviceName,
                                      std::string_view serviceType, int port,
                                      std::span<const std::pair<T, T>> txt) {
  if (!this->table.IsValid()) {
    return;
  }

  this->serviceName = serviceName;
  this->serviceType = serviceType;
  this->port = port;

  if (txt.empty()) {
    this->stringList = nullptr;
  } else {
    std::vector<std::string> txts;
    for (auto&& i : txt) {
      txts.push_back(fmt::format("{}={}", i.first, i.second));
    }

    std::vector<const char*> txtArr;
    for (auto&& i : txts) {
      txtArr.push_back(i.c_str());
    }

    this->stringList =
        this->table.string_list_new_from_array(txtArr.data(), txtArr.size());
  }
}

static void RegisterService(AvahiClient* client,
                            MulticastServiceAnnouncer::Impl* impl);

static void EntryGroupCallback(AvahiEntryGroup* group,
                               AvahiEntryGroupState state, void* userdata) {
  if (state == AVAHI_ENTRY_GROUP_COLLISION) {
    // Remote collision
    MulticastServiceAnnouncer::Impl* impl =
        reinterpret_cast<MulticastServiceAnnouncer::Impl*>(userdata);
    char* newName =
        impl->table.alternative_service_name(impl->serviceName.c_str());
    impl->serviceName = newName;
    impl->table.free(newName);
    RegisterService(impl->table.entry_group_get_client(group), impl);
  }
}

static void RegisterService(AvahiClient* client,
                            MulticastServiceAnnouncer::Impl* impl) {
  if (impl->group == nullptr) {
    impl->group = impl->table.entry_group_new(client, EntryGroupCallback, impl);
  }

  while (true) {
    if (impl->table.entry_group_is_empty(impl->group)) {
      int ret = 0;
      if (impl->stringList == nullptr) {
        ret = impl->table.entry_group_add_service(
            impl->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
            AVAHI_PUBLISH_USE_MULTICAST, impl->serviceName.c_str(),
            impl->serviceType.c_str(), "local", nullptr, impl->port, nullptr);
      } else {
        ret = impl->table.entry_group_add_service_strlst(
            impl->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
            AVAHI_PUBLISH_USE_MULTICAST, impl->serviceName.c_str(),
            impl->serviceType.c_str(), "local", nullptr, impl->port,
            impl->stringList);
      }
      if (ret == AVAHI_ERR_COLLISION) {
        // Local collision
        char* newName =
            impl->table.alternative_service_name(impl->serviceName.c_str());
        impl->serviceName = newName;
        impl->table.free(newName);
        continue;
      } else if (ret != AVAHI_OK) {
        break;
      }
      impl->table.entry_group_commit(impl->group);
      break;
    }
  }
}

static void ClientCallback(AvahiClient* client, AvahiClientState state,
                           void* userdata) {
  MulticastServiceAnnouncer::Impl* impl =
      reinterpret_cast<MulticastServiceAnnouncer::Impl*>(userdata);

  if (state == AVAHI_CLIENT_S_RUNNING) {
    RegisterService(client, impl);
  } else if (state == AVAHI_CLIENT_S_COLLISION ||
             state == AVAHI_CLIENT_S_REGISTERING) {
    if (impl->group) {
      impl->table.entry_group_reset(impl->group);
    }
  }
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port) {
  std::span<const std::pair<std::string_view, std::string_view>> txt;
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    std::span<const std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::MulticastServiceAnnouncer(
    std::string_view serviceName, std::string_view serviceType, int port,
    std::span<const std::pair<std::string_view, std::string_view>> txt) {
  pImpl = std::make_unique<Impl>(serviceName, serviceType, port, txt);
}

MulticastServiceAnnouncer::~MulticastServiceAnnouncer() noexcept {
  Stop();
}

bool MulticastServiceAnnouncer::HasImplementation() const {
  return pImpl->table.IsValid();
}

void MulticastServiceAnnouncer::Start() {
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

void MulticastServiceAnnouncer::Stop() {
  if (!pImpl->table.IsValid()) {
    return;
  }
  std::scoped_lock lock{*pImpl->thread};
  if (pImpl->client) {
    if (pImpl->group) {
      pImpl->table.entry_group_free(pImpl->group);
      pImpl->group = nullptr;
    }
    pImpl->table.client_free(pImpl->client);
    pImpl->client = nullptr;
  }
}
