// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/UsbDeviceDetector.hpp"

#include <poll.h>

#include <atomic>
#include <cerrno>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <systemd/sd-device.h>
#include <systemd/sd-event.h>

#include "UsbDeviceDetectorImpl.hpp"

using namespace wpi::net;

namespace {

using DeviceStringGetter = int (*)(sd_device*, const char**);

std::string GetValue(sd_device* device, DeviceStringGetter getter) {
  const char* value = nullptr;
  if (getter(device, &value) < 0 || !value) {
    return {};
  }
  return value;
}

std::string GetUsbValue(sd_device* device, const char* property,
                        const char* sysattr) {
  const char* value = nullptr;
  if (sd_device_get_property_value(device, property, &value) >= 0 && value) {
    return value;
  }
  value = nullptr;
  if (sd_device_get_sysattr_value(device, sysattr, &value) >= 0 && value) {
    return value;
  }
  return {};
}

class SystemCoreUsbDeviceDetector final : public UsbDeviceDetector::Impl {
 public:
  SystemCoreUsbDeviceDetector(int port, UsbDeviceDetector::Callback connected,
                              UsbDeviceDetector::Callback disconnected)
      : m_port{port},
        m_connected{std::move(connected)},
        m_disconnected{std::move(disconnected)} {}

  ~SystemCoreUsbDeviceDetector() override { Stop(); }

  bool Start() override {
    if (m_running) {
      return true;
    }

    std::promise<bool> initialized;
    auto result = initialized.get_future();
    m_thread = std::jthread([this, initialized = std::move(initialized)](
                                std::stop_token stopToken) mutable {
      Run(stopToken, std::move(initialized));
    });
    bool success = result.get();
    if (!success) {
      m_thread.join();
      return false;
    }
    return true;
  }

  void Stop() override {
    if (!m_thread.joinable()) {
      m_running = false;
      return;
    }

    m_thread.request_stop();
    m_thread.join();
    m_running = false;
  }

  bool IsRunning() const override { return m_running; }

 private:
  void Run(std::stop_token stopToken, std::promise<bool> initialized) {
    if (!Initialize()) {
      initialized.set_value(false);
      Cleanup();
      return;
    }

    m_running = true;
    initialized.set_value(true);
    EnumerateDevices();

    pollfd monitorPoll{
        .fd = m_fd,
        .events = POLLIN,
        .revents = 0,
    };
    while (!stopToken.stop_requested()) {
      int result = poll(&monitorPoll, 1, 100);
      if (result > 0 && (monitorPoll.revents & POLLIN) != 0) {
        while (sd_event_run(m_event, 0) > 0) {
        }
      } else if (result < 0 && errno != EINTR) {
        break;
      }
    }

    Cleanup();
    m_running = false;
  }

  bool Initialize() {
    if (sd_device_monitor_new(&m_monitor) < 0 || !m_monitor) {
      return false;
    }
    if (sd_device_monitor_start(m_monitor, MonitorCallback, this) < 0) {
      Cleanup();
      return false;
    }

    m_event = sd_device_monitor_get_event(m_monitor);
    if (!m_event) {
      Cleanup();
      return false;
    }
    m_fd = sd_event_get_fd(m_event);
    if (m_fd < 0) {
      Cleanup();
      return false;
    }
    return true;
  }

  void Cleanup() {
    m_fd = -1;
    m_event = nullptr;
    if (m_monitor) {
      sd_device_monitor_unref(m_monitor);
      m_monitor = nullptr;
    }
    m_devices.clear();
  }

  void EnumerateDevices() {
    sd_device_enumerator* enumerator = nullptr;
    if (sd_device_enumerator_new(&enumerator) < 0 || !enumerator) {
      return;
    }

    for (sd_device* device = sd_device_enumerator_get_device_first(enumerator);
         device; device = sd_device_enumerator_get_device_next(enumerator)) {
      AddDevice(device);
    }
    sd_device_enumerator_unref(enumerator);
  }

  static int MonitorCallback(sd_device_monitor*, sd_device* device,
                             void* userdata) {
    auto self = static_cast<SystemCoreUsbDeviceDetector*>(userdata);
    sd_device_action_t action = _SD_DEVICE_ACTION_INVALID;
    if (sd_device_get_action(device, &action) < 0) {
      return 0;
    }

    if (action == SD_DEVICE_ADD || action == SD_DEVICE_BIND) {
      self->AddDevice(device);
    } else if (action == SD_DEVICE_REMOVE) {
      self->RemoveDevice(device);
    }
    return 0;
  }

  sd_device* FindUsbDevice(sd_device* device) {
    sd_device* current = device;
    while (current) {
      const char* subsystem = nullptr;
      const char* deviceType = nullptr;
      if (sd_device_get_subsystem(current, &subsystem) >= 0 && subsystem &&
          std::string_view{subsystem} == "usb" &&
          sd_device_get_devtype(current, &deviceType) >= 0 && deviceType &&
          std::string_view{deviceType} == "usb_device") {
        return current;
      }

      sd_device* parent = nullptr;
      if (sd_device_get_parent(current, &parent) < 0 || !parent) {
        return nullptr;
      }
      current = parent;
    }
    return nullptr;
  }

  std::optional<UsbDevice> MakeDevice(sd_device* device) {
    sd_device* usbDevice = FindUsbDevice(device);
    if (!usbDevice) {
      return std::nullopt;
    }

    const char* usbSysname = nullptr;
    if (sd_device_get_sysname(usbDevice, &usbSysname) < 0 || !usbSysname) {
      return std::nullopt;
    }
    auto port = detail::GetSystemCoreUsbPort(usbSysname);
    if (!port || *port != m_port) {
      return std::nullopt;
    }

    UsbDevice result;
    result.port = *port;
    result.syspath = GetValue(device, sd_device_get_syspath);
    result.usbSyspath = GetValue(usbDevice, sd_device_get_syspath);
    if (result.syspath.empty() || result.usbSyspath.empty()) {
      return std::nullopt;
    }

    result.subsystem = GetValue(device, sd_device_get_subsystem);
    result.deviceType = GetValue(device, sd_device_get_devtype);
    result.deviceNode = GetValue(device, sd_device_get_devname);

    // Ignore intermediate interface and driver devices. The physical USB
    // device itself and descendants with usable device nodes are reported.
    if (result.syspath != result.usbSyspath && result.deviceNode.empty()) {
      return std::nullopt;
    }

    result.vendorId = GetUsbValue(usbDevice, "ID_VENDOR_ID", "idVendor");
    result.productId = GetUsbValue(usbDevice, "ID_MODEL_ID", "idProduct");
    result.manufacturer = GetUsbValue(usbDevice, "ID_VENDOR", "manufacturer");
    result.product = GetUsbValue(usbDevice, "ID_MODEL", "product");
    result.serialNumber = GetUsbValue(usbDevice, "ID_SERIAL_SHORT", "serial");
    return result;
  }

  void AddDevice(sd_device* device) {
    auto deviceInfo = MakeDevice(device);
    if (!deviceInfo) {
      return;
    }

    auto [it, inserted] =
        m_devices.try_emplace(deviceInfo->syspath, std::move(*deviceInfo));
    if (inserted) {
      Notify(m_connected, it->second);
    }
  }

  void RemoveDevice(sd_device* device) {
    std::string syspath = GetValue(device, sd_device_get_syspath);
    auto found = m_devices.find(syspath);
    if (found == m_devices.end()) {
      return;
    }

    std::string usbSyspath = found->second.usbSyspath;
    if (syspath == usbSyspath) {
      std::vector<UsbDevice> removed;
      for (auto it = m_devices.begin(); it != m_devices.end();) {
        if (it->second.usbSyspath == usbSyspath && it->first != syspath) {
          removed.emplace_back(std::move(it->second));
          it = m_devices.erase(it);
        } else {
          ++it;
        }
      }
      for (const auto& item : removed) {
        Notify(m_disconnected, item);
      }
    }

    found = m_devices.find(syspath);
    if (found != m_devices.end()) {
      UsbDevice removed = std::move(found->second);
      m_devices.erase(found);
      Notify(m_disconnected, removed);
    }
  }

  static void Notify(const UsbDeviceDetector::Callback& callback,
                     const UsbDevice& device) {
    if (!callback) {
      return;
    }
    try {
      callback(device);
    } catch (...) {
      // Do not unwind user callbacks through the C systemd callback boundary.
    }
  }

  int m_port;
  UsbDeviceDetector::Callback m_connected;
  UsbDeviceDetector::Callback m_disconnected;
  std::jthread m_thread;
  sd_device_monitor* m_monitor = nullptr;
  sd_event* m_event = nullptr;
  int m_fd = -1;
  std::unordered_map<std::string, UsbDevice> m_devices;
  std::atomic_bool m_running = false;
};

}  // namespace

std::unique_ptr<UsbDeviceDetector::Impl> UsbDeviceDetector::CreateImpl(
    int port, Callback connected, Callback disconnected) {
  return std::make_unique<SystemCoreUsbDeviceDetector>(
      port, std::move(connected), std::move(disconnected));
}
