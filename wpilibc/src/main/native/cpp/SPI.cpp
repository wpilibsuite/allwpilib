/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SPI.h"

#include <cstring>

#include <HAL/HAL.h>
#include <HAL/SPI.h>
#include <llvm/SmallVector.h>
#include <support/mutex.h>

#include "DigitalSource.h"
#include "Notifier.h"
#include "WPIErrors.h"

using namespace frc;

static constexpr int kAccumulateDepth = 2048;

class SPI::Accumulator {
 public:
  Accumulator(HAL_SPIPort port, int xferSize, int validMask, int validValue,
              int dataShift, int dataSize, bool isSigned, bool bigEndian)
      : m_notifier([=]() {
          std::lock_guard<wpi::mutex> lock(m_mutex);
          Update();
        }),
        m_buf(new uint8_t[xferSize * kAccumulateDepth]),
        m_validMask(validMask),
        m_validValue(validValue),
        m_dataMax(1 << dataSize),
        m_dataMsbMask(1 << (dataSize - 1)),
        m_dataShift(dataShift),
        m_xferSize(xferSize),
        m_isSigned(isSigned),
        m_bigEndian(bigEndian),
        m_port(port) {}
  ~Accumulator() { delete[] m_buf; }

  void Update();

  Notifier m_notifier;
  uint8_t* m_buf;
  wpi::mutex m_mutex;

  int64_t m_value = 0;
  uint32_t m_count = 0;
  int32_t m_lastValue = 0;

  int32_t m_center = 0;
  int32_t m_deadband = 0;

  int32_t m_validMask;
  int32_t m_validValue;
  int32_t m_dataMax;      // one more than max data value
  int32_t m_dataMsbMask;  // data field MSB mask (for signed)
  uint8_t m_dataShift;    // data field shift right amount, in bits
  int32_t m_xferSize;     // SPI transfer size, in bytes
  bool m_isSigned;        // is data field signed?
  bool m_bigEndian;       // is response big endian?
  HAL_SPIPort m_port;
};

void SPI::Accumulator::Update() {
  bool done;
  do {
    done = true;
    int32_t status = 0;

    // get amount of data available
    int32_t numToRead =
        HAL_ReadSPIAutoReceivedData(m_port, m_buf, 0, 0, &status);
    if (status != 0) return;  // error reading

    // only get whole responses
    numToRead -= numToRead % m_xferSize;
    if (numToRead > m_xferSize * kAccumulateDepth) {
      numToRead = m_xferSize * kAccumulateDepth;
      done = false;
    }
    if (numToRead == 0) return;  // no samples

    // read buffered data
    HAL_ReadSPIAutoReceivedData(m_port, m_buf, numToRead, 0, &status);
    if (status != 0) return;  // error reading

    // loop over all responses
    for (int32_t off = 0; off < numToRead; off += m_xferSize) {
      // convert from bytes
      uint32_t resp = 0;
      if (m_bigEndian) {
        for (int32_t i = 0; i < m_xferSize; ++i) {
          resp <<= 8;
          resp |= m_buf[off + i] & 0xff;
        }
      } else {
        for (int32_t i = m_xferSize - 1; i >= 0; --i) {
          resp <<= 8;
          resp |= m_buf[off + i] & 0xff;
        }
      }

      // process response
      if ((resp & m_validMask) == static_cast<uint32_t>(m_validValue)) {
        // valid sensor data; extract data field
        int32_t data = static_cast<int32_t>(resp >> m_dataShift);
        data &= m_dataMax - 1;
        // 2s complement conversion if signed MSB is set
        if (m_isSigned && (data & m_dataMsbMask) != 0) data -= m_dataMax;
        // center offset
        data -= m_center;
        // only accumulate if outside deadband
        if (data < -m_deadband || data > m_deadband) m_value += data;
        ++m_count;
        m_lastValue = data;
      } else {
        // no data from the sensor; just clear the last value
        m_lastValue = 0;
      }
    }
  } while (!done);
}

/**
 * Constructor
 *
 * @param port the physical SPI port
 */
SPI::SPI(Port port) : m_port(static_cast<HAL_SPIPort>(port)) {
  int32_t status = 0;
  HAL_InitializeSPI(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_SPI, instances);
}

/**
 * Destructor.
 */
SPI::~SPI() { HAL_CloseSPI(m_port); }

/**
 * Configure the rate of the generated clock signal.
 *
 * The default value is 500,000Hz.
 * The maximum value is 4,000,000Hz.
 *
 * @param hz The clock rate in Hertz.
 */
void SPI::SetClockRate(double hz) { HAL_SetSPISpeed(m_port, hz); }

/**
 * Configure the order that bits are sent and received on the wire
 * to be most significant bit first.
 */
void SPI::SetMSBFirst() {
  m_msbFirst = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be least significant bit first.
 */
void SPI::SetLSBFirst() {
  m_msbFirst = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure that the data is stable on the falling edge and the data
 * changes on the rising edge.
 */
void SPI::SetSampleDataOnFalling() {
  m_sampleOnTrailing = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure that the data is stable on the rising edge and the data
 * changes on the falling edge.
 */
void SPI::SetSampleDataOnRising() {
  m_sampleOnTrailing = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure the clock output line to be active low.
 * This is sometimes called clock polarity high or clock idle high.
 */
void SPI::SetClockActiveLow() {
  m_clk_idle_high = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure the clock output line to be active high.
 * This is sometimes called clock polarity low or clock idle low.
 */
void SPI::SetClockActiveHigh() {
  m_clk_idle_high = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clk_idle_high);
}

/**
 * Configure the chip select line to be active high.
 */
void SPI::SetChipSelectActiveHigh() {
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveHigh(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Configure the chip select line to be active low.
 */
void SPI::SetChipSelectActiveLow() {
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveLow(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Write data to the slave device.  Blocks until there is space in the
 * output FIFO.
 *
 * If not running in output only mode, also saves the data received
 * on the MISO input during the transfer into the receive FIFO.
 */
int SPI::Write(uint8_t* data, int size) {
  int retVal = 0;
  retVal = HAL_WriteSPI(m_port, data, size);
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
 * @param initiate If true, this function pushes "0" into the transmit buffer
 *                 and initiates a transfer. If false, this function assumes
 *                 that data is already in the receive FIFO from a previous
 *                 write.
 */
int SPI::Read(bool initiate, uint8_t* dataReceived, int size) {
  int retVal = 0;
  if (initiate) {
    llvm::SmallVector<uint8_t, 32> dataToSend;
    dataToSend.resize(size);
    retVal = HAL_TransactionSPI(m_port, dataToSend.data(), dataReceived, size);
  } else {
    retVal = HAL_ReadSPI(m_port, dataReceived, size);
  }
  return retVal;
}

/**
 * Perform a simultaneous read/write transaction with the device
 *
 * @param dataToSend   The data to be written out to the device
 * @param dataReceived Buffer to receive data from the device
 * @param size         The length of the transaction, in bytes
 */
int SPI::Transaction(uint8_t* dataToSend, uint8_t* dataReceived, int size) {
  int retVal = 0;
  retVal = HAL_TransactionSPI(m_port, dataToSend, dataReceived, size);
  return retVal;
}

/**
 * Initialize automatic SPI transfer engine.
 *
 * Only a single engine is available, and use of it blocks use of all other
 * chip select usage on the same physical SPI port while it is running.
 *
 * @param bufferSize buffer size in bytes
 */
void SPI::InitAuto(int bufferSize) {
  int32_t status = 0;
  HAL_InitSPIAuto(m_port, bufferSize, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Frees the automatic SPI transfer engine.
 */
void SPI::FreeAuto() {
  int32_t status = 0;
  HAL_FreeSPIAuto(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the data to be transmitted by the engine.
 *
 * Up to 16 bytes are configurable, and may be followed by up to 127 zero
 * bytes.
 *
 * @param dataToSend data to send (maximum 16 bytes)
 * @param zeroSize number of zeros to send after the data
 */
void SPI::SetAutoTransmitData(llvm::ArrayRef<uint8_t> dataToSend,
                              int zeroSize) {
  int32_t status = 0;
  HAL_SetSPIAutoTransmitData(m_port, dataToSend.data(), dataToSend.size(),
                             zeroSize, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Start running the automatic SPI transfer engine at a periodic rate.
 *
 * InitAuto() and SetAutoTransmitData() must be called before calling this
 * function.
 *
 * @param period period between transfers, in seconds (us resolution)
 */
void SPI::StartAutoRate(double period) {
  int32_t status = 0;
  HAL_StartSPIAutoRate(m_port, period, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

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
void SPI::StartAutoTrigger(DigitalSource& source, bool rising, bool falling) {
  int32_t status = 0;
  HAL_StartSPIAutoTrigger(
      m_port, source.GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)source.GetAnalogTriggerTypeForRouting(), rising,
      falling, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Stop running the automatic SPI transfer engine.
 */
void SPI::StopAuto() {
  int32_t status = 0;
  HAL_StopSPIAuto(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Force the engine to make a single transfer.
 */
void SPI::ForceAutoRead() {
  int32_t status = 0;
  HAL_ForceSPIAutoRead(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Read data that has been transferred by the automatic SPI transfer engine.
 *
 * Transfers may be made a byte at a time, so it's necessary for the caller
 * to handle cases where an entire transfer has not been completed.
 *
 * Blocks until numToRead bytes have been read or timeout expires.
 * May be called with numToRead=0 to retrieve how many bytes are available.
 *
 * @param buffer buffer where read bytes are stored
 * @param numToRead number of bytes to read
 * @param timeout timeout in seconds (ms resolution)
 * @return Number of bytes remaining to be read
 */
int SPI::ReadAutoReceivedData(uint8_t* buffer, int numToRead, double timeout) {
  int32_t status = 0;
  int32_t val =
      HAL_ReadSPIAutoReceivedData(m_port, buffer, numToRead, timeout, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

/**
 * Get the number of bytes dropped by the automatic SPI transfer engine due
 * to the receive buffer being full.
 *
 * @return Number of bytes dropped
 */
int SPI::GetAutoDroppedCount() {
  int32_t status = 0;
  int32_t val = HAL_GetSPIAutoDroppedCount(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

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
void SPI::InitAccumulator(double period, int cmd, int xferSize, int validMask,
                          int validValue, int dataShift, int dataSize,
                          bool isSigned, bool bigEndian) {
  InitAuto(xferSize * kAccumulateDepth);
  uint8_t cmdBytes[4] = {0, 0, 0, 0};
  if (bigEndian) {
    for (int32_t i = xferSize - 1; i >= 0; --i) {
      cmdBytes[i] = cmd & 0xff;
      cmd >>= 8;
    }
  } else {
    cmdBytes[0] = cmd & 0xff;
    cmd >>= 8;
    cmdBytes[1] = cmd & 0xff;
    cmd >>= 8;
    cmdBytes[2] = cmd & 0xff;
    cmd >>= 8;
    cmdBytes[3] = cmd & 0xff;
  }
  SetAutoTransmitData(cmdBytes, xferSize - 4);
  StartAutoRate(period);

  m_accum.reset(new Accumulator(m_port, xferSize, validMask, validValue,
                                dataShift, dataSize, isSigned, bigEndian));
  m_accum->m_notifier.StartPeriodic(period * kAccumulateDepth / 2);
}

/**
 * Frees the accumulator.
 */
void SPI::FreeAccumulator() {
  m_accum.reset(nullptr);
  FreeAuto();
}

/**
 * Resets the accumulator to zero.
 */
void SPI::ResetAccumulator() {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_value = 0;
  m_accum->m_count = 0;
  m_accum->m_lastValue = 0;
}

/**
 * Set the center value of the accumulator.
 *
 * The center value is subtracted from each value before it is added to the
 * accumulator. This is used for the center value of devices like gyros and
 * accelerometers to make integration work and to take the device offset into
 * account when integrating.
 */
void SPI::SetAccumulatorCenter(int center) {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_center = center;
}

/**
 * Set the accumulator's deadband.
 */
void SPI::SetAccumulatorDeadband(int deadband) {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_deadband = deadband;
}

/**
 * Read the last value read by the accumulator engine.
 */
int SPI::GetAccumulatorLastValue() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_lastValue;
}

/**
 * Read the accumulated value.
 *
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t SPI::GetAccumulatorValue() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_value;
}

/**
 * Read the number of accumulated values.
 *
 * Read the count of the accumulated values since the accumulator was last
 * Reset().
 *
 * @return The number of times samples from the channel were accumulated.
 */
int64_t SPI::GetAccumulatorCount() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_count;
}

/**
 * Read the average of the accumulated value.
 *
 * @return The accumulated average value (value / count).
 */
double SPI::GetAccumulatorAverage() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  if (m_accum->m_count == 0) return 0.0;
  return static_cast<double>(m_accum->m_value) / m_accum->m_count;
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
void SPI::GetAccumulatorOutput(int64_t& value, int64_t& count) const {
  if (!m_accum) {
    value = 0;
    count = 0;
    return;
  }
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  value = m_accum->m_value;
  count = m_accum->m_count;
}
