/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "I2CCommon.h"

#include <memory>
#include <vector>

#include "CommonInitializer.h"
#include "hal/HAL.h"
#include "hal/handles/UnlimitedHandleResource.h"

using namespace hal;

namespace {

// C++ I2CBus wrapper for HAL_I2CBus
class CI2CBus : public I2CBus {
 public:
  explicit CI2CBus(const HAL_I2CBus* impl, void* data)
      : m_impl(impl), m_data(data) {}
  CI2CBus(const CI2CBus&) = delete;
  CI2CBus& operator=(const CI2CBus&) = delete;

  ~CI2CBus() {
    CloseBus();
    m_impl->ReleaseData(m_data);
  }

  bool OpenBus(int32_t* status) override {
    return m_impl->OpenBus(m_data, status);
  }

  void CloseBus() override { m_impl->CloseBus(m_data); }

  bool Initialize(int32_t deviceAddress, int32_t* status) override {
    return m_impl->Initialize(m_data, deviceAddress, status);
  }

  int32_t Transaction(int32_t deviceAddress, const uint8_t* dataToSend,
                      int32_t sendSize, uint8_t* dataReceived,
                      int32_t receiveSize) override {
    return m_impl->Transaction(m_data, deviceAddress, dataToSend, sendSize,
                               dataReceived, receiveSize);
  }

  int32_t Write(int32_t deviceAddress, const uint8_t* dataToSend,
                int32_t sendSize) override {
    return m_impl->Write(m_data, deviceAddress, dataToSend, sendSize);
  }

  int32_t Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) override {
    return m_impl->Read(m_data, deviceAddress, buffer, count);
  }

 private:
  const HAL_I2CBus* m_impl;
  void* m_data;
};

class I2CBusData {
 public:
  I2CBusData(std::unique_ptr<I2CBus> bus_, int32_t busId_)
      : bus{std::move(bus_)}, busId{busId_} {}

  std::unique_ptr<I2CBus> bus;
  int32_t busId = -1;
  int objCount = 0;
};

struct I2CDeviceData {
  I2CBusData* busData = nullptr;
  int32_t busId = -1;
  int32_t deviceAddress = -1;
};

}  // namespace

// HAL_I2CBus wrapper for C++ I2CBus
static void CppI2CBusWrapper_ReleaseData(void* data) {
  delete static_cast<I2CBus*>(data);
}

static HAL_Bool CppI2CBusWrapper_OpenBus(void* data, int32_t* status) {
  return static_cast<I2CBus*>(data)->OpenBus(status);
}

static void CppI2CBusWrapper_CloseBus(void* data) {
  static_cast<I2CBus*>(data)->CloseBus();
}

static HAL_Bool CppI2CBusWrapper_Initialize(void* data, int32_t deviceAddress,
                                            int32_t* status) {
  return static_cast<I2CBus*>(data)->Initialize(deviceAddress, status);
}

static int32_t CppI2CBusWrapper_Transaction(void* data, int32_t deviceAddress,
                                            const uint8_t* dataToSend,
                                            int32_t sendSize,
                                            uint8_t* dataReceived,
                                            int32_t receiveSize) {
  return static_cast<I2CBus*>(data)->Transaction(
      deviceAddress, dataToSend, sendSize, dataReceived, receiveSize);
}

static int32_t CppI2CBusWrapper_Write(void* data, int32_t deviceAddress,
                                      const uint8_t* dataToSend,
                                      int32_t sendSize) {
  return static_cast<I2CBus*>(data)->Write(deviceAddress, dataToSend, sendSize);
}

static int32_t CppI2CBusWrapper_Read(void* data, int32_t deviceAddress,
                                     uint8_t* buffer, int32_t count) {
  return static_cast<I2CBus*>(data)->Read(deviceAddress, buffer, count);
}

const HAL_I2CBus hal::cppI2CBusWrapper{
    CppI2CBusWrapper_ReleaseData, CppI2CBusWrapper_OpenBus,
    CppI2CBusWrapper_CloseBus,    CppI2CBusWrapper_Initialize,
    CppI2CBusWrapper_Transaction, CppI2CBusWrapper_Write,
    CppI2CBusWrapper_Read,
};

static wpi::mutex i2cBussesMutex;
static std::vector<std::unique_ptr<I2CBusData>> i2cBusses;

static UnlimitedHandleResource<HAL_I2CHandle, I2CDeviceData,
                               HAL_HandleEnum::I2C>* i2cHandles;

namespace hal {
namespace init {
void InitializeCommonI2C() {
  static UnlimitedHandleResource<HAL_I2CHandle, I2CDeviceData,
                                 HAL_HandleEnum::I2C>
      ih;
  i2cHandles = &ih;
}
}  // namespace init
}  // namespace hal

bool hal::RegisterI2CBus(int32_t bus, std::unique_ptr<I2CBus> impl) {
  std::scoped_lock lock(i2cBussesMutex);
  if (bus < 0) return false;
  if (static_cast<size_t>(bus) >= i2cBusses.size()) i2cBusses.resize(bus + 1);
  auto& elem = i2cBusses[bus];
  if (elem) return false;  // already exists

  elem = std::make_unique<I2CBusData>(std::move(impl), bus);

  // assign references
  i2cHandles->ForEach([&](auto, I2CDeviceData* dev) {
    if (dev->busId == bus) {
      dev->busData = elem.get();
      ++elem->objCount;
    }
  });

  // open device if necessary; ignore errors because we don't have another
  // option
  if (elem->objCount > 0) {
    int32_t status = 0;
    elem->bus->OpenBus(&status);
  }

  return true;
}

I2CBus* hal::GetRegisteredI2CBus(int32_t bus) {
  std::scoped_lock lock(i2cBussesMutex);
  if (bus < 0 || static_cast<size_t>(bus) >= i2cBusses.size()) return nullptr;
  auto& elem = i2cBusses[bus];
  if (!elem) return nullptr;
  return elem->bus.get();
}

std::unique_ptr<I2CBus> hal::UnregisterI2CBus(int32_t bus) {
  std::scoped_lock lock(i2cBussesMutex);
  if (bus < 0 || static_cast<size_t>(bus) >= i2cBusses.size()) return nullptr;
  auto elem = std::move(i2cBusses[bus]);

  if (elem) {
    // clear references
    i2cHandles->ForEach([&](auto, I2CDeviceData* dev) {
      if (dev->busData == elem.get()) dev->busData = nullptr;
    });

    // make sure bus is closed
    if (elem->objCount != 0) elem->bus->CloseBus();
  }

  return std::move(elem->bus);
}

extern "C" {

HAL_Bool HAL_RegisterI2CBus(int32_t bus, void* data, const HAL_I2CBus* impl) {
  // avoid double-wrapping C++ wrapper
  if (impl == &cppI2CBusWrapper)
    return RegisterI2CBus(bus,
                          std::unique_ptr<I2CBus>{static_cast<I2CBus*>(data)});
  else
    return RegisterI2CBus(bus, std::make_unique<CI2CBus>(impl, data));
}

const HAL_I2CBus* HAL_GetRegisteredI2CBus(int32_t bus, void** data) {
  auto cppBus = GetRegisteredI2CBus(bus);
  if (!cppBus) return nullptr;
  *data = cppBus;
  return &cppI2CBusWrapper;
}

const HAL_I2CBus* HAL_UnregisterI2CBus(int32_t bus, void** data) {
  auto cppBus = UnregisterI2CBus(bus);
  if (!cppBus) return nullptr;
  *data = cppBus.release();
  return &cppI2CBusWrapper;
}

HAL_I2CHandle HAL_InitializeI2C(int32_t bus, int32_t deviceAddress,
                                int32_t* status) {
  hal::init::CheckInit();

  std::scoped_lock lock(i2cBussesMutex);
  if (bus < 0 || static_cast<size_t>(bus) >= i2cBusses.size())
    return HAL_kInvalidHandle;
  auto busData = i2cBusses[bus].get();
  if (!busData) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  auto i2c = std::make_shared<I2CDeviceData>();
  HAL_I2CHandle handle = i2cHandles->Allocate(i2c);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  i2c->busData = busData;
  i2c->busId = bus;
  i2c->deviceAddress = deviceAddress;

  if (++busData->objCount == 1) {
    if (!busData->bus->OpenBus(status)) {
      i2cHandles->Free(handle);
      return HAL_kInvalidHandle;
    }
  }

  if (!busData->bus->Initialize(deviceAddress, status)) {
    i2cHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

int32_t HAL_TransactionI2C(HAL_I2CHandle handle, const uint8_t* dataToSend,
                           int32_t sendSize, uint8_t* dataReceived,
                           int32_t receiveSize) {
  auto i2c = i2cHandles->Get(handle);
  if (!i2c || !i2c->busData) return -1;
  return i2c->busData->bus->Transaction(i2c->deviceAddress, dataToSend,
                                        sendSize, dataReceived, receiveSize);
}

int32_t HAL_WriteI2C(HAL_I2CHandle handle, const uint8_t* dataToSend,
                     int32_t sendSize) {
  auto i2c = i2cHandles->Get(handle);
  if (!i2c || !i2c->busData) return -1;
  return i2c->busData->bus->Write(i2c->deviceAddress, dataToSend, sendSize);
}

int32_t HAL_ReadI2C(HAL_I2CHandle handle, uint8_t* buffer, int32_t count) {
  auto i2c = i2cHandles->Get(handle);
  if (!i2c || !i2c->busData) return -1;
  return i2c->busData->bus->Read(i2c->deviceAddress, buffer, count);
}

void HAL_CloseI2C(HAL_I2CHandle handle) {
  std::scoped_lock lock(i2cBussesMutex);
  auto i2c = i2cHandles->Free(handle);
  if (!i2c || !i2c->busData) return;
  if (i2c->busData->objCount-- == 0) i2c->busData->bus->CloseBus();
}

}  // extern "C"
