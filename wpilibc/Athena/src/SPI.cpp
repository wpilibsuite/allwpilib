/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SPI.h"

#include "WPIErrors.h"
#include "HAL/Digital.hpp"

#include <string.h>

/**
 * Constructor
 *
 * @param SPIport the physical SPI port
 */
SPI::SPI(Port SPIport) {
  m_port = SPIport;
  int32_t status = 0;
  spiInitialize(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  static int32_t instances = 0;
  instances++;
  HALReport(HALUsageReporting::kResourceType_SPI, instances);
}

/**
 * Destructor.
 */
SPI::~SPI() { spiClose(m_port); }

/**
 * Configure the rate of the generated clock signal.
 *
 * The default value is 500,000Hz.
 * The maximum value is 4,000,000Hz.
 *
 * @param hz	The clock rate in Hertz.
 */
void SPI::SetClockRate(double hz) { spiSetSpeed(m_port, hz); }

/**
 * Configure the order that bits are sent and received on the wire
 * to be most significant bit first.
 */
void SPI::SetMSBFirst() {
  m_msbFirst = true;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be least significant bit first.
 */
void SPI::SetLSBFirst() {
  m_msbFirst = false;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure that the data is stable on the falling edge and the data
 * changes on the rising edge.
 */
void SPI::SetSampleDataOnFalling() {
  m_sampleOnTrailing = true;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure that the data is stable on the rising edge and the data
 * changes on the falling edge.
 */
void SPI::SetSampleDataOnRising() {
  m_sampleOnTrailing = false;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure the clock output line to be active low.
 * This is sometimes called clock polarity high or clock idle high.
 */
void SPI::SetClockActiveLow() {
  m_clk_idle_high = true;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure the clock output line to be active high.
 * This is sometimes called clock polarity low or clock idle low.
 */
void SPI::SetClockActiveHigh() {
  m_clk_idle_high = false;
  spiSetOpts(m_port, (int)m_msbFirst, (int)m_sampleOnTrailing,
             (int)m_clk_idle_high);
}

/**
 * Configure the chip select line to be active high.
 */
void SPI::SetChipSelectActiveHigh() {
  int32_t status = 0;
  spiSetChipSelectActiveHigh(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the chip select line to be active low.
 */
void SPI::SetChipSelectActiveLow() {
  int32_t status = 0;
  spiSetChipSelectActiveLow(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Write data to the slave device.  Blocks until there is space in the
 * output FIFO.
 *
 * If not running in output only mode, also saves the data received
 * on the MISO input during the transfer into the receive FIFO.
 */
int32_t SPI::Write(uint8_t* data, uint8_t size) {
  int32_t retVal = 0;
  retVal = spiWrite(m_port, data, size);
  return retVal;
}

/**
 * Read a word from the receive FIFO.
 *
 * Waits for the current transfer to complete if the receive FIFO is empty.
 *
 * If the receive FIFO is empty, there is no active transfer, and initiate
 * is false, errors.
 *
 * @param initiate	If true, this function pushes "0" into the
 * 				    transmit buffer and initiates a transfer.
 * 				    If false, this function assumes that data is
 * 				    already in the receive FIFO from a previous
 * write.
 */
int32_t SPI::Read(bool initiate, uint8_t* dataReceived, uint8_t size) {
  int32_t retVal = 0;
  if (initiate) {
    auto dataToSend = new uint8_t[size];
    memset(dataToSend, 0, size);
    retVal = spiTransaction(m_port, dataToSend, dataReceived, size);
  } else
    retVal = spiRead(m_port, dataReceived, size);
  return retVal;
}

/**
 * Perform a simultaneous read/write transaction with the device
 *
 * @param dataToSend The data to be written out to the device
 * @param dataReceived Buffer to receive data from the device
 * @param size The length of the transaction, in bytes
 */
int32_t SPI::Transaction(uint8_t* dataToSend, uint8_t* dataReceived,
                         uint8_t size) {
  int32_t retVal = 0;
  retVal = spiTransaction(m_port, dataToSend, dataReceived, size);
  return retVal;
}

/**
 * Initialize the accumulator.
 *
 * @param period Time between reads
 * @param cmd SPI command to send to request data
 * @param xfer_size SPI transfer size, in bytes
 * @param valid_mask Mask to apply to received data for validity checking
 * @param valid_data After valid_mask is applied, required matching value for
 *                   validity checking
 * @param data_shift Bit shift to apply to received data to get actual data
 *                   value
 * @param data_size Size (in bits) of data field
 * @param is_signed Is data field signed?
 * @param big_endian Is device big endian?
 */
void SPI::InitAccumulator(double period, uint32_t cmd, uint8_t xfer_size,
                          uint32_t valid_mask, uint32_t valid_value,
                          uint8_t data_shift, uint8_t data_size, bool is_signed,
                          bool big_endian) {
  int32_t status = 0;
  spiInitAccumulator(m_port, (uint32_t)(period * 1e6), cmd, xfer_size,
                     valid_mask, valid_value, data_shift, data_size, is_signed,
                     big_endian, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Frees the accumulator.
 */
void SPI::FreeAccumulator() {
  int32_t status = 0;
  spiFreeAccumulator(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Resets the accumulator to zero.
 */
void SPI::ResetAccumulator() {
  int32_t status = 0;
  spiResetAccumulator(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the center value of the accumulator.
 *
 * The center value is subtracted from each value before it is added to the accumulator. This
 * is used for the center value of devices like gyros and accelerometers to make integration work
 * and to take the device offset into account when integrating.
 */
void SPI::SetAccumulatorCenter(int32_t center) {
  int32_t status = 0;
  spiSetAccumulatorCenter(m_port, center, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the accumulator's deadband.
 */
void SPI::SetAccumulatorDeadband(int32_t deadband) {
  int32_t status = 0;
  spiSetAccumulatorDeadband(m_port, deadband, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read the last value read by the accumulator engine.
 */
int32_t SPI::GetAccumulatorLastValue() const {
  int32_t status = 0;
  int32_t retVal = spiGetAccumulatorLastValue(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Read the accumulated value.
 *
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t SPI::GetAccumulatorValue() const {
  int32_t status = 0;
  int64_t retVal = spiGetAccumulatorValue(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Read the number of accumulated values.
 *
 * Read the count of the accumulated values since the accumulator was last Reset().
 *
 * @return The number of times samples from the channel were accumulated.
 */
uint32_t SPI::GetAccumulatorCount() const {
  int32_t status = 0;
  uint32_t retVal = spiGetAccumulatorCount(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Read the average of the accumulated value.
 *
 * @return The accumulated average value (value / count).
 */
double SPI::GetAccumulatorAverage() const {
  int32_t status = 0;
  double retVal = spiGetAccumulatorAverage(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Read the accumulated value and the number of accumulated values atomically.
 *
 * This function reads the value and count atomically.
 * This can be used for averaging.
 *
 * @param value Pointer to the 64-bit accumulated output.
 * @param count Pointer to the number of accumulation cycles.
 */
void SPI::GetAccumulatorOutput(int64_t &value, uint32_t &count) const {
  int32_t status = 0;
  spiGetAccumulatorOutput(m_port, &value, &count, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}
