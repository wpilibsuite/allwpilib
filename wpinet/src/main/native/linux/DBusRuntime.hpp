// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace wpi::net::linuxbluetooth {

using dbus_bool_t = int;
using dbus_uint32_t = uint32_t;

struct DBusConnection;
struct DBusMessage;
struct DBusPendingCall;

enum DBusBusType {
  DBUS_BUS_SESSION,
  DBUS_BUS_SYSTEM,
  DBUS_BUS_STARTER,
};

enum DBusHandlerResult {
  DBUS_HANDLER_RESULT_HANDLED,
  DBUS_HANDLER_RESULT_NOT_YET_HANDLED,
  DBUS_HANDLER_RESULT_NEED_MEMORY,
};

struct DBusError {
  const char* name;
  const char* message;
  unsigned int dummy1 : 1;
  unsigned int dummy2 : 1;
  unsigned int dummy3 : 1;
  unsigned int dummy4 : 1;
  unsigned int dummy5 : 1;
  void* padding1;
};

struct DBusMessageIter {
  void* dummy1;
  void* dummy2;
  dbus_uint32_t dummy3;
  int dummy4;
  int dummy5;
  int dummy6;
  int dummy7;
  int dummy8;
  int dummy9;
  int dummy10;
  int dummy11;
  int pad1;
  void* pad2;
  void* pad3;
};
static_assert(sizeof(void*) <= 8,
              "DBusMessageIter layout is only defined for <= 64-bit pointers");

using DBusObjectPathUnregisterFunction = void (*)(DBusConnection* connection,
                                                  void* userData);
using DBusObjectPathMessageFunction = DBusHandlerResult (*)(
    DBusConnection* connection, DBusMessage* message, void* userData);

struct DBusObjectPathVTable {
  DBusObjectPathUnregisterFunction unregisterFunction;
  DBusObjectPathMessageFunction messageFunction;
  void (*dbusInternalPad1)(void*);
  void (*dbusInternalPad2)(void*);
  void (*dbusInternalPad3)(void*);
  void (*dbusInternalPad4)(void*);
};

constexpr int DBUS_TIMEOUT_USE_DEFAULT = -1;
constexpr int DBUS_MESSAGE_TYPE_ERROR = 3;
constexpr int DBUS_TYPE_BOOLEAN = 'b';
constexpr int DBUS_TYPE_STRING = 's';
constexpr int DBUS_TYPE_OBJECT_PATH = 'o';
constexpr int DBUS_TYPE_ARRAY = 'a';
constexpr int DBUS_TYPE_VARIANT = 'v';
constexpr int DBUS_TYPE_DICT_ENTRY = 'e';

class DBusApi {
 public:
  DBusApi(const DBusApi&) = delete;
  DBusApi& operator=(const DBusApi&) = delete;

  static const DBusApi* Get(std::string* error = nullptr);

  ~DBusApi();

  void (*dbus_error_init)(DBusError* error) = nullptr;
  void (*dbus_error_free)(DBusError* error) = nullptr;
  dbus_bool_t (*dbus_error_is_set)(const DBusError* error) = nullptr;

  DBusConnection* (*dbus_bus_get_private)(DBusBusType type,
                                          DBusError* error) = nullptr;
  void (*dbus_connection_close)(DBusConnection* connection) = nullptr;
  void (*dbus_connection_flush)(DBusConnection* connection) = nullptr;
  dbus_bool_t (*dbus_connection_read_write_dispatch)(DBusConnection* connection,
                                                     int timeoutMs) = nullptr;
  dbus_bool_t (*dbus_connection_register_object_path)(
      DBusConnection* connection, const char* path,
      const DBusObjectPathVTable* vtable, void* userData) = nullptr;
  dbus_bool_t (*dbus_connection_send)(DBusConnection* connection,
                                      DBusMessage* message,
                                      dbus_uint32_t* serial) = nullptr;
  dbus_bool_t (*dbus_connection_send_with_reply)(
      DBusConnection* connection, DBusMessage* message,
      DBusPendingCall** pendingReturn, int timeoutMs) = nullptr;
  DBusMessage* (*dbus_connection_send_with_reply_and_block)(
      DBusConnection* connection, DBusMessage* message, int timeoutMs,
      DBusError* error) = nullptr;
  void (*dbus_connection_set_exit_on_disconnect)(
      DBusConnection* connection, dbus_bool_t exitOnDisconnect) = nullptr;
  void (*dbus_connection_unref)(DBusConnection* connection) = nullptr;
  dbus_bool_t (*dbus_connection_unregister_object_path)(
      DBusConnection* connection, const char* path) = nullptr;

  const char* (*dbus_message_get_error_name)(DBusMessage* message) = nullptr;
  const char* (*dbus_message_get_interface)(DBusMessage* message) = nullptr;
  const char* (*dbus_message_get_member)(DBusMessage* message) = nullptr;
  int (*dbus_message_get_type)(DBusMessage* message) = nullptr;
  DBusMessage* (*dbus_message_new_error)(DBusMessage* replyTo,
                                         const char* errorName,
                                         const char* errorMessage) = nullptr;
  DBusMessage* (*dbus_message_new_method_call)(const char* busName,
                                               const char* path,
                                               const char* interface,
                                               const char* method) = nullptr;
  DBusMessage* (*dbus_message_new_method_return)(DBusMessage* methodCall) =
      nullptr;
  void (*dbus_message_unref)(DBusMessage* message) = nullptr;

  dbus_bool_t (*dbus_message_iter_append_basic)(DBusMessageIter* iter, int type,
                                                const void* value) = nullptr;
  dbus_bool_t (*dbus_message_iter_close_container)(
      DBusMessageIter* iter, DBusMessageIter* sub) = nullptr;
  int (*dbus_message_iter_get_arg_type)(DBusMessageIter* iter) = nullptr;
  void (*dbus_message_iter_get_basic)(DBusMessageIter* iter,
                                      void* value) = nullptr;
  dbus_bool_t (*dbus_message_iter_init)(DBusMessage* message,
                                        DBusMessageIter* iter) = nullptr;
  void (*dbus_message_iter_init_append)(DBusMessage* message,
                                        DBusMessageIter* iter) = nullptr;
  dbus_bool_t (*dbus_message_iter_next)(DBusMessageIter* iter) = nullptr;
  dbus_bool_t (*dbus_message_iter_open_container)(
      DBusMessageIter* iter, int type, const char* containedSignature,
      DBusMessageIter* sub) = nullptr;
  void (*dbus_message_iter_recurse)(DBusMessageIter* iter,
                                    DBusMessageIter* sub) = nullptr;

  dbus_bool_t (*dbus_pending_call_get_completed)(DBusPendingCall* pending) =
      nullptr;
  DBusMessage* (*dbus_pending_call_steal_reply)(DBusPendingCall* pending) =
      nullptr;
  void (*dbus_pending_call_unref)(DBusPendingCall* pending) = nullptr;

 private:
  DBusApi() = default;

  void* LoadSymbol(const char* name, std::string* error);
  bool Load(std::string* error);

  void* m_handle = nullptr;
};

const DBusApi& DBus();

struct DBusErrorHolder {
  DBusErrorHolder() { DBus().dbus_error_init(&error); }

  ~DBusErrorHolder() {
    if (DBus().dbus_error_is_set(&error)) {
      DBus().dbus_error_free(&error);
    }
  }

  std::string GetName() const {
    return error.name != nullptr ? std::string{error.name} : std::string{};
  }

  std::string GetMessage(std::string_view fallback) const {
    if (error.message != nullptr && error.message[0] != '\0') {
      return error.message;
    }
    if (error.name != nullptr && error.name[0] != '\0') {
      return error.name;
    }
    return std::string{fallback};
  }

  DBusError error{};
};

struct DBusMessagePtr {
  DBusMessagePtr() = default;
  explicit DBusMessagePtr(DBusMessage* message) : message{message} {}

  ~DBusMessagePtr() { Reset(); }

  DBusMessagePtr(const DBusMessagePtr&) = delete;
  DBusMessagePtr& operator=(const DBusMessagePtr&) = delete;

  DBusMessagePtr(DBusMessagePtr&& rhs) noexcept : message{rhs.message} {
    rhs.message = nullptr;
  }

  DBusMessagePtr& operator=(DBusMessagePtr&& rhs) noexcept {
    if (this != &rhs) {
      Reset();
      message = rhs.message;
      rhs.message = nullptr;
    }
    return *this;
  }

  explicit operator bool() const { return message != nullptr; }

  DBusMessage* Get() const { return message; }

  void Reset() {
    if (message != nullptr) {
      DBus().dbus_message_unref(message);
      message = nullptr;
    }
  }

  DBusMessage* message = nullptr;
};

struct DBusPendingCallPtr {
  DBusPendingCallPtr() = default;
  explicit DBusPendingCallPtr(DBusPendingCall* pending) : pending{pending} {}

  ~DBusPendingCallPtr() { Reset(); }

  DBusPendingCallPtr(const DBusPendingCallPtr&) = delete;
  DBusPendingCallPtr& operator=(const DBusPendingCallPtr&) = delete;

  explicit operator bool() const { return pending != nullptr; }

  DBusPendingCall* Get() const { return pending; }

  void Reset() {
    if (pending != nullptr) {
      DBus().dbus_pending_call_unref(pending);
      pending = nullptr;
    }
  }

  DBusPendingCall* pending = nullptr;
};

}  // namespace wpi::net::linuxbluetooth
