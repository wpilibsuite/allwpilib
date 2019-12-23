/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/SPITypes.h"
#include "hal/Types.h"

/**
 * @defgroup hal_spi SPI Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the SPI port. Opens the port if necessary and saves the handle.
 *
 * If opening the MXP port, also sets up the channel functions appropriately.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS3, 4 for MXP
 */
void HAL_InitializeSPI(HAL_SPIPort port, int32_t* status);

/**
 * Performs an SPI send/receive transaction.
 *
 * This is a lower-level interface to the spi hardware giving you more control
 * over each transaction.
 *
 * @param port         The number of the port to use. 0-3 for Onboard CS0-CS2, 4
 * for MXP
 * @param dataToSend   Buffer of data to send as part of the transaction.
 * @param dataReceived Buffer to read data into.
 * @param size         Number of bytes to transfer. [0..7]
 * @return             Number of bytes transferred, -1 for error
 */
int32_t HAL_TransactionSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                           uint8_t* dataReceived, int32_t size);

/**
 * Executes a write transaction with the device.
 *
 * Writes to a device and wait until the transaction is complete.
 *
 * @param port      The number of the port to use. 0-3 for Onboard CS0-CS2, 4
 * for MXP
 * @param datToSend The data to write to the register on the device.
 * @param sendSize  The number of bytes to be written
 * @return          The number of bytes written. -1 for an error
 */
int32_t HAL_WriteSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                     int32_t sendSize);

/**
 * Executes a read from the device.
 *
 * This method does not write any data out to the device.
 *
 * Most spi devices will require a register address to be written before they
 * begin returning data.
 *
 * @param port   The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP
 * @param buffer A pointer to the array of bytes to store the data read from the
 * device.
 * @param count  The number of bytes to read in the transaction. [1..7]
 * @return       Number of bytes read. -1 for error.
 */
int32_t HAL_ReadSPI(HAL_SPIPort port, uint8_t* buffer, int32_t count);

/**
 * Closes the SPI port.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
 */
void HAL_CloseSPI(HAL_SPIPort port);

/**
 * Sets the clock speed for the SPI bus.
 *
 * @param port  The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP
 * @param speed The speed in Hz (0-1MHz)
 */
void HAL_SetSPISpeed(HAL_SPIPort port, int32_t speed);

/**
 * Sets the SPI options.
 *
 * @param port             The number of the port to use. 0-3 for Onboard
 * CS0-CS2, 4 for MXP
 * @param msbFirst         True to write the MSB first, False for LSB first
 * @param sampleOnTrailing True to sample on the trailing edge, False to sample
 * on the leading edge
 * @param clkIdleHigh      True to set the clock to active low, False to set the
 * clock active high
 */
void HAL_SetSPIOpts(HAL_SPIPort port, HAL_Bool msbFirst,
                    HAL_Bool sampleOnTrailing, HAL_Bool clkIdleHigh);

/**
 * Sets the CS Active high for a SPI port.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
 */
void HAL_SetSPIChipSelectActiveHigh(HAL_SPIPort port, int32_t* status);

/**
 * Sets the CS Active low for a SPI port.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
 */
void HAL_SetSPIChipSelectActiveLow(HAL_SPIPort port, int32_t* status);

/**
 * Gets the stored handle for a SPI port.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
 * @return     The stored handle for the SPI port. 0 represents no stored
 * handle.
 */
int32_t HAL_GetSPIHandle(HAL_SPIPort port);

/**
 * Sets the stored handle for a SPI port.
 *
 * @param port   The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 * @param handle The value of the handle for the port.
 */
void HAL_SetSPIHandle(HAL_SPIPort port, int32_t handle);

/**
 * Initializes the SPI automatic accumulator.
 *
 * @param port       The number of the port to use. 0-3 for Onboard CS0-CS2, 4
 * for MXP.
 * @param bufferSize The accumulator buffer size.
 */
void HAL_InitSPIAuto(HAL_SPIPort port, int32_t bufferSize, int32_t* status);

/**
 * Frees an SPI automatic accumulator.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 */
void HAL_FreeSPIAuto(HAL_SPIPort port, int32_t* status);

/**
 * Sets the period for automatic SPI accumulation.
 *
 * @param port   The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 * @param period The accumlation period (seconds).
 */
void HAL_StartSPIAutoRate(HAL_SPIPort port, double period, int32_t* status);

/**
 * Starts the auto SPI accumulator on a specific trigger.
 *
 * Note that triggering on both rising and falling edges is a valid
 * configuration.
 *
 * @param port                The number of the port to use. 0-3 for Onboard
 * CS0-CS2, 4 for MXP.
 * @param digitalSourceHandle The trigger source to use (Either
 * HAL_AnalogTriggerHandle or HAL_DigitalHandle).
 * @param analogTriggerType   The analog trigger type, if the source is an
 * analog trigger.
 * @param triggerRising       Trigger on the rising edge if true.
 * @param triggerFalling      Trigger on the falling edge if true.
 */
void HAL_StartSPIAutoTrigger(HAL_SPIPort port, HAL_Handle digitalSourceHandle,
                             HAL_AnalogTriggerType analogTriggerType,
                             HAL_Bool triggerRising, HAL_Bool triggerFalling,
                             int32_t* status);

/**
 * Stops an automatic SPI accumlation.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 */
void HAL_StopSPIAuto(HAL_SPIPort port, int32_t* status);

/**
 * Sets the data to be transmitted to the device to initiate a read.
 *
 * @param port       The number of the port to use. 0-3 for Onboard CS0-CS2, 4
 * for MXP.
 * @param dataToSend Pointer to the data to send (Gets copied for continue use,
 * so no need to keep alive).
 * @param dataSize   The length of the data to send.
 * @param zeroSize   The number of zeros to send after the data.
 */
void HAL_SetSPIAutoTransmitData(HAL_SPIPort port, const uint8_t* dataToSend,
                                int32_t dataSize, int32_t zeroSize,
                                int32_t* status);

/**
 * Immediately forces an SPI read to happen.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 */
void HAL_ForceSPIAutoRead(HAL_SPIPort port, int32_t* status);

/**
 * Reads data received by the SPI accumulator.  Each received data sequence
 * consists of a timestamp followed by the received data bytes, one byte per
 * word (in the least significant byte).  The length of each received data
 * sequence is the same as the combined dataSize + zeroSize set in
 * HAL_SetSPIAutoTransmitData.
 *
 * @param port      The number of the port to use. 0-3 for Onboard CS0-CS2, 4
 * for MXP.
 * @param buffer    The buffer to store the data into.
 * @param numToRead The number of words to read.
 * @param timeout   The read timeout (in seconds).
 * @return          The number of words actually read.
 */
int32_t HAL_ReadSPIAutoReceivedData(HAL_SPIPort port, uint32_t* buffer,
                                    int32_t numToRead, double timeout,
                                    int32_t* status);

/**
 * Gets the count of how many SPI accumulations have been missed.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 * @return     The number of missed accumulations.
 */
int32_t HAL_GetSPIAutoDroppedCount(HAL_SPIPort port, int32_t* status);

/**
 * Configure the Auto SPI Stall time between reads.
 *
 * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
 * MXP.
 * @param csToSclkTicks the number of ticks to wait before asserting the cs pin
 * @param stallTicks the number of ticks to stall for
 * @param pow2BytesPerRead the number of bytes to read before stalling
 */
void HAL_ConfigureSPIAutoStall(HAL_SPIPort port, int32_t csToSclkTicks,
                               int32_t stallTicks, int32_t pow2BytesPerRead,
                               int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
