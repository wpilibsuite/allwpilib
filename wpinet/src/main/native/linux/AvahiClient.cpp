// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AvahiClient.h"

#include <wpi/mutex.h>

#include <thread>

#include "dlfcn.h"

using namespace wpi;

#define AvahiFunctionLoad(snake_name)                                          \
  do {                                                                         \
    snake_name =                                                               \
        reinterpret_cast<snake_name##_func>(dlsym(lib, "avahi_" #snake_name)); \
    if (!snake_name) {                                                         \
      return;                                                                  \
    }                                                                          \
  } while (false)

AvahiFunctionTable::AvahiFunctionTable() {
  void* lib = dlopen("libavahi-common.so.3", RTLD_LAZY);

  valid = false;

  if (lib == nullptr) {
    return;
  }

  AvahiFunctionLoad(threaded_poll_new);
  AvahiFunctionLoad(threaded_poll_free);
  AvahiFunctionLoad(threaded_poll_get);
  AvahiFunctionLoad(threaded_poll_start);
  AvahiFunctionLoad(threaded_poll_stop);
  AvahiFunctionLoad(threaded_poll_lock);
  AvahiFunctionLoad(threaded_poll_unlock);
  AvahiFunctionLoad(string_list_new_from_array);
  AvahiFunctionLoad(string_list_free);
  AvahiFunctionLoad(unescape_label);
  AvahiFunctionLoad(alternative_service_name);
  AvahiFunctionLoad(free);

  lib = dlopen("libavahi-client.so.3", RTLD_LAZY);

  if (lib == nullptr) {
    return;
  }

  AvahiFunctionLoad(client_new);
  AvahiFunctionLoad(client_free);
  AvahiFunctionLoad(service_browser_new);
  AvahiFunctionLoad(service_browser_get_client);
  AvahiFunctionLoad(service_browser_free);
  AvahiFunctionLoad(service_resolver_new);
  AvahiFunctionLoad(service_resolver_free);
  AvahiFunctionLoad(entry_group_new);
  AvahiFunctionLoad(entry_group_free);
  AvahiFunctionLoad(entry_group_add_service);
  AvahiFunctionLoad(entry_group_add_service_strlst);
  AvahiFunctionLoad(entry_group_reset);
  AvahiFunctionLoad(entry_group_is_empty);
  AvahiFunctionLoad(entry_group_commit);
  AvahiFunctionLoad(entry_group_get_client);

  valid = true;
}

AvahiFunctionTable& AvahiFunctionTable::Get() {
  static AvahiFunctionTable table;
  return table;
}

static wpi::mutex ThreadLoopLock;
static std::weak_ptr<AvahiThread> ThreadLoop;

std::shared_ptr<AvahiThread> AvahiThread::Get() {
  std::scoped_lock lock{ThreadLoopLock};
  auto locked = ThreadLoop.lock();
  if (!locked) {
    locked = std::make_unique<AvahiThread>(private_init{});
    ThreadLoop = locked;
  }
  return locked;
}

AvahiThread::AvahiThread(const private_init&) {
  if (!table.IsValid()) {
    return;
  }

  threadedPoll = table.threaded_poll_new();
  table.threaded_poll_start(threadedPoll);
}

AvahiThread::~AvahiThread() noexcept {
  if (!table.IsValid()) {
    return;
  }

  if (threadedPoll) {
    table.threaded_poll_stop(threadedPoll);
    table.threaded_poll_free(threadedPoll);
  }
}

void AvahiThread::lock() {
  table.threaded_poll_lock(threadedPoll);
}

void AvahiThread::unlock() {
  table.threaded_poll_unlock(threadedPoll);
}

const AvahiPoll* AvahiThread::GetPoll() const {
  return table.threaded_poll_get(threadedPoll);
}
