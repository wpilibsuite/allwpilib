// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "BlueZInternal.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <format>
#include <functional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "DBusRuntime.hpp"

namespace wpi::net::linuxbluetooth {
namespace {

#define dbus_bus_get_private DBus().dbus_bus_get_private
#define dbus_connection_close DBus().dbus_connection_close
#define dbus_connection_flush DBus().dbus_connection_flush
#define dbus_connection_read_write_dispatch \
  DBus().dbus_connection_read_write_dispatch
#define dbus_connection_register_object_path \
  DBus().dbus_connection_register_object_path
#define dbus_connection_send DBus().dbus_connection_send
#define dbus_connection_send_with_reply DBus().dbus_connection_send_with_reply
#define dbus_connection_send_with_reply_and_block \
  DBus().dbus_connection_send_with_reply_and_block
#define dbus_connection_set_exit_on_disconnect \
  DBus().dbus_connection_set_exit_on_disconnect
#define dbus_connection_unref DBus().dbus_connection_unref
#define dbus_connection_unregister_object_path \
  DBus().dbus_connection_unregister_object_path
#define dbus_message_get_error_name DBus().dbus_message_get_error_name
#define dbus_message_get_interface DBus().dbus_message_get_interface
#define dbus_message_get_member DBus().dbus_message_get_member
#define dbus_message_get_type DBus().dbus_message_get_type
#define dbus_message_iter_append_basic DBus().dbus_message_iter_append_basic
#define dbus_message_iter_close_container \
  DBus().dbus_message_iter_close_container
#define dbus_message_iter_get_arg_type DBus().dbus_message_iter_get_arg_type
#define dbus_message_iter_get_basic DBus().dbus_message_iter_get_basic
#define dbus_message_iter_init DBus().dbus_message_iter_init
#define dbus_message_iter_init_append DBus().dbus_message_iter_init_append
#define dbus_message_iter_next DBus().dbus_message_iter_next
#define dbus_message_iter_open_container DBus().dbus_message_iter_open_container
#define dbus_message_iter_recurse DBus().dbus_message_iter_recurse
#define dbus_message_new_error DBus().dbus_message_new_error
#define dbus_message_new_method_call DBus().dbus_message_new_method_call
#define dbus_message_new_method_return DBus().dbus_message_new_method_return
#define dbus_pending_call_get_completed DBus().dbus_pending_call_get_completed
#define dbus_pending_call_steal_reply DBus().dbus_pending_call_steal_reply

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

bool IsBluetoothAddress(std::string_view address) {
  if (address.size() != 17) {
    return false;
  }

  for (int i = 0; i < 6; ++i) {
    size_t pos = static_cast<size_t>(i * 3);
    if (i < 5 && address[pos + 2] != ':') {
      return false;
    }
    if (HexDigit(address[pos]) < 0 || HexDigit(address[pos + 1]) < 0) {
      return false;
    }
  }
  return true;
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
    if (DBusApi::Get(error) == nullptr) {
      return false;
    }

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

}  // namespace

BluetoothLEDeviceScanResult ScanBlueZDevices(
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
  result.status =
      std::format("Discovered {} Bluetooth LE devices", result.devices.size());
  return result;
}

BluetoothLEPairingResult PairBlueZDevice(std::string_view target) {
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

}  // namespace wpi::net::linuxbluetooth
