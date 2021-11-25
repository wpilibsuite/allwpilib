// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/MulticastServiceAnnouncer.h"

#include <vector>

#include "AvahiClient.h"
#include "fmt/format.h"
#include "wpi/mutex.h"

using namespace wpi;

struct MulticastServiceAnnouncer::Impl {
  AvahiFunctionTable& table = AvahiFunctionTable::Get();
  std::shared_ptr<AvahiThread> thread = AvahiThread::Get();
  AvahiClient* client;
  AvahiEntryGroup* group = nullptr;
  std::string serviceName;
  std::string serviceType;
  int port;
  AvahiStringList* stringList = nullptr;

  ~Impl() noexcept {
    if (stringList != nullptr) {
      table.string_list_free(stringList);
    }
  }
};

static void EntryGroupCallback(AvahiEntryGroup*, AvahiEntryGroupState, void*) {}

static void RegisterService(AvahiClient* client, MulticastServiceAnnouncer::Impl* impl) {
  if (impl->group == nullptr) {
    impl->group =
        impl->table.entry_group_new(client, EntryGroupCallback, nullptr);
  }

  if (impl->table.entry_group_is_empty(impl->group)) {
    impl->table.entry_group_add_service_strlst(
        impl->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, AVAHI_PUBLISH_USE_MULTICAST,
        impl->serviceName.c_str(), impl->serviceType.c_str(), "local", nullptr,
        impl->port, impl->stringList);
    impl->table.entry_group_commit(impl->group);
  }
}

static void ClientCallback(AvahiClient* client, AvahiClientState state,
                           void* userdata) {
  MulticastServiceAnnouncer::Impl* impl = reinterpret_cast<MulticastServiceAnnouncer::Impl*>(userdata);

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
    std::string_view serviceName, std::string_view serviceType, int port,
    wpi::span<const std::pair<std::string, std::string>> txt) {
  pImpl = std::make_unique<Impl>();

  if (!pImpl->table.IsValid()) {
    return;
  }

  pImpl->serviceName = serviceName;
  pImpl->serviceType = serviceType;
  pImpl->port = port;

  std::vector<std::string> txts;
  for (auto&& i : txt) {
    txts.push_back(fmt::format("{}={}", i.first, i.second));
  }

  std::vector<const char*> txtArr;
  for (auto&& i : txts) {
    txtArr.push_back(i.c_str());
  }

  pImpl->stringList =
      pImpl->table.string_list_new_from_array(txtArr.data(), txtArr.size());
}

MulticastServiceAnnouncer::~MulticastServiceAnnouncer() noexcept {
  Stop();
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
