/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <algorithm>

#include <wpi/DenseMap.h>
#include <wpi/mutex.h>

#include "../PortsInternal.h"
#include "I2CCommon.h"
#include "hal/Errors.h"
#include "hal/I2CTypes.h"
#include "hal/simulation/I2CData.h"

using namespace hal;
using namespace hal::sim;

namespace {

class SimI2CBus : public I2CBus {
 public:
  bool OpenBus(int32_t* status) override { return true; }
  void CloseBus() override {}

  bool Initialize(int32_t deviceAddress, int32_t* status) override;
  int32_t Transaction(int32_t deviceAddress, const uint8_t* dataToSend,
                      int32_t sendSize, uint8_t* dataReceived,
                      int32_t receiveSize) override;
  int32_t Write(int32_t deviceAddress, const uint8_t* dataToSend,
                int32_t sendSize) override;
  int32_t Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) override;

  wpi::mutex m_mutex;
  wpi::SmallDenseMap<int32_t, std::unique_ptr<I2CDevice>> m_devices;
};

// C++ I2CDevice wrapper for HALSIM_I2CDevice
class CI2CDevice : public I2CDevice {
 public:
  explicit CI2CDevice(const HALSIM_I2CDevice* impl, void* data)
      : m_impl(impl), m_data(data) {}
  CI2CDevice(const CI2CDevice&) = delete;
  CI2CDevice& operator=(const CI2CDevice&) = delete;

  ~CI2CDevice() { m_impl->ReleaseData(m_data); }

  bool Initialize(int32_t* status) override {
    return m_impl->Initialize(m_data, status);
  }

  int32_t Transaction(const uint8_t* dataToSend, int32_t sendSize,
                      uint8_t* dataReceived, int32_t receiveSize) override {
    return m_impl->Transaction(m_data, dataToSend, sendSize, dataReceived,
                               receiveSize);
  }

  int32_t Write(const uint8_t* dataToSend, int32_t sendSize) override {
    return m_impl->Write(m_data, dataToSend, sendSize);
  }

  int32_t Read(uint8_t* buffer, int32_t count) override {
    return m_impl->Read(m_data, buffer, count);
  }

 private:
  const HALSIM_I2CDevice* m_impl;
  void* m_data;
};

}  // namespace

bool SimI2CBus::Initialize(int32_t deviceAddress, int32_t* status) {
  std::scoped_lock lock(m_mutex);
  auto it = m_devices.find(deviceAddress);
  if (it == m_devices.end()) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  return it->second->Initialize(status);
}

int32_t SimI2CBus::Transaction(int32_t deviceAddress, const uint8_t* dataToSend,
                               int32_t sendSize, uint8_t* dataReceived,
                               int32_t receiveSize) {
  std::scoped_lock lock(m_mutex);
  auto it = m_devices.find(deviceAddress);
  if (it == m_devices.end()) return -1;
  return it->second->Transaction(dataToSend, sendSize, dataReceived,
                                 receiveSize);
}

int32_t SimI2CBus::Write(int32_t deviceAddress, const uint8_t* dataToSend,
                         int32_t sendSize) {
  std::scoped_lock lock(m_mutex);
  auto it = m_devices.find(deviceAddress);
  if (it == m_devices.end()) return -1;
  return it->second->Write(dataToSend, sendSize);
}

int32_t SimI2CBus::Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) {
  std::scoped_lock lock(m_mutex);
  auto it = m_devices.find(deviceAddress);
  if (it == m_devices.end()) return -1;
  return it->second->Read(buffer, count);
}

// HAL_I2CDevice wrapper for C++ I2CDevice
static void CppI2CDeviceWrapper_ReleaseData(void* data) {
  delete static_cast<I2CDevice*>(data);
}

static HAL_Bool CppI2CDeviceWrapper_Initialize(void* data, int32_t* status) {
  return static_cast<I2CDevice*>(data)->Initialize(status);
}

static int32_t CppI2CDeviceWrapper_Transaction(void* data,
                                               const uint8_t* dataToSend,
                                               int32_t sendSize,
                                               uint8_t* dataReceived,
                                               int32_t receiveSize) {
  return static_cast<I2CDevice*>(data)->Transaction(dataToSend, sendSize,
                                                    dataReceived, receiveSize);
}

static int32_t CppI2CDeviceWrapper_Write(void* data, const uint8_t* dataToSend,
                                         int32_t sendSize) {
  return static_cast<I2CDevice*>(data)->Write(dataToSend, sendSize);
}

static int32_t CppI2CDeviceWrapper_Read(void* data, uint8_t* buffer,
                                        int32_t count) {
  return static_cast<I2CDevice*>(data)->Read(buffer, count);
}

static const HALSIM_I2CDevice cppI2CDeviceWrapper{
    CppI2CDeviceWrapper_ReleaseData, CppI2CDeviceWrapper_Initialize,
    CppI2CDeviceWrapper_Transaction, CppI2CDeviceWrapper_Write,
    CppI2CDeviceWrapper_Read,
};

namespace hal {
namespace init {
void InitializeI2CData() {
  // mimic the RoboRIO by default
  RegisterI2CBus(HAL_I2C_RIO_kOnboard, GetSimI2CBus());
  RegisterI2CBus(HAL_I2C_RIO_kMXP, GetSimI2CBus());
}
}  // namespace init
}  // namespace hal

std::unique_ptr<I2CBus> hal::sim::GetSimI2CBus() {
  return std::make_unique<SimI2CBus>();
}

bool hal::sim::RegisterI2CDevice(int32_t bus, int32_t deviceAddress,
                                 std::unique_ptr<I2CDevice> impl) {
  auto simBus = dynamic_cast<SimI2CBus*>(GetRegisteredI2CBus(bus));

  // not simulated bus
  if (!simBus) return false;

  std::scoped_lock lock(simBus->m_mutex);
  return simBus->m_devices.try_emplace(deviceAddress, std::move(impl)).second;
}

std::unique_ptr<I2CDevice> hal::sim::UnregisterI2CDevice(
    int32_t bus, int32_t deviceAddress) {
  auto simBus = dynamic_cast<SimI2CBus*>(GetRegisteredI2CBus(bus));

  // not simulated bus
  if (!simBus) return nullptr;

  std::scoped_lock lock(simBus->m_mutex);
  auto it = simBus->m_devices.find(deviceAddress);
  if (it == simBus->m_devices.end()) return nullptr;

  auto simDev = std::move(it->second);
  simBus->m_devices.erase(it);
  return simDev;
}

extern "C" {

const HAL_I2CBus* HALSIM_GetSimI2CBus(void** data) {
  auto cppBus = GetSimI2CBus();
  if (!cppBus) return nullptr;
  *data = cppBus.release();
  return &cppI2CBusWrapper;
}

HAL_Bool HALSIM_RegisterI2CDevice(int32_t bus, int32_t deviceAddress,
                                  void* data, const HALSIM_I2CDevice* impl) {
  // avoid double-wrapping C++ wrapper
  if (impl == &cppI2CDeviceWrapper)
    return RegisterI2CDevice(
        bus, deviceAddress,
        std::unique_ptr<I2CDevice>{static_cast<I2CDevice*>(data)});
  else
    return RegisterI2CDevice(bus, deviceAddress,
                             std::make_unique<CI2CDevice>(impl, data));
}

const HALSIM_I2CDevice* HALSIM_UnregisterI2CDevice(int32_t bus,
                                                   int32_t deviceAddress,
                                                   void** data) {
  auto cppDev = UnregisterI2CDevice(bus, deviceAddress);
  if (!cppDev) return nullptr;
  *data = cppDev.release();
  return &cppI2CDeviceWrapper;
}

}  // extern "C"
