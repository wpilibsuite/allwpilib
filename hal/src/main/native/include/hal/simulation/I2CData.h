/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef __cplusplus
#include <memory>
#endif

#include "hal/I2C.h"
#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
namespace hal {
namespace sim {

class I2CDevice {
 public:
  virtual ~I2CDevice() = default;

  virtual bool Initialize(int32_t* status) = 0;
  virtual int32_t Transaction(const uint8_t* dataToSend, int32_t sendSize,
                              uint8_t* dataReceived, int32_t receiveSize) = 0;
  virtual int32_t Write(const uint8_t* dataToSend, int32_t sendSize) = 0;
  virtual int32_t Read(uint8_t* buffer, int32_t count) = 0;
};

/**
 * Gets I2C implementation for a simulated bus.
 *
 * @return Bus implementation
 */
std::unique_ptr<I2CBus> GetSimI2CBus();

/**
 * Registers implementation for an I2C device.
 * Duplicate registrations are rejected.
 * Device implementations can only be registered on a bus that has a simulated
 * bus implementation registered.
 *
 * @param bus bus number
 * @param deviceAddress device address
 * @param impl device implementation
 * @return True if registered, false if another handler already registered.
 */
bool RegisterI2CDevice(int32_t bus, int32_t deviceAddress,
                       std::unique_ptr<I2CDevice> impl);

/**
 * Unregisters an I2C device.
 *
 * @param bus bus number
 * @param deviceAddress device address
 * @return Device implementation; NULL if no device was registered.
 */
std::unique_ptr<I2CDevice> UnregisterI2CDevice(int32_t bus,
                                               int32_t deviceAddress);

}  // namespace sim
}  // namespace hal
#endif  // __cplusplus

/**
 * @defgroup halsim_i2c Simulation I2C Functions
 * @ingroup halsim_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HALSIM_I2CDevice {
  void (*ReleaseData)(void* data);

  HAL_Bool (*Initialize)(void* data, int32_t* status);
  int32_t (*Transaction)(void* data, const uint8_t* dataToSend,
                         int32_t sendSize, uint8_t* dataReceived,
                         int32_t receiveSize);
  int32_t (*Write)(void* data, const uint8_t* dataToSend, int32_t sendSize);
  int32_t (*Read)(void* data, uint8_t* buffer, int32_t count);
} HALSIM_I2CDevice;

/**
 * Gets I2C implementation for a simulated bus.
 *
 * @param data data pointer to pass to implementation (output parameter)
 * @return Bus implementation
 */
const HAL_I2CBus* HALSIM_GetSimI2CBus(void** data);

/**
 * Registers implementation for a simulated I2C device.
 * Duplicate registrations are rejected.
 * Device implementations can only be registered on a bus that has a simulated
 * bus implementation registered.
 *
 * @param bus bus number
 * @param deviceAddress device address
 * @param data data pointer passed to implementation
 * @param impl device implementation.  This pointer is kept.
 * @return True if registered, false if another handler already registered.
 */
HAL_Bool HALSIM_RegisterI2CDevice(int32_t bus, int32_t deviceAddress,
                                  void* data, const HALSIM_I2CDevice* impl);

/**
 * Unregisters a simulated I2C device.
 *
 * @param bus bus number
 * @param deviceAddress device address
 * @param data data pointer that was passed to implementation (output parameter)
 * @return Device implementation; NULL if no device was registered.
 */
const HALSIM_I2CDevice* HALSIM_UnregisterI2CDevice(int32_t bus,
                                                   int32_t deviceAddress,
                                                   void** data);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
