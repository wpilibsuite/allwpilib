// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>

#include <hal/SPITypes.h>
#include <units/time.h>
#include <wpi/ArrayRef.h>
#include <wpi/deprecated.h>

#include "frc/ErrorBase.h"

namespace frc {

class DigitalSource;

/**
 * SPI bus interface class.
 *
 * This class is intended to be used by sensor (and other SPI device) drivers.
 * It probably should not be used directly.
 *
 */
class SPI : public ErrorBase {
 public:
  enum Port { kOnboardCS0 = 0, kOnboardCS1, kOnboardCS2, kOnboardCS3, kMXP };

  /**
   * Constructor
   *
   * @param port the physical SPI port
   */
  explicit SPI(Port port);

  ~SPI() override;

  SPI(SPI&&) = default;
  SPI& operator=(SPI&&) = default;

  /**
   * Configure the rate of the generated clock signal.
   *
   * The default value is 500,000Hz.
   * The maximum value is 4,000,000Hz.
   *
   * @param hz The clock rate in Hertz.
   */
  void SetClockRate(int hz);

  /**
   * Configure the order that bits are sent and received on the wire
   * to be most significant bit first.
   */
  void SetMSBFirst();

  /**
   * Configure the order that bits are sent and received on the wire
   * to be least significant bit first.
   */
  void SetLSBFirst();

  /**
   * Configure that the data is stable on the leading edge and the data
   * changes on the trailing edge.
   */
  void SetSampleDataOnLeadingEdge();

  /**
   * Configure that the data is stable on the trailing edge and the data
   * changes on the leading edge.
   */
  void SetSampleDataOnTrailingEdge();

  /**
   * Configure that the data is stable on the falling edge and the data
   * changes on the rising edge.
   *
   * @deprecated Use SetSampleDataOnTrailingEdge() instead.
   *
   */
  WPI_DEPRECATED("Use SetSampleDataOnTrailingEdge instead.")
  void SetSampleDataOnFalling();

  /**
   * Configure that the data is stable on the rising edge and the data
   * changes on the falling edge.
   *
   * @deprecated Use SetSampleDataOnLeadingEdge() instead.
   *
   */
  WPI_DEPRECATED("Use SetSampleDataOnLeadingEdge instead")
  void SetSampleDataOnRising();

  /**
   * Configure the clock output line to be active low.
   * This is sometimes called clock polarity high or clock idle high.
   */
  void SetClockActiveLow();

  /**
   * Configure the clock output line to be active high.
   * This is sometimes called clock polarity low or clock idle low.
   */
  void SetClockActiveHigh();

  /**
   * Configure the chip select line to be active high.
   */
  void SetChipSelectActiveHigh();

  /**
   * Configure the chip select line to be active low.
   */
  void SetChipSelectActiveLow();

  /**
   * Write data to the peripheral device.  Blocks until there is space in the
   * output FIFO.
   *
   * If not running in output only mode, also saves the data received
   * on the CIPO input during the transfer into the receive FIFO.
   */
  virtual int Write(uint8_t* data, int size);

  /**
   * Read a word from the receive FIFO.
   *
   * Waits for the current transfer to complete if the receive FIFO is empty.
   *
   * If the receive FIFO is empty, there is no active transfer, and initiate
   * is false, errors.
   *
   * @param initiate If true, this function pushes "0" into the transmit buffer
   *                 and initiates a transfer. If false, this function assumes
   *                 that data is already in the receive FIFO from a previous
   *                 write.
   */
  virtual int Read(bool initiate, uint8_t* dataReceived, int size);

  /**
   * Perform a simultaneous read/write transaction with the device
   *
   * @param dataToSend   The data to be written out to the device
   * @param dataReceived Buffer to receive data from the device
   * @param size         The length of the transaction, in bytes
   */
  virtual int Transaction(uint8_t* dataToSend, uint8_t* dataReceived, int size);

  /**
   * Initialize automatic SPI transfer engine.
   *
   * Only a single engine is available, and use of it blocks use of all other
   * chip select usage on the same physical SPI port while it is running.
   *
   * @param bufferSize buffer size in bytes
   */
  void InitAuto(int bufferSize);

  /**
   * Frees the automatic SPI transfer engine.
   */
  void FreeAuto();

  /**
   * Set the data to be transmitted by the engine.
   *
   * Up to 16 bytes are configurable, and may be followed by up to 127 zero
   * bytes.
   *
   * @param dataToSend data to send (maximum 16 bytes)
   * @param zeroSize number of zeros to send after the data
   */
  void SetAutoTransmitData(wpi::ArrayRef<uint8_t> dataToSend, int zeroSize);

  /**
   * Start running the automatic SPI transfer engine at a periodic rate.
   *
   * InitAuto() and SetAutoTransmitData() must be called before calling this
   * function.
   *
   * @param period period between transfers (us resolution)
   */
  void StartAutoRate(units::second_t period);

  /**
   * Start running the automatic SPI transfer engine at a periodic rate.
   *
   * InitAuto() and SetAutoTransmitData() must be called before calling this
   * function.
   *
   * @deprecated use unit-safe StartAutoRate(units::second_t period) instead.
   *
   * @param period period between transfers, in seconds (us resolution)
   */
  WPI_DEPRECATED("Use StartAutoRate with unit-safety instead")
  void StartAutoRate(double period);

  /**
   * Start running the automatic SPI transfer engine when a trigger occurs.
   *
   * InitAuto() and SetAutoTransmitData() must be called before calling this
   * function.
   *
   * @param source digital source for the trigger (may be an analog trigger)
   * @param rising trigger on the rising edge
   * @param falling trigger on the falling edge
   */
  void StartAutoTrigger(DigitalSource& source, bool rising, bool falling);

  /**
   * Stop running the automatic SPI transfer engine.
   */
  void StopAuto();

  /**
   * Force the engine to make a single transfer.
   */
  void ForceAutoRead();

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * Transfers may be made a byte at a time, so it's necessary for the caller
   * to handle cases where an entire transfer has not been completed.
   *
   * Each received data sequence consists of a timestamp followed by the
   * received data bytes, one byte per word (in the least significant byte).
   * The length of each received data sequence is the same as the combined
   * size of the data and zeroSize set in SetAutoTransmitData().
   *
   * Blocks until numToRead words have been read or timeout expires.
   * May be called with numToRead=0 to retrieve how many words are available.
   *
   * @param buffer buffer where read words are stored
   * @param numToRead number of words to read
   * @param timeout timeout (ms resolution)
   * @return Number of words remaining to be read
   */
  int ReadAutoReceivedData(uint32_t* buffer, int numToRead,
                           units::second_t timeout);

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * Transfers may be made a byte at a time, so it's necessary for the caller
   * to handle cases where an entire transfer has not been completed.
   *
   * Each received data sequence consists of a timestamp followed by the
   * received data bytes, one byte per word (in the least significant byte).
   * The length of each received data sequence is the same as the combined
   * size of the data and zeroSize set in SetAutoTransmitData().
   *
   * Blocks until numToRead words have been read or timeout expires.
   * May be called with numToRead=0 to retrieve how many words are available.
   *
   * @deprecated Use unit safe version instead.
   *             ReadAutoReceivedData(uint32_t* buffer, int numToRead, <!--
   * -->         units::second_t timeout)
   *
   * @param buffer buffer where read words are stored
   * @param numToRead number of words to read
   * @param timeout timeout in seconds (ms resolution)
   * @return Number of words remaining to be read
   */
  WPI_DEPRECATED("Use ReadAutoReceivedData with unit-safety instead")
  int ReadAutoReceivedData(uint32_t* buffer, int numToRead, double timeout);

  /**
   * Get the number of bytes dropped by the automatic SPI transfer engine due
   * to the receive buffer being full.
   *
   * @return Number of bytes dropped
   */
  int GetAutoDroppedCount();

  /**
   * Configure the Auto SPI Stall time between reads.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for
   * MXP.
   * @param csToSclkTicks the number of ticks to wait before asserting the cs
   * pin
   * @param stallTicks the number of ticks to stall for
   * @param pow2BytesPerRead the number of bytes to read before stalling
   */
  void ConfigureAutoStall(HAL_SPIPort port, int csToSclkTicks, int stallTicks,
                          int pow2BytesPerRead);

  /**
   * Initialize the accumulator.
   *
   * @param period    Time between reads
   * @param cmd       SPI command to send to request data
   * @param xferSize  SPI transfer size, in bytes
   * @param validMask Mask to apply to received data for validity checking
   * @param validData After valid_mask is applied, required matching value for
   *                  validity checking
   * @param dataShift Bit shift to apply to received data to get actual data
   *                  value
   * @param dataSize  Size (in bits) of data field
   * @param isSigned  Is data field signed?
   * @param bigEndian Is device big endian?
   */
  void InitAccumulator(units::second_t period, int cmd, int xferSize,
                       int validMask, int validValue, int dataShift,
                       int dataSize, bool isSigned, bool bigEndian);

  /**
   * Initialize the accumulator.
   *
   * @deprecated Use unit-safe version instead.
   *             InitAccumulator(units::second_t period, int cmd, int <!--
   * -->         xferSize, int validMask, int validValue, int dataShift, <!--
   * -->         int dataSize, bool isSigned, bool bigEndian)
   *
   * @param period    Time between reads
   * @param cmd       SPI command to send to request data
   * @param xferSize  SPI transfer size, in bytes
   * @param validMask Mask to apply to received data for validity checking
   * @param validData After valid_mask is applied, required matching value for
   *                  validity checking
   * @param dataShift Bit shift to apply to received data to get actual data
   *                  value
   * @param dataSize  Size (in bits) of data field
   * @param isSigned  Is data field signed?
   * @param bigEndian Is device big endian?
   */
  WPI_DEPRECATED("Use InitAccumulator with unit-safety instead")
  void InitAccumulator(double period, int cmd, int xferSize, int validMask,
                       int validValue, int dataShift, int dataSize,
                       bool isSigned, bool bigEndian);

  /**
   * Frees the accumulator.
   */
  void FreeAccumulator();

  /**
   * Resets the accumulator to zero.
   */
  void ResetAccumulator();

  /**
   * Set the center value of the accumulator.
   *
   * The center value is subtracted from each value before it is added to the
   * accumulator. This is used for the center value of devices like gyros and
   * accelerometers to make integration work and to take the device offset into
   * account when integrating.
   */
  void SetAccumulatorCenter(int center);

  /**
   * Set the accumulator's deadband.
   */
  void SetAccumulatorDeadband(int deadband);

  /**
   * Read the last value read by the accumulator engine.
   */
  int GetAccumulatorLastValue() const;

  /**
   * Read the accumulated value.
   *
   * @return The 64-bit value accumulated since the last Reset().
   */
  int64_t GetAccumulatorValue() const;

  /**
   * Read the number of accumulated values.
   *
   * Read the count of the accumulated values since the accumulator was last
   * Reset().
   *
   * @return The number of times samples from the channel were accumulated.
   */
  int64_t GetAccumulatorCount() const;

  /**
   * Read the average of the accumulated value.
   *
   * @return The accumulated average value (value / count).
   */
  double GetAccumulatorAverage() const;

  /**
   * Read the accumulated value and the number of accumulated values atomically.
   *
   * This function reads the value and count atomically.
   * This can be used for averaging.
   *
   * @param value Pointer to the 64-bit accumulated output.
   * @param count Pointer to the number of accumulation cycles.
   */
  void GetAccumulatorOutput(int64_t& value, int64_t& count) const;

  /**
   * Set the center value of the accumulator integrator.
   *
   * The center value is subtracted from each value*dt before it is added to the
   * integrated value. This is used for the center value of devices like gyros
   * and accelerometers to take the device offset into account when integrating.
   */
  void SetAccumulatorIntegratedCenter(double center);

  /**
   * Read the integrated value.  This is the sum of (each value * time between
   * values).
   *
   * @return The integrated value accumulated since the last Reset().
   */
  double GetAccumulatorIntegratedValue() const;

  /**
   * Read the average of the integrated value.  This is the sum of (each value
   * times the time between values), divided by the count.
   *
   * @return The average of the integrated value accumulated since the last
   *         Reset().
   */
  double GetAccumulatorIntegratedAverage() const;

 protected:
  hal::SPIPort m_port;
  bool m_msbFirst = false;          // Default little-endian
  bool m_sampleOnTrailing = false;  // Default data updated on falling edge
  bool m_clockIdleHigh = false;     // Default clock active high

 private:
  void Init();

  class Accumulator;
  std::unique_ptr<Accumulator> m_accum;
};

}  // namespace frc
