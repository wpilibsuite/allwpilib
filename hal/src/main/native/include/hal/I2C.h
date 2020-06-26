/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <memory>
#endif

#include "hal/I2CTypes.h"
#include "hal/Types.h"

#ifdef __cplusplus
namespace hal {

class I2CBus {
 public:
  virtual ~I2CBus() = default;

  virtual bool OpenBus(int32_t* status) = 0;
  virtual void CloseBus() = 0;

  virtual bool Initialize(int32_t deviceAddress, int32_t* status) = 0;
  virtual int32_t Transaction(int32_t deviceAddress, const uint8_t* dataToSend,
                              int32_t sendSize, uint8_t* dataReceived,
                              int32_t receiveSize) = 0;
  virtual int32_t Write(int32_t deviceAddress, const uint8_t* dataToSend,
                        int32_t sendSize) = 0;
  virtual int32_t Read(int32_t deviceAddress, uint8_t* buffer,
                       int32_t count) = 0;
};

/**
 * Registers implementation for an I2C bus.
 * Duplicate registrations are rejected.
 *
 * @param bus bus number
 * @param impl bus implementation
 * @return True if registered, false if another handler already registered.
 */
bool RegisterI2CBus(int32_t bus, std::unique_ptr<I2CBus> impl);

/**
 * Gets I2C implementation for a physical hardware bus.
 *
 * @param bus physical bus number
 * @return Bus implementation, or nullptr if the hardware doesn't have that bus
 */
std::unique_ptr<I2CBus> GetSystemI2CBus(int32_t bus);

/**
 * Gets registered I2C bus.
 *
 * @param bus physical bus number
 * @return Bus implementation, or nullptr if no bus was registered.
 */
I2CBus* GetRegisteredI2CBus(int32_t bus);

/**
 * Unregisters an I2C bus.
 *
 * @param bus bus number
 * @return Bus implementation; nullptr if no bus was registered.
 */
std::unique_ptr<I2CBus> UnregisterI2CBus(int32_t bus);

}  // namespace hal
#endif  // __cplusplus

/**
 * @defgroup hal_i2c I2C Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HAL_I2CBus {
  void (*ReleaseData)(void* data);

  HAL_Bool (*OpenBus)(void* data, int32_t* status);
  void (*CloseBus)(void* data);

  HAL_Bool (*Initialize)(void* data, int32_t deviceAddress, int32_t* status);
  int32_t (*Transaction)(void* data, int32_t deviceAddress,
                         const uint8_t* dataToSend, int32_t sendSize,
                         uint8_t* dataReceived, int32_t receiveSize);
  int32_t (*Write)(void* data, int32_t deviceAddress, const uint8_t* dataToSend,
                   int32_t sendSize);
  int32_t (*Read)(void* data, int32_t deviceAddress, uint8_t* buffer,
                  int32_t count);
} HAL_I2CBus;

/**
 * Registers implementation for an I2C bus.
 * Duplicate registrations are rejected.
 *
 * @param bus bus number
 * @param data data pointer passed to implementation functions
 * @param impl bus implementation.  This pointer is kept.
 * @return True if registered, false if another handler already registered.
 */
HAL_Bool HAL_RegisterI2CBus(int32_t bus, void* data, const HAL_I2CBus* impl);

/**
 * Gets I2C implementation for a physical hardware bus.
 *
 * @param bus physical bus number
 * @param data data pointer to pass to implementation (output parameter)
 * @return Bus implementation, or NULL if the hardware doesn't have that bus
 */
const HAL_I2CBus* HAL_GetSystemI2CBus(int32_t bus, void** data);

/**
 * Gets registered I2C bus.
 *
 * @param bus bus number
 * @param data data pointer that was passed to implementation (output parameter)
 * @return Bus implementation; NULL if no bus was registered.
 */
const HAL_I2CBus* HAL_GetRegisteredI2CBus(int32_t bus, void** data);

/**
 * Unregisters an I2C bus.
 *
 * @param bus bus number
 * @param data data pointer that was passed to implementation (output parameter)
 * @return Bus implementation; NULL if no bus was registered.
 */
const HAL_I2CBus* HAL_UnregisterI2CBus(int32_t bus, void** data);

/**
 * Initializes an I2C device.
 *
 * Opens the bus if necessary.
 *
 * On the RoboRIO, if opening the MXP bus, also sets up the channel functions
 * appropriately.
 *
 * @param bus The bus number.  On the RoboRIO, the on-board bus is 2 and the
 *            MXP bus is 1 (see HAL_I2CBus enum).
 * @param deviceAddress The device address on the I2C bus
 * @return The created I2C handle
 */
HAL_I2CHandle HAL_InitializeI2C(int32_t bus, int32_t deviceAddress,
                                int32_t* status);

/**
 * Generic I2C read/write transaction.
 *
 * This is a lower-level interface to the I2C hardware giving you more control
 * over each transaction.
 *
 * @param handle The I2C handle
 * @param dataToSend Buffer of data to send as part of the transaction.
 * @param sendSize Number of bytes to send as part of the transaction.
 * @param dataReceived Buffer to read data into.
 * @param receiveSize Number of bytes to read from the device.
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_TransactionI2C(HAL_I2CHandle handle, const uint8_t* dataToSend,
                           int32_t sendSize, uint8_t* dataReceived,
                           int32_t receiveSize);

/**
 * Executes a write transaction with the device.
 *
 * Writes a single byte to a register on a device and wait until the
 *   transaction is complete.
 *
 * @param handle The I2C handle
 * @param registerAddress The address of the register on the device to be
 * written.
 * @param data The byte to write to the register on the device.
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_WriteI2C(HAL_I2CHandle handle, const uint8_t* dataToSend,
                     int32_t sendSize);

/**
 * Executes a read transaction with the device.
 *
 * Reads bytes from a device.
 * Most I2C devices will auto-increment the register pointer internally allowing
 *   you to read consecutive registers on a device in a single transaction.
 *
 * @param handle The I2C handle
 * @param registerAddress The register to read first in the transaction.
 * @param count The number of bytes to read in the transaction.
 * @param buffer A pointer to the array of bytes to store the data read from the
 * device.
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_ReadI2C(HAL_I2CHandle handle, uint8_t* buffer, int32_t count);

/**
 * Closes an I2C port
 *
 * @param handle The I2C handle
 */
void HAL_CloseI2C(HAL_I2CHandle handle);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
