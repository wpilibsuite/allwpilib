#include "AvahiClient.h"
#include "dlfcn.h"
#include <wpi/mutex.h>
#include <thread>

using namespace wpi;

#define AvahiFunctionLoad(snake_name) \
  snake_name =                        \
      reinterpret_cast<snake_name##_func>(dlsym(lib, "avahi_" #snake_name))

AvahiFunctionTable::AvahiFunctionTable() {
  void* lib = dlopen("libavahi-common.so.3", RTLD_LAZY);

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

  lib = dlopen("libavahi-client.so.3", RTLD_LAZY);

  AvahiFunctionLoad(client_new);
  AvahiFunctionLoad(client_free);
  AvahiFunctionLoad(service_browser_new);
  AvahiFunctionLoad(service_browser_get_client);
  AvahiFunctionLoad(service_browser_free);
  AvahiFunctionLoad(service_resolver_new);
  AvahiFunctionLoad(service_resolver_free);
  AvahiFunctionLoad(entry_group_new);
  AvahiFunctionLoad(entry_group_free);
  AvahiFunctionLoad(entry_group_add_service_strlst);
  AvahiFunctionLoad(entry_group_reset);
  AvahiFunctionLoad(entry_group_is_empty);
  AvahiFunctionLoad(entry_group_commit);

  // TODO make this better
  valid = entry_group_add_service_strlst != nullptr;
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
  threadedPoll = table.threaded_poll_new();
  table.threaded_poll_start(threadedPoll);
}

AvahiThread::~AvahiThread() noexcept {
  table.threaded_poll_stop(threadedPoll);
  table.threaded_poll_free(threadedPoll);
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
