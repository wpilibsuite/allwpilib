// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothLEPacketClient.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <dbus/dbus.h>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Poll.hpp"

namespace uv = wpi::net::uv;

using namespace wpi::net;

namespace {

#ifndef AF_BLUETOOTH
constexpr int WPI_AF_BLUETOOTH = 31;
#else
constexpr int WPI_AF_BLUETOOTH = AF_BLUETOOTH;
#endif
constexpr int BLUETOOTH_PROTOCOL_L2CAP = 0;
constexpr int WPI_SOL_BLUETOOTH = 274;
constexpr int WPI_BT_SECURITY = 4;
constexpr uint8_t WPI_BT_SECURITY_LOW = 1;
constexpr uint8_t WPI_BDADDR_LE_PUBLIC = 0x01;
constexpr uint8_t WPI_BDADDR_LE_RANDOM = 0x02;
constexpr uint16_t WPI_L2CAP_CID_ATT = 0x0004;

constexpr uint16_t DEFAULT_ATT_MTU = 23;
constexpr uint16_t MAX_ATT_MTU = 517;
constexpr uint8_t ATT_OP_ERROR_RESPONSE = 0x01;
constexpr uint8_t ATT_OP_EXCHANGE_MTU_REQUEST = 0x02;
constexpr uint8_t ATT_OP_EXCHANGE_MTU_RESPONSE = 0x03;
constexpr uint8_t ATT_OP_FIND_INFORMATION_REQUEST = 0x04;
constexpr uint8_t ATT_OP_FIND_INFORMATION_RESPONSE = 0x05;
constexpr uint8_t ATT_OP_FIND_BY_TYPE_VALUE_REQUEST = 0x06;
constexpr uint8_t ATT_OP_FIND_BY_TYPE_VALUE_RESPONSE = 0x07;
constexpr uint8_t ATT_OP_READ_BY_TYPE_REQUEST = 0x08;
constexpr uint8_t ATT_OP_READ_BY_TYPE_RESPONSE = 0x09;
constexpr uint8_t ATT_OP_WRITE_REQUEST = 0x12;
constexpr uint8_t ATT_OP_WRITE_RESPONSE = 0x13;
constexpr uint8_t ATT_OP_NOTIFICATION = 0x1b;
constexpr uint8_t ATT_OP_WRITE_COMMAND = 0x52;
constexpr uint8_t ATT_ERROR_ATTRIBUTE_NOT_FOUND = 0x0a;

constexpr uint16_t GATT_PRIMARY_SERVICE_UUID = 0x2800;
constexpr uint16_t GATT_CHARACTERISTIC_UUID = 0x2803;
constexpr uint16_t GATT_CLIENT_CHARACTERISTIC_CONFIG_UUID = 0x2902;

#ifndef MSG_NOSIGNAL
constexpr int MSG_NOSIGNAL = 0;
#endif

struct bdaddr_t {
  uint8_t b[6];
} __attribute__((packed));

struct sockaddr_l2 {
  sa_family_t l2_family;
  uint16_t l2_psm;
  bdaddr_t l2_bdaddr;
  uint16_t l2_cid;
  uint8_t l2_bdaddr_type;
};

struct bt_security {
  uint8_t level;
  uint8_t key_size;
};

uint16_t HostToLe16(uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return value;
#else
  return static_cast<uint16_t>((value >> 8) | (value << 8));
#endif
}

int HexDigit(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  return -1;
}

bool ParseBluetoothAddress(std::string_view address, bdaddr_t* out) {
  if (address.size() != 17) {
    return false;
  }

  bdaddr_t parsed{};
  for (int i = 0; i < 6; ++i) {
    size_t pos = static_cast<size_t>(i * 3);
    if (i < 5 && address[pos + 2] != ':') {
      return false;
    }

    int high = HexDigit(address[pos]);
    int low = HexDigit(address[pos + 1]);
    if (high < 0 || low < 0) {
      return false;
    }

    // BlueZ stores Bluetooth addresses least-significant octet first.
    parsed.b[5 - i] = static_cast<uint8_t>((high << 4) | low);
  }

  *out = parsed;
  return true;
}

bool IsBluetoothAddress(std::string_view address) {
  bdaddr_t parsed{};
  return ParseBluetoothAddress(address, &parsed);
}

void SortBluetoothDevices(std::vector<BluetoothLEDeviceInfo>* devices) {
  std::stable_sort(devices->begin(), devices->end(),
                   [](const auto& a, const auto& b) {
                     std::string_view aKey =
                         a.name.empty() ? std::string_view{a.target} : a.name;
                     std::string_view bKey =
                         b.name.empty() ? std::string_view{b.target} : b.name;
                     if (aKey != bKey) {
                       return aKey < bKey;
                     }
                     return a.target < b.target;
                   });
}

constexpr const char* BLUEZ_SERVICE = "org.bluez";
constexpr const char* BLUEZ_ROOT_PATH = "/";
constexpr const char* BLUEZ_MANAGER_PATH = "/org/bluez";
constexpr const char* BLUEZ_ADAPTER_INTERFACE = "org.bluez.Adapter1";
constexpr const char* BLUEZ_DEVICE_INTERFACE = "org.bluez.Device1";
constexpr const char* BLUEZ_AGENT_INTERFACE = "org.bluez.Agent1";
constexpr const char* BLUEZ_AGENT_MANAGER_INTERFACE = "org.bluez.AgentManager1";
constexpr const char* DBUS_OBJECT_MANAGER_INTERFACE =
    "org.freedesktop.DBus.ObjectManager";
constexpr const char* DBUS_PROPERTIES_INTERFACE =
    "org.freedesktop.DBus.Properties";
constexpr const char* DBUS_INTROSPECTABLE_INTERFACE =
    "org.freedesktop.DBus.Introspectable";
constexpr const char* BLUEZ_AGENT_PATH = "/org/wpilib/wpinet/bluetooth/agent";
constexpr const char* BLUEZ_AGENT_CAPABILITY = "NoInputNoOutput";
constexpr std::chrono::milliseconds BLUEZ_PAIR_TIMEOUT{30000};

struct DBusErrorHolder {
  DBusErrorHolder() { dbus_error_init(&error); }

  ~DBusErrorHolder() {
    if (dbus_error_is_set(&error)) {
      dbus_error_free(&error);
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
      dbus_message_unref(message);
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
      dbus_pending_call_unref(pending);
      pending = nullptr;
    }
  }

  DBusPendingCall* pending = nullptr;
};

struct BlueZAdapterInfo {
  std::string path;
  bool hasPowered = false;
  bool powered = false;
};

struct BlueZDeviceInfo {
  std::string path;
  BluetoothLEDeviceInfo device;
};

struct BlueZObjects {
  std::vector<BlueZAdapterInfo> adapters;
  std::vector<BlueZDeviceInfo> devices;
};

class BlueZClient {
 public:
  ~BlueZClient() {
    UnregisterPairingAgent();
    if (m_connection != nullptr) {
      dbus_connection_close(m_connection);
      dbus_connection_unref(m_connection);
    }
  }

  bool Connect(std::string* error) {
    DBusErrorHolder dbusError;
    m_connection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &dbusError.error);
    if (m_connection == nullptr) {
      *error = dbusError.GetMessage("Failed to connect to the system D-Bus");
      return false;
    }
    dbus_connection_set_exit_on_disconnect(m_connection, false);
    return true;
  }

  bool GetManagedObjects(BlueZObjects* objects, std::string* error) {
    DBusMessagePtr reply =
        CallMethod(BLUEZ_ROOT_PATH, DBUS_OBJECT_MANAGER_INTERFACE,
                   "GetManagedObjects", DBUS_TIMEOUT_USE_DEFAULT, {}, error);
    if (!reply) {
      return false;
    }
    return ParseManagedObjects(reply.Get(), objects, error);
  }

  bool StartDiscovery(const std::string& adapterPath, std::string* error,
                      std::string* errorName) {
    return CallNoArgs(adapterPath.c_str(), BLUEZ_ADAPTER_INTERFACE,
                      "StartDiscovery", DBUS_TIMEOUT_USE_DEFAULT, error,
                      errorName);
  }

  void StopDiscovery(const std::string& adapterPath) {
    std::string error;
    std::string errorName;
    CallNoArgs(adapterPath.c_str(), BLUEZ_ADAPTER_INTERFACE, "StopDiscovery",
               DBUS_TIMEOUT_USE_DEFAULT, &error, &errorName);
  }

  bool PairDevice(const std::string& devicePath, std::string* error,
                  std::string* errorName) {
    if (!RegisterPairingAgent(error)) {
      return false;
    }

    DBusMessagePtr request{dbus_message_new_method_call(
        BLUEZ_SERVICE, devicePath.c_str(), BLUEZ_DEVICE_INTERFACE, "Pair")};
    if (!request) {
      *error = "Failed to create Bluetooth pairing request";
      return false;
    }

    DBusPendingCall* pendingCall = nullptr;
    if (!dbus_connection_send_with_reply(
            m_connection, request.Get(), &pendingCall,
            static_cast<int>(BLUEZ_PAIR_TIMEOUT.count())) ||
        pendingCall == nullptr) {
      *error = "Failed to send Bluetooth pairing request";
      return false;
    }
    DBusPendingCallPtr pending{pendingCall};
    dbus_connection_flush(m_connection);

    auto deadline = std::chrono::steady_clock::now() + BLUEZ_PAIR_TIMEOUT;
    while (!dbus_pending_call_get_completed(pending.Get())) {
      auto now = std::chrono::steady_clock::now();
      if (now >= deadline) {
        *error = "Bluetooth pairing timed out";
        return false;
      }

      auto remaining =
          std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
      int dispatchTimeout =
          static_cast<int>(std::min<std::chrono::milliseconds>(
                               remaining, std::chrono::milliseconds{100})
                               .count());
      if (!dbus_connection_read_write_dispatch(m_connection, dispatchTimeout)) {
        *error = "D-Bus connection closed during Bluetooth pairing";
        return false;
      }
    }

    DBusMessagePtr reply{dbus_pending_call_steal_reply(pending.Get())};
    if (!reply) {
      *error = "Bluetooth pairing did not return a response";
      return false;
    }
    if (dbus_message_get_type(reply.Get()) == DBUS_MESSAGE_TYPE_ERROR) {
      *errorName = GetMessageErrorName(reply.Get());
      *error = GetMessageError(reply.Get(), "Bluetooth pairing failed");
      return false;
    }
    return true;
  }

  bool SetTrusted(const std::string& devicePath, bool trusted,
                  std::string* error) {
    return CallMethod(
               devicePath.c_str(), DBUS_PROPERTIES_INTERFACE, "Set",
               DBUS_TIMEOUT_USE_DEFAULT,
               [&](DBusMessage* message) {
                 DBusMessageIter iter;
                 dbus_message_iter_init_append(message, &iter);
                 const char* interface = BLUEZ_DEVICE_INTERFACE;
                 const char* property = "Trusted";
                 int trustedValue = trusted ? 1 : 0;
                 if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,
                                                     &interface) ||
                     !dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,
                                                     &property)) {
                   return false;
                 }

                 DBusMessageIter variant;
                 if (!dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                                       "b", &variant)) {
                   return false;
                 }
                 if (!dbus_message_iter_append_basic(
                         &variant, DBUS_TYPE_BOOLEAN, &trustedValue)) {
                   return false;
                 }
                 return dbus_message_iter_close_container(&iter, &variant) != 0;
               },
               error)
               .Get() != nullptr;
  }

 private:
  using AppendArgsFunc = std::function<bool(DBusMessage*)>;

  static DBusHandlerResult AgentMessageHandler(DBusConnection* connection,
                                               DBusMessage* message,
                                               void* userData) {
    return static_cast<BlueZClient*>(userData)->HandleAgentMessage(connection,
                                                                   message);
  }

  bool RegisterPairingAgent(std::string* error) {
    if (m_agentRegistered) {
      return true;
    }

    static DBusObjectPathVTable vtable{
        nullptr, AgentMessageHandler, nullptr, nullptr, nullptr, nullptr};
    if (!dbus_connection_register_object_path(m_connection, BLUEZ_AGENT_PATH,
                                              &vtable, this)) {
      *error = "Failed to register Bluetooth pairing agent object";
      return false;
    }
    m_agentObjectRegistered = true;

    bool registered = CallMethod(
                          BLUEZ_MANAGER_PATH, BLUEZ_AGENT_MANAGER_INTERFACE,
                          "RegisterAgent", DBUS_TIMEOUT_USE_DEFAULT,
                          [&](DBusMessage* message) {
                            DBusMessageIter iter;
                            dbus_message_iter_init_append(message, &iter);
                            const char* path = BLUEZ_AGENT_PATH;
                            const char* capability = BLUEZ_AGENT_CAPABILITY;
                            return dbus_message_iter_append_basic(
                                       &iter, DBUS_TYPE_OBJECT_PATH, &path) &&
                                   dbus_message_iter_append_basic(
                                       &iter, DBUS_TYPE_STRING, &capability);
                          },
                          error)
                          .Get() != nullptr;
    if (!registered) {
      UnregisterPairingAgent();
      return false;
    }
    m_agentRegistered = true;

    std::string defaultAgentError;
    CallMethod(
        BLUEZ_MANAGER_PATH, BLUEZ_AGENT_MANAGER_INTERFACE,
        "RequestDefaultAgent", DBUS_TIMEOUT_USE_DEFAULT,
        [&](DBusMessage* message) {
          DBusMessageIter iter;
          dbus_message_iter_init_append(message, &iter);
          const char* path = BLUEZ_AGENT_PATH;
          return dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH,
                                                &path);
        },
        &defaultAgentError);
    return true;
  }

  void UnregisterPairingAgent() {
    if (m_connection == nullptr) {
      return;
    }

    if (m_agentObjectRegistered) {
      dbus_connection_unregister_object_path(m_connection, BLUEZ_AGENT_PATH);
      m_agentObjectRegistered = false;
    }
    m_agentRegistered = false;
  }

  DBusHandlerResult HandleAgentMessage(DBusConnection* connection,
                                       DBusMessage* message) {
    const char* interface = dbus_message_get_interface(message);
    const char* member = dbus_message_get_member(message);
    if (member == nullptr) {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    DBusMessagePtr reply;
    if (interface != nullptr &&
        std::strcmp(interface, DBUS_INTROSPECTABLE_INTERFACE) == 0 &&
        std::strcmp(member, "Introspect") == 0) {
      reply = DBusMessagePtr{dbus_message_new_method_return(message)};
      if (!reply) {
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
      }
      DBusMessageIter iter;
      dbus_message_iter_init_append(reply.Get(), &iter);
      const char* xml =
          "<node><interface name=\"org.bluez.Agent1\">"
          "<method name=\"Release\"/>"
          "<method name=\"RequestConfirmation\"/>"
          "<method name=\"RequestAuthorization\"/>"
          "<method name=\"AuthorizeService\"/>"
          "<method name=\"Cancel\"/>"
          "</interface></node>";
      if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &xml)) {
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
      }
    } else if (interface == nullptr ||
               std::strcmp(interface, BLUEZ_AGENT_INTERFACE) != 0) {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    } else if (std::strcmp(member, "RequestPinCode") == 0 ||
               std::strcmp(member, "RequestPasskey") == 0) {
      reply = DBusMessagePtr{dbus_message_new_error(
          message, "org.bluez.Error.Rejected", "Pairing requires user input")};
    } else if (std::strcmp(member, "Release") == 0 ||
               std::strcmp(member, "DisplayPinCode") == 0 ||
               std::strcmp(member, "DisplayPasskey") == 0 ||
               std::strcmp(member, "RequestConfirmation") == 0 ||
               std::strcmp(member, "RequestAuthorization") == 0 ||
               std::strcmp(member, "AuthorizeService") == 0 ||
               std::strcmp(member, "Cancel") == 0) {
      reply = DBusMessagePtr{dbus_message_new_method_return(message)};
    } else {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    if (!reply) {
      return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    if (!dbus_connection_send(connection, reply.Get(), nullptr)) {
      return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_connection_flush(connection);
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  bool CallNoArgs(const char* path, const char* interface, const char* method,
                  int timeoutMs, std::string* error, std::string* errorName) {
    DBusMessagePtr reply =
        CallMethod(path, interface, method, timeoutMs, {}, error, errorName);
    return static_cast<bool>(reply);
  }

  DBusMessagePtr CallMethod(const char* path, const char* interface,
                            const char* method, int timeoutMs,
                            AppendArgsFunc appendArgs, std::string* error,
                            std::string* errorName = nullptr) {
    if (errorName != nullptr) {
      errorName->clear();
    }

    DBusMessagePtr request{
        dbus_message_new_method_call(BLUEZ_SERVICE, path, interface, method)};
    if (!request) {
      *error = "Failed to create D-Bus method call";
      return {};
    }

    if (appendArgs && !appendArgs(request.Get())) {
      *error = "Failed to append D-Bus method arguments";
      return {};
    }

    DBusErrorHolder dbusError;
    DBusMessage* reply = dbus_connection_send_with_reply_and_block(
        m_connection, request.Get(), timeoutMs, &dbusError.error);
    if (reply == nullptr) {
      if (errorName != nullptr) {
        *errorName = dbusError.GetName();
      }
      *error = dbusError.GetMessage("D-Bus method call failed");
      return {};
    }
    return DBusMessagePtr{reply};
  }

  std::string GetString(DBusMessageIter* iter) {
    const char* value = nullptr;
    dbus_message_iter_get_basic(iter, &value);
    return value != nullptr ? std::string{value} : std::string{};
  }

  bool GetBool(DBusMessageIter* iter) {
    int value = 0;
    dbus_message_iter_get_basic(iter, &value);
    return value != 0;
  }

  bool GetVariantString(DBusMessageIter* iter, std::string* value) {
    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_VARIANT) {
      return false;
    }
    DBusMessageIter variant;
    dbus_message_iter_recurse(iter, &variant);
    int type = dbus_message_iter_get_arg_type(&variant);
    if (type != DBUS_TYPE_STRING && type != DBUS_TYPE_OBJECT_PATH) {
      return false;
    }
    *value = GetString(&variant);
    return true;
  }

  bool GetVariantBool(DBusMessageIter* iter, bool* value) {
    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_VARIANT) {
      return false;
    }
    DBusMessageIter variant;
    dbus_message_iter_recurse(iter, &variant);
    if (dbus_message_iter_get_arg_type(&variant) != DBUS_TYPE_BOOLEAN) {
      return false;
    }
    *value = GetBool(&variant);
    return true;
  }

  void ParseAdapterProperties(const std::string& path, DBusMessageIter* props,
                              BlueZObjects* objects) {
    BlueZAdapterInfo adapter;
    adapter.path = path;

    while (dbus_message_iter_get_arg_type(props) == DBUS_TYPE_DICT_ENTRY) {
      DBusMessageIter entry;
      dbus_message_iter_recurse(props, &entry);
      if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING) {
        dbus_message_iter_next(props);
        continue;
      }

      std::string property = GetString(&entry);
      if (!dbus_message_iter_next(&entry)) {
        dbus_message_iter_next(props);
        continue;
      }

      if (property == "Powered") {
        bool powered = false;
        if (GetVariantBool(&entry, &powered)) {
          adapter.hasPowered = true;
          adapter.powered = powered;
        }
      }

      dbus_message_iter_next(props);
    }

    objects->adapters.emplace_back(std::move(adapter));
  }

  void ParseDeviceProperties(const std::string& path, DBusMessageIter* props,
                             BlueZObjects* objects) {
    BlueZDeviceInfo deviceInfo;
    deviceInfo.path = path;
    deviceInfo.device.addressType = BluetoothAddressType::RANDOM;

    std::string alias;
    while (dbus_message_iter_get_arg_type(props) == DBUS_TYPE_DICT_ENTRY) {
      DBusMessageIter entry;
      dbus_message_iter_recurse(props, &entry);
      if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING) {
        dbus_message_iter_next(props);
        continue;
      }

      std::string property = GetString(&entry);
      if (!dbus_message_iter_next(&entry)) {
        dbus_message_iter_next(props);
        continue;
      }

      if (property == "Address") {
        GetVariantString(&entry, &deviceInfo.device.target);
      } else if (property == "Name") {
        GetVariantString(&entry, &deviceInfo.device.name);
      } else if (property == "Alias") {
        GetVariantString(&entry, &alias);
      } else if (property == "AddressType") {
        std::string addressType;
        if (GetVariantString(&entry, &addressType) && addressType == "public") {
          deviceInfo.device.addressType = BluetoothAddressType::PUBLIC;
        }
      } else if (property == "Paired") {
        GetVariantBool(&entry, &deviceInfo.device.paired);
      }

      dbus_message_iter_next(props);
    }

    if (deviceInfo.device.name.empty()) {
      deviceInfo.device.name = std::move(alias);
    }
    deviceInfo.device.pairable = !deviceInfo.device.paired;
    if (IsBluetoothAddress(deviceInfo.device.target)) {
      objects->devices.emplace_back(std::move(deviceInfo));
    }
  }

  bool ParseManagedObjects(DBusMessage* message, BlueZObjects* objects,
                           std::string* error) {
    objects->adapters.clear();
    objects->devices.clear();

    DBusMessageIter iter;
    if (!dbus_message_iter_init(message, &iter) ||
        dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY) {
      *error = "BlueZ returned an invalid managed object list";
      return false;
    }

    DBusMessageIter objectArray;
    dbus_message_iter_recurse(&iter, &objectArray);
    while (dbus_message_iter_get_arg_type(&objectArray) ==
           DBUS_TYPE_DICT_ENTRY) {
      DBusMessageIter objectEntry;
      dbus_message_iter_recurse(&objectArray, &objectEntry);
      if (dbus_message_iter_get_arg_type(&objectEntry) !=
          DBUS_TYPE_OBJECT_PATH) {
        dbus_message_iter_next(&objectArray);
        continue;
      }
      std::string objectPath = GetString(&objectEntry);
      if (!dbus_message_iter_next(&objectEntry) ||
          dbus_message_iter_get_arg_type(&objectEntry) != DBUS_TYPE_ARRAY) {
        dbus_message_iter_next(&objectArray);
        continue;
      }

      DBusMessageIter interfaces;
      dbus_message_iter_recurse(&objectEntry, &interfaces);
      while (dbus_message_iter_get_arg_type(&interfaces) ==
             DBUS_TYPE_DICT_ENTRY) {
        DBusMessageIter interfaceEntry;
        dbus_message_iter_recurse(&interfaces, &interfaceEntry);
        if (dbus_message_iter_get_arg_type(&interfaceEntry) !=
            DBUS_TYPE_STRING) {
          dbus_message_iter_next(&interfaces);
          continue;
        }
        std::string interfaceName = GetString(&interfaceEntry);
        if (!dbus_message_iter_next(&interfaceEntry) ||
            dbus_message_iter_get_arg_type(&interfaceEntry) !=
                DBUS_TYPE_ARRAY) {
          dbus_message_iter_next(&interfaces);
          continue;
        }

        DBusMessageIter properties;
        dbus_message_iter_recurse(&interfaceEntry, &properties);
        if (interfaceName == BLUEZ_ADAPTER_INTERFACE) {
          ParseAdapterProperties(objectPath, &properties, objects);
        } else if (interfaceName == BLUEZ_DEVICE_INTERFACE) {
          ParseDeviceProperties(objectPath, &properties, objects);
        }

        dbus_message_iter_next(&interfaces);
      }

      dbus_message_iter_next(&objectArray);
    }
    return true;
  }

  std::string GetMessageErrorName(DBusMessage* message) {
    const char* name = dbus_message_get_error_name(message);
    return name != nullptr ? std::string{name} : std::string{};
  }

  std::string GetMessageError(DBusMessage* message, std::string_view fallback) {
    std::string error = GetMessageErrorName(message);
    if (error.empty()) {
      error = std::string{fallback};
    }

    DBusMessageIter iter;
    if (dbus_message_iter_init(message, &iter) &&
        dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING) {
      std::string detail = GetString(&iter);
      if (!detail.empty()) {
        error.append(": ");
        error.append(detail);
      }
    }
    return error;
  }

  DBusConnection* m_connection = nullptr;
  bool m_agentObjectRegistered = false;
  bool m_agentRegistered = false;
};

const BlueZAdapterInfo* SelectBluetoothAdapter(
    const std::vector<BlueZAdapterInfo>& adapters) {
  auto powered =
      std::find_if(adapters.begin(), adapters.end(), [](const auto& adapter) {
        return !adapter.hasPowered || adapter.powered;
      });
  if (powered != adapters.end()) {
    return &*powered;
  }
  return adapters.empty() ? nullptr : &adapters.front();
}

const BlueZDeviceInfo* FindBluetoothDevice(
    const std::vector<BlueZDeviceInfo>& devices, std::string_view target) {
  auto device =
      std::find_if(devices.begin(), devices.end(), [&](const auto& info) {
        return info.device.target == target || info.path == target;
      });
  return device != devices.end() ? &*device : nullptr;
}

bool ParseUuid128(std::string_view uuid, std::array<uint8_t, 16>* out) {
  if (uuid.size() != 36) {
    return false;
  }

  std::array<uint8_t, 16> bigEndian{};
  size_t byteIndex = 0;
  for (size_t i = 0; i < uuid.size();) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      if (uuid[i] != '-') {
        return false;
      }
      ++i;
      continue;
    }

    if (i + 1 >= uuid.size() || byteIndex >= bigEndian.size()) {
      return false;
    }

    int high = HexDigit(uuid[i]);
    int low = HexDigit(uuid[i + 1]);
    if (high < 0 || low < 0) {
      return false;
    }

    bigEndian[byteIndex++] = static_cast<uint8_t>((high << 4) | low);
    i += 2;
  }

  if (byteIndex != bigEndian.size()) {
    return false;
  }

  std::reverse_copy(bigEndian.begin(), bigEndian.end(), out->begin());
  return true;
}

int SetCloseOnExecAndNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFD, 0);
  if (flags < 0 || fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
    return -1;
  }

  flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    return -1;
  }

  return 0;
}

std::string ErrnoString(std::string_view prefix) {
  std::string error{prefix};
  error.append(": ");
  error.append(std::strerror(errno));
  return error;
}

uint16_t ReadLe16(const uint8_t* data) {
  return static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8);
}

void AppendLe16(std::vector<uint8_t>* data, uint16_t value) {
  data->push_back(static_cast<uint8_t>(value & 0xff));
  data->push_back(static_cast<uint8_t>(value >> 8));
}

void AppendUuid16(std::vector<uint8_t>* data, uint16_t uuid) {
  AppendLe16(data, uuid);
}

bool Uuid128Equals(std::span<const uint8_t> value,
                   const std::array<uint8_t, 16>& uuid) {
  return value.size() == uuid.size() &&
         std::equal(value.begin(), value.end(), uuid.begin());
}

enum class LinuxBluetoothTransport { NONE, L2CAP, GATT };

enum class GattDiscoveryState {
  IDLE,
  WAIT_MTU_RESPONSE,
  WAIT_SERVICE_RESPONSE,
  WAIT_CHARACTERISTIC_RESPONSE,
  WAIT_DESCRIPTOR_RESPONSE,
  WAIT_CCCD_WRITE_RESPONSE,
  CONNECTED
};

struct GattCharacteristicInfo {
  uint16_t declarationHandle = 0;
  uint16_t valueHandle = 0;
  bool hasUuid128 = false;
  std::array<uint8_t, 16> uuid128{};
};

}  // namespace

class BluetoothLEPacketClient::Impl
    : public std::enable_shared_from_this<BluetoothLEPacketClient::Impl> {
 public:
  using LoopFunc = std::function<void()>;
  using UvExecFunc = uv::Async<LoopFunc>;

  static std::shared_ptr<Impl> Create(uv::Loop& loop,
                                      PacketCallback packetCallback,
                                      StatusCallback statusCallback) {
    auto impl = std::make_shared<Impl>(loop, std::move(packetCallback),
                                       std::move(statusCallback));
    impl->m_exec = UvExecFunc::Create(loop);
    if (!impl->m_exec) {
      return nullptr;
    }
    impl->m_exec->wakeup.connect([](auto func) { func(); });
    return impl;
  }

  Impl(uv::Loop& loop, PacketCallback packetCallback,
       StatusCallback statusCallback)
      : m_loop{loop},
        m_packetCallback{std::move(packetCallback)},
        m_statusCallback{std::move(statusCallback)} {
    m_status.supported = true;
    m_status.status = "Waiting for Bluetooth address";
  }

  ~Impl() { CloseOnLoop({}); }

  bool Connect(BluetoothLEPacketClientConfig config) {
    if (config.address.empty()) {
      SetError("No Bluetooth address configured");
      return false;
    }
    if (config.preferL2CAP && config.psm == 0) {
      SetError("No Bluetooth L2CAP PSM configured");
      return false;
    }

    {
      std::scoped_lock lock{m_statusMutex};
      m_config = config;
      m_status.targetAddress = config.address;
      m_status.addressType = config.addressType;
      m_status.targetConfigured = true;
      m_status.error.clear();
      m_status.status =
          config.preferL2CAP ? "Connecting (L2CAP)" : "Connecting (GATT)";
      m_status.connecting = true;
      m_status.connected = false;
      m_status.transport = BluetoothPacketTransport::NONE;
    }
    PublishStatus();

    auto self = shared_from_this();
    m_exec->Send(
        [self, config = std::move(config)] { self->ConnectOnLoop(config); });
    return true;
  }

  void Disconnect(std::string_view reason) {
    auto self = shared_from_this();
    std::string reasonString{reason};
    m_exec->Send([self, reasonString = std::move(reasonString)] {
      self->CloseOnLoop(reasonString);
    });
  }

  bool Send(std::span<const uint8_t> packet) {
    if (packet.empty()) {
      return false;
    }

    bool tooLarge = false;
    {
      std::scoped_lock lock{m_statusMutex};
      if (!m_status.connected) {
        return false;
      }
      tooLarge = packet.size() > m_config.maxPacketSize;
    }
    if (tooLarge) {
      SetError("Packet is larger than Bluetooth transport MTU");
      return false;
    }

    std::vector<uint8_t> packetCopy{packet.begin(), packet.end()};
    auto self = shared_from_this();
    m_exec->Send([self, packetCopy = std::move(packetCopy)] {
      self->SendOnLoop(packetCopy);
    });
    return true;
  }

  BluetoothLEPacketConnectionStatus GetStatus() const {
    std::scoped_lock lock{m_statusMutex};
    return m_status;
  }

 private:
  void ConnectOnLoop(const BluetoothLEPacketClientConfig& config) {
    CloseOnLoop("Connecting");

    bdaddr_t remoteAddress{};
    if (!ParseBluetoothAddress(config.address, &remoteAddress)) {
      SetError("Bluetooth address must use the form AA:BB:CC:DD:EE:FF");
      return;
    }
    m_remoteAddress = remoteAddress;

    if (config.preferL2CAP) {
      ConnectL2CAPOnLoop(config, remoteAddress);
    } else {
      ConnectGattOnLoop(config, remoteAddress);
    }
  }

  bool ConfigureBluetoothSocket(int fd, std::string_view transportName) {
    if (SetCloseOnExecAndNonBlock(fd) < 0) {
      SetError(ErrnoString("Failed to configure Bluetooth socket"));
      return false;
    }

    bt_security security{};
    security.level = WPI_BT_SECURITY_LOW;
    if (::setsockopt(fd, WPI_SOL_BLUETOOTH, WPI_BT_SECURITY, &security,
                     sizeof(security)) < 0) {
      SetError(ErrnoString("Failed to configure Bluetooth security"));
      return false;
    }

    sockaddr_l2 localAddr{};
    localAddr.l2_family = WPI_AF_BLUETOOTH;
    localAddr.l2_bdaddr_type = WPI_BDADDR_LE_PUBLIC;
    if (m_activeTransport == LinuxBluetoothTransport::GATT) {
      localAddr.l2_cid = HostToLe16(WPI_L2CAP_CID_ATT);
    }
    if (::bind(fd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) <
        0) {
      SetError(ErrnoString(std::string{"Failed to bind Bluetooth "}
                               .append(transportName)
                               .append(" socket")));
      return false;
    }

    return true;
  }

  bool StartSocketPoll(int fd) {
    m_poll = uv::Poll::Create(m_loop, fd);
    if (!m_poll) {
      SetError("Failed to create Bluetooth socket poll handle");
      return false;
    }

    auto self = shared_from_this();
    m_poll->pollEvent.connect([self](int events) {
      if ((events & UV_DISCONNECT) != 0) {
        self->CloseOnLoop("Bluetooth connection closed");
        return;
      }
      if ((events & UV_WRITABLE) != 0) {
        self->CheckConnect();
      }
      if ((events & UV_READABLE) != 0) {
        if (self->m_activeTransport == LinuxBluetoothTransport::GATT) {
          self->ReadGattPackets();
        } else {
          self->ReadPackets();
        }
      }
    });

    return true;
  }

  void ConnectL2CAPOnLoop(const BluetoothLEPacketClientConfig& config,
                          const bdaddr_t& remoteAddress) {
    m_activeTransport = LinuxBluetoothTransport::L2CAP;

    int fd =
        ::socket(WPI_AF_BLUETOOTH, SOCK_SEQPACKET, BLUETOOTH_PROTOCOL_L2CAP);
    if (fd < 0) {
      BeginGattFallback(ErrnoString("Failed to create Bluetooth L2CAP socket"),
                        config, remoteAddress);
      return;
    }

    m_socket = fd;

    if (!ConfigureBluetoothSocket(fd, "L2CAP")) {
      BeginGattFallback(GetStatus().error, config, remoteAddress);
      return;
    }

    sockaddr_l2 remoteAddr{};
    remoteAddr.l2_family = WPI_AF_BLUETOOTH;
    remoteAddr.l2_psm = HostToLe16(config.psm);
    remoteAddr.l2_bdaddr = remoteAddress;
    remoteAddr.l2_bdaddr_type =
        config.addressType == BluetoothAddressType::PUBLIC
            ? WPI_BDADDR_LE_PUBLIC
            : WPI_BDADDR_LE_RANDOM;

    int result = ::connect(fd, reinterpret_cast<sockaddr*>(&remoteAddr),
                           sizeof(remoteAddr));
    if (result < 0 && errno != EINPROGRESS) {
      BeginGattFallback(ErrnoString("Failed to connect Bluetooth L2CAP socket"),
                        config, remoteAddress);
      return;
    }

    if (!StartSocketPoll(fd)) {
      BeginGattFallback(GetStatus().error, config, remoteAddress);
      return;
    }

    SetConnecting("Connecting (L2CAP)");

    if (result == 0) {
      CheckConnect();
    } else {
      m_poll->Start(UV_WRITABLE | UV_DISCONNECT);
    }
  }

  void ConnectGattOnLoop(const BluetoothLEPacketClientConfig& config,
                         const bdaddr_t& remoteAddress) {
    if (!ParseUuid128(config.gattServiceUuid, &m_gattServiceUuid) ||
        !ParseUuid128(config.gattControlCharacteristicUuid,
                      &m_gattControlCharacteristicUuid) ||
        !ParseUuid128(config.gattStatusCharacteristicUuid,
                      &m_gattStatusCharacteristicUuid)) {
      SetError("Bluetooth GATT UUIDs must use 128-bit UUID string form");
      return;
    }

    m_activeTransport = LinuxBluetoothTransport::GATT;
    ResetGattDiscovery();

    int fd =
        ::socket(WPI_AF_BLUETOOTH, SOCK_SEQPACKET, BLUETOOTH_PROTOCOL_L2CAP);
    if (fd < 0) {
      SetError(ErrnoString("Failed to create Bluetooth GATT socket"));
      return;
    }

    m_socket = fd;

    if (!ConfigureBluetoothSocket(fd, "GATT")) {
      CloseSocket();
      return;
    }

    sockaddr_l2 remoteAddr{};
    remoteAddr.l2_family = WPI_AF_BLUETOOTH;
    remoteAddr.l2_cid = HostToLe16(WPI_L2CAP_CID_ATT);
    remoteAddr.l2_bdaddr = remoteAddress;
    remoteAddr.l2_bdaddr_type =
        config.addressType == BluetoothAddressType::PUBLIC
            ? WPI_BDADDR_LE_PUBLIC
            : WPI_BDADDR_LE_RANDOM;

    int result = ::connect(fd, reinterpret_cast<sockaddr*>(&remoteAddr),
                           sizeof(remoteAddr));
    if (result < 0 && errno != EINPROGRESS) {
      SetError(ErrnoString("Failed to connect Bluetooth GATT socket"));
      CloseSocket();
      return;
    }

    if (!StartSocketPoll(fd)) {
      CloseSocket();
      return;
    }

    SetConnecting("Connecting (GATT)");

    if (result == 0) {
      CheckConnect();
    } else {
      m_poll->Start(UV_WRITABLE | UV_DISCONNECT);
    }
  }

  void BeginGattFallback(std::string_view l2capError,
                         const BluetoothLEPacketClientConfig& config,
                         const bdaddr_t& remoteAddress) {
    CloseSocket();
    UpdateStatus([&](auto& status) {
      status.connecting = true;
      status.connected = false;
      status.transport = BluetoothPacketTransport::NONE;
      status.error = l2capError;
      status.status = "L2CAP unavailable; connecting GATT";
    });
    ConnectGattOnLoop(config, remoteAddress);
  }

  void CloseSocket() {
    if (m_poll) {
      if (!m_poll->IsClosing()) {
        m_poll->Stop();
        m_poll->Close();
      }
      m_poll.reset();
    }

    if (m_socket >= 0) {
      ::close(m_socket);
      m_socket = -1;
    }

    m_activeTransport = LinuxBluetoothTransport::NONE;
    m_gattState = GattDiscoveryState::IDLE;
  }

  void CloseOnLoop(std::string_view reason) {
    CloseSocket();

    if (!reason.empty()) {
      UpdateStatus([&](auto& status) {
        status.connecting = false;
        status.connected = false;
        status.transport = BluetoothPacketTransport::NONE;
        status.status = reason;
      });
    }
  }

  void CheckConnect() {
    if (m_socket < 0) {
      return;
    }

    int socketError = 0;
    socklen_t socketErrorLen = sizeof(socketError);
    if (::getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &socketError,
                     &socketErrorLen) < 0) {
      std::string error = ErrnoString("Failed to query Bluetooth connection");
      if (m_activeTransport == LinuxBluetoothTransport::L2CAP) {
        BeginGattFallback(error, m_config, m_remoteAddress);
      } else {
        SetError(error);
        CloseSocket();
      }
      return;
    }

    if (socketError != 0) {
      errno = socketError;
      std::string error =
          m_activeTransport == LinuxBluetoothTransport::GATT
              ? ErrnoString("Failed to connect Bluetooth GATT socket")
              : ErrnoString("Failed to connect Bluetooth L2CAP socket");
      if (m_activeTransport == LinuxBluetoothTransport::L2CAP) {
        BeginGattFallback(error, m_config, m_remoteAddress);
      } else {
        SetError(error);
        CloseSocket();
      }
      return;
    }

    if (m_poll) {
      m_poll->Start(UV_READABLE | UV_DISCONNECT);
    }

    if (m_activeTransport == LinuxBluetoothTransport::GATT) {
      StartGattDiscovery();
      return;
    }

    UpdateStatus([](auto& status) {
      status.connecting = false;
      status.connected = true;
      status.transport = BluetoothPacketTransport::L2CAP;
      status.status = "Connected (L2CAP)";
      status.error.clear();
    });
  }

  void ReadPackets() {
    if (m_socket < 0) {
      return;
    }

    std::vector<uint8_t> packet(m_config.maxPacketSize);
    while (true) {
      ssize_t received = ::recv(m_socket, packet.data(), packet.size(), 0);
      if (received > 0) {
        UpdateStatus([](auto& status) { ++status.packetsReceived; });
        m_packetCallback({packet.data(), static_cast<size_t>(received)});
        continue;
      }

      if (received == 0) {
        CloseOnLoop("Bluetooth connection closed");
        return;
      }

      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }

      SetError(ErrnoString("Bluetooth receive failed"));
      CloseOnLoop("Disconnected");
      return;
    }
  }

  void ResetGattDiscovery() {
    m_gattState = GattDiscoveryState::IDLE;
    m_gattRequestedMtu = DEFAULT_ATT_MTU;
    m_gattMtu = DEFAULT_ATT_MTU;
    m_gattServiceStartHandle = 0;
    m_gattServiceEndHandle = 0;
    m_gattControlValueHandle = 0;
    m_gattStatusDeclarationHandle = 0;
    m_gattStatusValueHandle = 0;
    m_gattStatusCccdHandle = 0;
    m_gattSearchStartHandle = 0;
    m_gattDescriptorEndHandle = 0;
    m_gattCharacteristics.clear();
  }

  void FailGatt(std::string_view error) {
    SetError(error);
    CloseSocket();
  }

  bool SendGattPdu(std::span<const uint8_t> pdu) {
    if (m_socket < 0) {
      return false;
    }

    ssize_t sent = ::send(m_socket, pdu.data(), pdu.size(), MSG_NOSIGNAL);
    if (sent == static_cast<ssize_t>(pdu.size())) {
      return true;
    }

    if (sent < 0) {
      FailGatt(ErrnoString("Bluetooth GATT send failed"));
    } else {
      FailGatt("Bluetooth GATT send was truncated");
    }
    return false;
  }

  void StartGattDiscovery() {
    UpdateStatus([](auto& status) {
      status.status = "Negotiating Bluetooth GATT MTU";
      status.connecting = true;
      status.connected = false;
      status.transport = BluetoothPacketTransport::NONE;
    });

    m_gattRequestedMtu = static_cast<uint16_t>(std::min<size_t>(
        MAX_ATT_MTU,
        std::max<size_t>(DEFAULT_ATT_MTU, m_config.maxPacketSize + 3)));

    std::vector<uint8_t> request;
    request.push_back(ATT_OP_EXCHANGE_MTU_REQUEST);
    AppendLe16(&request, m_gattRequestedMtu);
    m_gattState = GattDiscoveryState::WAIT_MTU_RESPONSE;
    SendGattPdu(request);
  }

  void SendGattFindServiceRequest() {
    UpdateStatus([](auto& status) {
      status.status = "Discovering Bluetooth GATT service";
    });

    std::vector<uint8_t> request;
    request.push_back(ATT_OP_FIND_BY_TYPE_VALUE_REQUEST);
    AppendLe16(&request, 0x0001);
    AppendLe16(&request, 0xffff);
    AppendUuid16(&request, GATT_PRIMARY_SERVICE_UUID);
    request.insert(request.end(), m_gattServiceUuid.begin(),
                   m_gattServiceUuid.end());
    m_gattState = GattDiscoveryState::WAIT_SERVICE_RESPONSE;
    SendGattPdu(request);
  }

  void SendGattReadCharacteristicsRequest() {
    UpdateStatus([](auto& status) {
      status.status = "Discovering Bluetooth GATT characteristics";
    });

    std::vector<uint8_t> request;
    request.push_back(ATT_OP_READ_BY_TYPE_REQUEST);
    AppendLe16(&request, m_gattSearchStartHandle);
    AppendLe16(&request, m_gattServiceEndHandle);
    AppendUuid16(&request, GATT_CHARACTERISTIC_UUID);
    m_gattState = GattDiscoveryState::WAIT_CHARACTERISTIC_RESPONSE;
    SendGattPdu(request);
  }

  void SendGattFindDescriptorRequest() {
    UpdateStatus([](auto& status) {
      status.status = "Discovering Bluetooth GATT notification descriptor";
    });

    std::vector<uint8_t> request;
    request.push_back(ATT_OP_FIND_INFORMATION_REQUEST);
    AppendLe16(&request, m_gattSearchStartHandle);
    AppendLe16(&request, m_gattDescriptorEndHandle);
    m_gattState = GattDiscoveryState::WAIT_DESCRIPTOR_RESPONSE;
    SendGattPdu(request);
  }

  void SendGattWriteCccdRequest() {
    UpdateStatus([](auto& status) {
      status.status = "Enabling Bluetooth GATT notifications";
    });

    std::vector<uint8_t> request;
    request.push_back(ATT_OP_WRITE_REQUEST);
    AppendLe16(&request, m_gattStatusCccdHandle);
    AppendLe16(&request, 0x0001);
    m_gattState = GattDiscoveryState::WAIT_CCCD_WRITE_RESPONSE;
    SendGattPdu(request);
  }

  void HandleGattError(std::span<const uint8_t> pdu) {
    if (pdu.size() < 5) {
      FailGatt("Malformed Bluetooth GATT error response");
      return;
    }

    uint8_t requestOpcode = pdu[1];
    uint8_t errorCode = pdu[4];
    if (m_gattState == GattDiscoveryState::WAIT_MTU_RESPONSE &&
        requestOpcode == ATT_OP_EXCHANGE_MTU_REQUEST) {
      m_gattMtu = DEFAULT_ATT_MTU;
      SendGattFindServiceRequest();
      return;
    }

    if (m_gattState == GattDiscoveryState::WAIT_CHARACTERISTIC_RESPONSE &&
        requestOpcode == ATT_OP_READ_BY_TYPE_REQUEST &&
        errorCode == ATT_ERROR_ATTRIBUTE_NOT_FOUND &&
        !m_gattCharacteristics.empty()) {
      FinishGattCharacteristicDiscovery();
      return;
    }

    if (m_gattState == GattDiscoveryState::WAIT_DESCRIPTOR_RESPONSE &&
        requestOpcode == ATT_OP_FIND_INFORMATION_REQUEST &&
        errorCode == ATT_ERROR_ATTRIBUTE_NOT_FOUND) {
      FailGatt("Bluetooth GATT status notification descriptor was not found");
      return;
    }

    FailGatt("Bluetooth GATT discovery failed");
  }

  void HandleGattMtuResponse(std::span<const uint8_t> pdu) {
    if (m_gattState != GattDiscoveryState::WAIT_MTU_RESPONSE) {
      return;
    }
    if (pdu.size() < 3) {
      FailGatt("Malformed Bluetooth GATT MTU response");
      return;
    }

    uint16_t serverMtu = ReadLe16(pdu.data() + 1);
    m_gattMtu = std::max<uint16_t>(
        DEFAULT_ATT_MTU, std::min<uint16_t>(m_gattRequestedMtu, serverMtu));
    SendGattFindServiceRequest();
  }

  void HandleGattServiceResponse(std::span<const uint8_t> pdu) {
    if (m_gattState != GattDiscoveryState::WAIT_SERVICE_RESPONSE) {
      return;
    }
    if (pdu.size() < 5 || ((pdu.size() - 1) % 4) != 0) {
      FailGatt("Malformed Bluetooth GATT service response");
      return;
    }

    m_gattServiceStartHandle = ReadLe16(pdu.data() + 1);
    m_gattServiceEndHandle = ReadLe16(pdu.data() + 3);
    if (m_gattServiceStartHandle == 0 ||
        m_gattServiceStartHandle > m_gattServiceEndHandle) {
      FailGatt("Bluetooth GATT service has an invalid handle range");
      return;
    }

    m_gattSearchStartHandle = m_gattServiceStartHandle;
    SendGattReadCharacteristicsRequest();
  }

  void HandleGattCharacteristicResponse(std::span<const uint8_t> pdu) {
    if (m_gattState != GattDiscoveryState::WAIT_CHARACTERISTIC_RESPONSE) {
      return;
    }
    if (pdu.size() < 2) {
      FailGatt("Malformed Bluetooth GATT characteristic response");
      return;
    }

    size_t entryLength = pdu[1];
    if (entryLength < 7 || ((pdu.size() - 2) % entryLength) != 0) {
      FailGatt("Malformed Bluetooth GATT characteristic entry");
      return;
    }

    uint16_t lastDeclarationHandle = 0;
    for (size_t offset = 2; offset + entryLength <= pdu.size();
         offset += entryLength) {
      GattCharacteristicInfo characteristic;
      characteristic.declarationHandle = ReadLe16(pdu.data() + offset);
      characteristic.valueHandle = ReadLe16(pdu.data() + offset + 3);
      if (entryLength == 21) {
        characteristic.hasUuid128 = true;
        std::copy_n(pdu.begin() + offset + 5, characteristic.uuid128.size(),
                    characteristic.uuid128.begin());
      }

      if (characteristic.declarationHandle == 0 ||
          characteristic.valueHandle == 0) {
        FailGatt("Bluetooth GATT characteristic has an invalid handle");
        return;
      }

      if (characteristic.hasUuid128) {
        if (Uuid128Equals(characteristic.uuid128,
                          m_gattControlCharacteristicUuid)) {
          m_gattControlValueHandle = characteristic.valueHandle;
        } else if (Uuid128Equals(characteristic.uuid128,
                                 m_gattStatusCharacteristicUuid)) {
          m_gattStatusDeclarationHandle = characteristic.declarationHandle;
          m_gattStatusValueHandle = characteristic.valueHandle;
        }
      }

      lastDeclarationHandle = characteristic.declarationHandle;
      m_gattCharacteristics.emplace_back(characteristic);
    }

    if (lastDeclarationHandle == 0 ||
        lastDeclarationHandle >= m_gattServiceEndHandle) {
      FinishGattCharacteristicDiscovery();
      return;
    }

    m_gattSearchStartHandle = lastDeclarationHandle + 1;
    SendGattReadCharacteristicsRequest();
  }

  void FinishGattCharacteristicDiscovery() {
    if (m_gattControlValueHandle == 0 || m_gattStatusValueHandle == 0) {
      FailGatt("Bluetooth GATT packet characteristics were not found");
      return;
    }

    std::sort(m_gattCharacteristics.begin(), m_gattCharacteristics.end(),
              [](const auto& a, const auto& b) {
                return a.declarationHandle < b.declarationHandle;
              });

    uint16_t descriptorEndHandle = m_gattServiceEndHandle;
    for (const auto& characteristic : m_gattCharacteristics) {
      if (characteristic.declarationHandle > m_gattStatusDeclarationHandle) {
        descriptorEndHandle = characteristic.declarationHandle - 1;
        break;
      }
    }

    if (m_gattStatusValueHandle == std::numeric_limits<uint16_t>::max() ||
        m_gattStatusValueHandle + 1 > descriptorEndHandle) {
      FailGatt("Bluetooth GATT status notification descriptor was not found");
      return;
    }

    m_gattSearchStartHandle = m_gattStatusValueHandle + 1;
    m_gattDescriptorEndHandle = descriptorEndHandle;
    SendGattFindDescriptorRequest();
  }

  void HandleGattDescriptorResponse(std::span<const uint8_t> pdu) {
    if (m_gattState != GattDiscoveryState::WAIT_DESCRIPTOR_RESPONSE) {
      return;
    }
    if (pdu.size() < 2) {
      FailGatt("Malformed Bluetooth GATT descriptor response");
      return;
    }

    uint8_t format = pdu[1];
    size_t entryLength = format == 0x01 ? 4 : 18;
    if ((format != 0x01 && format != 0x02) ||
        ((pdu.size() - 2) % entryLength) != 0) {
      FailGatt("Malformed Bluetooth GATT descriptor entry");
      return;
    }

    uint16_t lastHandle = 0;
    for (size_t offset = 2; offset + entryLength <= pdu.size();
         offset += entryLength) {
      uint16_t handle = ReadLe16(pdu.data() + offset);
      if (format == 0x01) {
        uint16_t uuid = ReadLe16(pdu.data() + offset + 2);
        if (uuid == GATT_CLIENT_CHARACTERISTIC_CONFIG_UUID) {
          m_gattStatusCccdHandle = handle;
          SendGattWriteCccdRequest();
          return;
        }
      }
      lastHandle = handle;
    }

    if (lastHandle == 0 || lastHandle >= m_gattDescriptorEndHandle) {
      FailGatt("Bluetooth GATT status notification descriptor was not found");
      return;
    }

    m_gattSearchStartHandle = lastHandle + 1;
    SendGattFindDescriptorRequest();
  }

  void HandleGattWriteResponse(std::span<const uint8_t> pdu) {
    (void)pdu;
    if (m_gattState != GattDiscoveryState::WAIT_CCCD_WRITE_RESPONSE) {
      return;
    }

    m_gattState = GattDiscoveryState::CONNECTED;
    UpdateStatus([](auto& status) {
      status.connecting = false;
      status.connected = true;
      status.transport = BluetoothPacketTransport::GATT;
      status.status = "Connected (GATT)";
      status.error.clear();
    });
  }

  void HandleGattNotification(std::span<const uint8_t> pdu) {
    if (pdu.size() < 3) {
      return;
    }
    uint16_t handle = ReadLe16(pdu.data() + 1);
    if (handle != m_gattStatusValueHandle) {
      return;
    }

    auto packet = pdu.subspan(3);
    if (packet.empty()) {
      return;
    }

    UpdateStatus([](auto& status) { ++status.packetsReceived; });
    m_packetCallback(packet);
  }

  void HandleGattPdu(std::span<const uint8_t> pdu) {
    if (pdu.empty()) {
      return;
    }

    switch (pdu[0]) {
      case ATT_OP_ERROR_RESPONSE:
        HandleGattError(pdu);
        break;
      case ATT_OP_EXCHANGE_MTU_RESPONSE:
        HandleGattMtuResponse(pdu);
        break;
      case ATT_OP_FIND_BY_TYPE_VALUE_RESPONSE:
        HandleGattServiceResponse(pdu);
        break;
      case ATT_OP_READ_BY_TYPE_RESPONSE:
        HandleGattCharacteristicResponse(pdu);
        break;
      case ATT_OP_FIND_INFORMATION_RESPONSE:
        HandleGattDescriptorResponse(pdu);
        break;
      case ATT_OP_WRITE_RESPONSE:
        HandleGattWriteResponse(pdu);
        break;
      case ATT_OP_NOTIFICATION:
        HandleGattNotification(pdu);
        break;
      default:
        FailGatt("Unexpected Bluetooth GATT response");
        break;
    }
  }

  void ReadGattPackets() {
    if (m_socket < 0) {
      return;
    }

    std::vector<uint8_t> pdu(
        std::max<size_t>(m_gattMtu, m_config.maxPacketSize + 3));
    while (true) {
      ssize_t received = ::recv(m_socket, pdu.data(), pdu.size(), 0);
      if (received > 0) {
        HandleGattPdu({pdu.data(), static_cast<size_t>(received)});
        continue;
      }

      if (received == 0) {
        CloseOnLoop("Bluetooth connection closed");
        return;
      }

      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }

      SetError(ErrnoString("Bluetooth GATT receive failed"));
      CloseOnLoop("Disconnected");
      return;
    }
  }

  void SendOnLoop(std::span<const uint8_t> packet) {
    if (m_socket < 0) {
      return;
    }

    if (m_activeTransport == LinuxBluetoothTransport::GATT) {
      if (m_gattState != GattDiscoveryState::CONNECTED ||
          m_gattControlValueHandle == 0) {
        return;
      }

      if (packet.size() + 3 > m_gattMtu) {
        SetError("Packet is larger than Bluetooth GATT write MTU");
        return;
      }

      std::vector<uint8_t> pdu;
      pdu.reserve(packet.size() + 3);
      pdu.push_back(ATT_OP_WRITE_COMMAND);
      AppendLe16(&pdu, m_gattControlValueHandle);
      pdu.insert(pdu.end(), packet.begin(), packet.end());

      ssize_t sent = ::send(m_socket, pdu.data(), pdu.size(), MSG_NOSIGNAL);
      if (sent == static_cast<ssize_t>(pdu.size())) {
        UpdateStatus([](auto& status) { ++status.packetsSent; });
      } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK &&
                 errno != EINTR) {
        SetError(ErrnoString("Bluetooth GATT send failed"));
        CloseOnLoop("Disconnected");
      }
      return;
    }

    ssize_t sent = ::send(m_socket, packet.data(), packet.size(), MSG_NOSIGNAL);
    if (sent == static_cast<ssize_t>(packet.size())) {
      UpdateStatus([](auto& status) { ++status.packetsSent; });
    } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK &&
               errno != EINTR) {
      SetError(ErrnoString("Bluetooth send failed"));
      CloseOnLoop("Disconnected");
    }
  }

  void SetConnecting(std::string_view text) {
    UpdateStatus([&](auto& status) {
      status.connecting = true;
      status.connected = false;
      status.status = text;
      status.transport = BluetoothPacketTransport::NONE;
    });
  }

  void SetError(std::string_view error) {
    UpdateStatus([&](auto& status) {
      status.error = error;
      status.status = error;
      status.connecting = false;
      status.connected = false;
      status.transport = BluetoothPacketTransport::NONE;
    });
  }

  void PublishStatus() {
    BluetoothLEPacketConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      snapshot = m_status;
    }
    if (m_statusCallback) {
      m_statusCallback(snapshot);
    }
  }

  void PublishStatusSnapshot(
      const BluetoothLEPacketConnectionStatus& snapshot) {
    if (m_statusCallback) {
      m_statusCallback(snapshot);
    }
  }

  template <typename F>
  void UpdateStatus(F&& func) {
    BluetoothLEPacketConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      func(m_status);
      snapshot = m_status;
    }
    PublishStatusSnapshot(snapshot);
  }

  uv::Loop& m_loop;
  PacketCallback m_packetCallback;
  StatusCallback m_statusCallback;
  std::shared_ptr<UvExecFunc> m_exec;

  mutable std::mutex m_statusMutex;
  BluetoothLEPacketConnectionStatus m_status;
  BluetoothLEPacketClientConfig m_config;

  std::shared_ptr<uv::Poll> m_poll;
  int m_socket = -1;
  bdaddr_t m_remoteAddress{};
  LinuxBluetoothTransport m_activeTransport = LinuxBluetoothTransport::NONE;

  std::array<uint8_t, 16> m_gattServiceUuid{};
  std::array<uint8_t, 16> m_gattControlCharacteristicUuid{};
  std::array<uint8_t, 16> m_gattStatusCharacteristicUuid{};
  GattDiscoveryState m_gattState = GattDiscoveryState::IDLE;
  uint16_t m_gattRequestedMtu = DEFAULT_ATT_MTU;
  uint16_t m_gattMtu = DEFAULT_ATT_MTU;
  uint16_t m_gattServiceStartHandle = 0;
  uint16_t m_gattServiceEndHandle = 0;
  uint16_t m_gattControlValueHandle = 0;
  uint16_t m_gattStatusDeclarationHandle = 0;
  uint16_t m_gattStatusValueHandle = 0;
  uint16_t m_gattStatusCccdHandle = 0;
  uint16_t m_gattSearchStartHandle = 0;
  uint16_t m_gattDescriptorEndHandle = 0;
  std::vector<GattCharacteristicInfo> m_gattCharacteristics;
};

std::shared_ptr<BluetoothLEPacketClient> BluetoothLEPacketClient::Create(
    wpi::net::uv::Loop& loop, PacketCallback packetCallback,
    StatusCallback statusCallback) {
  auto impl =
      Impl::Create(loop, std::move(packetCallback), std::move(statusCallback));
  if (!impl) {
    return nullptr;
  }
  return std::shared_ptr<BluetoothLEPacketClient>(
      new BluetoothLEPacketClient{std::move(impl)});
}

BluetoothLEPacketClient::BluetoothLEPacketClient(std::shared_ptr<Impl> impl)
    : m_impl{std::move(impl)} {}

BluetoothLEPacketClient::~BluetoothLEPacketClient() = default;

bool BluetoothLEPacketClient::IsPairingSupported() {
  return true;
}

BluetoothLEDeviceScanResult BluetoothLEPacketClient::ScanDevices(
    std::chrono::milliseconds timeout) {
  BluetoothLEDeviceScanResult result;
  result.supported = true;

  BlueZClient bluez;
  if (!bluez.Connect(&result.error)) {
    return result;
  }

  BlueZObjects objects;
  if (!bluez.GetManagedObjects(&objects, &result.error)) {
    return result;
  }

  const BlueZAdapterInfo* adapter = SelectBluetoothAdapter(objects.adapters);
  if (adapter == nullptr) {
    result.error = "No BlueZ Bluetooth adapter was found";
    return result;
  }

  if (timeout > std::chrono::milliseconds{0}) {
    if (adapter->hasPowered && !adapter->powered) {
      result.error = "Bluetooth adapter is powered off";
      return result;
    }

    std::string discoveryError;
    std::string discoveryErrorName;
    bool discoveryStarted = bluez.StartDiscovery(adapter->path, &discoveryError,
                                                 &discoveryErrorName);
    if (!discoveryStarted &&
        discoveryErrorName != "org.bluez.Error.InProgress") {
      result.error = std::move(discoveryError);
      return result;
    }

    std::this_thread::sleep_for(timeout);
    bluez.StopDiscovery(adapter->path);

    if (!bluez.GetManagedObjects(&objects, &result.error)) {
      return result;
    }
  }

  result.devices.reserve(objects.devices.size());
  for (auto& device : objects.devices) {
    result.devices.emplace_back(std::move(device.device));
  }
  SortBluetoothDevices(&result.devices);
  result.status = "Discovered " + std::to_string(result.devices.size()) +
                  " Bluetooth LE devices";
  return result;
}

BluetoothLEPairingResult BluetoothLEPacketClient::PairDevice(
    std::string_view target) {
  BluetoothLEPairingResult result;
  result.supported = true;

  if (!IsBluetoothAddress(target)) {
    result.error = "Linux pairing requires a Bluetooth address";
    return result;
  }

  BlueZClient bluez;
  if (!bluez.Connect(&result.error)) {
    return result;
  }

  BlueZObjects objects;
  if (!bluez.GetManagedObjects(&objects, &result.error)) {
    return result;
  }

  const BlueZDeviceInfo* device = FindBluetoothDevice(objects.devices, target);
  if (device == nullptr) {
    result.error =
        "Bluetooth device was not found; scan for the device before pairing";
    return result;
  }

  if (!device->device.paired) {
    std::string pairError;
    std::string pairErrorName;
    if (!bluez.PairDevice(device->path, &pairError, &pairErrorName) &&
        pairErrorName != "org.bluez.Error.AlreadyExists") {
      result.error = std::move(pairError);
      return result;
    }
  }

  if (!bluez.SetTrusted(device->path, true, &result.error)) {
    return result;
  }

  result.paired = true;
  result.status = device->device.paired ? "Bluetooth device is already paired"
                                        : "Bluetooth device paired";
  return result;
}

bool BluetoothLEPacketClient::Connect(BluetoothLEPacketClientConfig config) {
  return m_impl->Connect(std::move(config));
}

void BluetoothLEPacketClient::Disconnect(std::string_view reason) {
  m_impl->Disconnect(reason);
}

bool BluetoothLEPacketClient::Send(std::span<const uint8_t> packet) {
  return m_impl->Send(packet);
}

BluetoothLEPacketConnectionStatus BluetoothLEPacketClient::GetStatus() const {
  return m_impl->GetStatus();
}
