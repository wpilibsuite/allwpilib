// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DBusRuntime.hpp"

#include <dlfcn.h>

#include <format>
#include <mutex>
#include <string>

namespace wpi::net::linuxbluetooth {

const DBusApi* DBusApi::Get(std::string* error) {
  static std::mutex mutex;
  static DBusApi api;
  static bool loaded = false;
  static bool attempted = false;
  static std::string loadError;

  std::scoped_lock lock{mutex};
  if (!attempted) {
    attempted = true;
    loaded = api.Load(&loadError);
  }

  if (!loaded) {
    if (error != nullptr) {
      *error = loadError;
    }
    return nullptr;
  }
  return &api;
}

DBusApi::~DBusApi() {
  if (m_handle != nullptr) {
    dlclose(m_handle);
  }
}

void* DBusApi::LoadSymbol(const char* name, std::string* error) {
  dlerror();
  void* symbol = dlsym(m_handle, name);
  const char* dlsymError = dlerror();
  if (dlsymError != nullptr) {
    *error =
        std::format("Failed to load {} from libdbus: {}", name, dlsymError);
    return nullptr;
  }
  if (symbol == nullptr) {
    *error = std::format("Failed to load {} from libdbus", name);
  }
  return symbol;
}

bool DBusApi::Load(std::string* error) {
  m_handle = dlopen("libdbus-1.so.3", RTLD_NOW | RTLD_LOCAL);
  if (m_handle == nullptr) {
    m_handle = dlopen("libdbus-1.so", RTLD_NOW | RTLD_LOCAL);
  }
  if (m_handle == nullptr) {
    *error = std::format("Failed to load libdbus-1: {}", dlerror());
    return false;
  }

#define LOAD_DBUS_SYMBOL(symbol)                                             \
  do {                                                                       \
    symbol = reinterpret_cast<decltype(symbol)>(LoadSymbol(#symbol, error)); \
    if (symbol == nullptr) {                                                 \
      return false;                                                          \
    }                                                                        \
  } while (false)

  LOAD_DBUS_SYMBOL(dbus_error_init);
  LOAD_DBUS_SYMBOL(dbus_error_free);
  LOAD_DBUS_SYMBOL(dbus_error_is_set);
  LOAD_DBUS_SYMBOL(dbus_bus_get_private);
  LOAD_DBUS_SYMBOL(dbus_connection_close);
  LOAD_DBUS_SYMBOL(dbus_connection_flush);
  LOAD_DBUS_SYMBOL(dbus_connection_read_write_dispatch);
  LOAD_DBUS_SYMBOL(dbus_connection_register_object_path);
  LOAD_DBUS_SYMBOL(dbus_connection_send);
  LOAD_DBUS_SYMBOL(dbus_connection_send_with_reply);
  LOAD_DBUS_SYMBOL(dbus_connection_send_with_reply_and_block);
  LOAD_DBUS_SYMBOL(dbus_connection_set_exit_on_disconnect);
  LOAD_DBUS_SYMBOL(dbus_connection_unref);
  LOAD_DBUS_SYMBOL(dbus_connection_unregister_object_path);
  LOAD_DBUS_SYMBOL(dbus_message_get_error_name);
  LOAD_DBUS_SYMBOL(dbus_message_get_interface);
  LOAD_DBUS_SYMBOL(dbus_message_get_member);
  LOAD_DBUS_SYMBOL(dbus_message_get_type);
  LOAD_DBUS_SYMBOL(dbus_message_new_error);
  LOAD_DBUS_SYMBOL(dbus_message_new_method_call);
  LOAD_DBUS_SYMBOL(dbus_message_new_method_return);
  LOAD_DBUS_SYMBOL(dbus_message_unref);
  LOAD_DBUS_SYMBOL(dbus_message_iter_append_basic);
  LOAD_DBUS_SYMBOL(dbus_message_iter_close_container);
  LOAD_DBUS_SYMBOL(dbus_message_iter_get_arg_type);
  LOAD_DBUS_SYMBOL(dbus_message_iter_get_basic);
  LOAD_DBUS_SYMBOL(dbus_message_iter_init);
  LOAD_DBUS_SYMBOL(dbus_message_iter_init_append);
  LOAD_DBUS_SYMBOL(dbus_message_iter_next);
  LOAD_DBUS_SYMBOL(dbus_message_iter_open_container);
  LOAD_DBUS_SYMBOL(dbus_message_iter_recurse);
  LOAD_DBUS_SYMBOL(dbus_pending_call_get_completed);
  LOAD_DBUS_SYMBOL(dbus_pending_call_steal_reply);
  LOAD_DBUS_SYMBOL(dbus_pending_call_unref);

#undef LOAD_DBUS_SYMBOL

  return true;
}

const DBusApi& DBus() {
  return *DBusApi::Get();
}

}  // namespace wpi::net::linuxbluetooth
