/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SPI.h"

#include <cstring>
#include <utility>

#include <hal/HAL.h>
#include <hal/SPI.h>
#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

#include "frc/DigitalSource.h"
#include "frc/Notifier.h"
#include "frc/WPIErrors.h"

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
        m_buf(new uint32_t[(xferSize + 1) * kAccumulateDepth]),
        m_validMask(validMask),
        m_validValue(validValue),
        m_dataMax(1 << dataSize),
        m_dataMsbMask(1 << (dataSize - 1)),
        m_dataShift(dataShift),
        m_xferSize(xferSize + 1),  // +1 for timestamp
        m_isSigned(isSigned),
        m_bigEndian(bigEndian),
        m_port(port) {}
  ~Accumulator() { delete[] m_buf; }

  void Update();

  Notifier m_notifier;
  uint32_t* m_buf;
  wpi::mutex m_mutex;

  int64_t m_value = 0;
  uint32_t m_count = 0;
  int32_t m_lastValue = 0;
  uint32_t m_lastTimestamp = 0;
  double m_integratedValue = 0;

  int32_t m_center = 0;
  int32_t m_deadband = 0;
  double m_integratedCenter = 0;

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

    // only get whole responses; +1 is for timestamp
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
      // get timestamp from first word
      uint32_t timestamp = m_buf[off];

      // convert from bytes
      uint32_t resp = 0;
      if (m_bigEndian) {
        for (int32_t i = 1; i < m_xferSize; ++i) {
          resp <<= 8;
          resp |= m_buf[off + i] & 0xff;
        }
      } else {
        for (int32_t i = m_xferSize - 1; i >= 1; --i) {
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
        int32_t dataNoCenter = data;
        data -= m_center;
        // only accumulate if outside deadband
        if (data < -m_deadband || data > m_deadband) {
          m_value += data;
          if (m_count != 0) {
            // timestamps use the 1us FPGA clock; also handle rollover
            if (timestamp >= m_lastTimestamp)
              m_integratedValue +=
                  dataNoCenter *
                      static_cast<int32_t>(timestamp - m_lastTimestamp) * 1e-6 -
                  m_integratedCenter;
            else
              m_integratedValue +=
                  dataNoCenter *
                      static_cast<int32_t>((1ULL << 32) - m_lastTimestamp +
                                           timestamp) *
                      1e-6 -
                  m_integratedCenter;
          }
        }
        ++m_count;
        m_lastValue = data;
      } else {
        // no data from the sensor; just clear the last value
        m_lastValue = 0;
      }
      m_lastTimestamp = timestamp;
    }
  } while (!done);
}

SPI::SPI(Port port) : m_port(static_cast<HAL_SPIPort>(port)) {
  int32_t status = 0;
  HAL_InitializeSPI(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_SPI, instances);
}

SPI::~SPI() { HAL_CloseSPI(m_port); }

SPI::SPI(SPI&& rhs)
    : ErrorBase(std::move(rhs)),
      m_msbFirst(std::move(rhs.m_msbFirst)),
      m_sampleOnTrailing(std::move(rhs.m_sampleOnTrailing)),
      m_clockIdleHigh(std::move(rhs.m_clockIdleHigh)),
      m_accum(std::move(rhs.m_accum)) {
  std::swap(m_port, rhs.m_port);
}

SPI& SPI::operator=(SPI&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  std::swap(m_port, rhs.m_port);
  m_msbFirst = std::move(rhs.m_msbFirst);
  m_sampleOnTrailing = std::move(rhs.m_sampleOnTrailing);
  m_clockIdleHigh = std::move(rhs.m_clockIdleHigh);
  m_accum = std::move(rhs.m_accum);

  return *this;
}

void SPI::SetClockRate(double hz) { HAL_SetSPISpeed(m_port, hz); }

void SPI::SetMSBFirst() {
  m_msbFirst = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetLSBFirst() {
  m_msbFirst = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetSampleDataOnLeadingEdge() {
  m_sampleOnTrailing = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetSampleDataOnTrailingEdge() {
  m_sampleOnTrailing = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetSampleDataOnFalling() {
  m_sampleOnTrailing = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetSampleDataOnRising() {
  m_sampleOnTrailing = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetClockActiveLow() {
  m_clockIdleHigh = true;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetClockActiveHigh() {
  m_clockIdleHigh = false;
  HAL_SetSPIOpts(m_port, m_msbFirst, m_sampleOnTrailing, m_clockIdleHigh);
}

void SPI::SetChipSelectActiveHigh() {
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveHigh(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::SetChipSelectActiveLow() {
  int32_t status = 0;
  HAL_SetSPIChipSelectActiveLow(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int SPI::Write(uint8_t* data, int size) {
  int retVal = 0;
  retVal = HAL_WriteSPI(m_port, data, size);
  return retVal;
}

int SPI::Read(bool initiate, uint8_t* dataReceived, int size) {
  int retVal = 0;
  if (initiate) {
    wpi::SmallVector<uint8_t, 32> dataToSend;
    dataToSend.resize(size);
    retVal = HAL_TransactionSPI(m_port, dataToSend.data(), dataReceived, size);
  } else {
    retVal = HAL_ReadSPI(m_port, dataReceived, size);
  }
  return retVal;
}

int SPI::Transaction(uint8_t* dataToSend, uint8_t* dataReceived, int size) {
  int retVal = 0;
  retVal = HAL_TransactionSPI(m_port, dataToSend, dataReceived, size);
  return retVal;
}

void SPI::InitAuto(int bufferSize) {
  int32_t status = 0;
  HAL_InitSPIAuto(m_port, bufferSize, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::FreeAuto() {
  int32_t status = 0;
  HAL_FreeSPIAuto(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::SetAutoTransmitData(wpi::ArrayRef<uint8_t> dataToSend, int zeroSize) {
  int32_t status = 0;
  HAL_SetSPIAutoTransmitData(m_port, dataToSend.data(), dataToSend.size(),
                             zeroSize, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::StartAutoRate(double period) {
  int32_t status = 0;
  HAL_StartSPIAutoRate(m_port, period, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::StartAutoTrigger(DigitalSource& source, bool rising, bool falling) {
  int32_t status = 0;
  HAL_StartSPIAutoTrigger(
      m_port, source.GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)source.GetAnalogTriggerTypeForRouting(), rising,
      falling, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::StopAuto() {
  int32_t status = 0;
  HAL_StopSPIAuto(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SPI::ForceAutoRead() {
  int32_t status = 0;
  HAL_ForceSPIAutoRead(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int SPI::ReadAutoReceivedData(uint32_t* buffer, int numToRead, double timeout) {
  int32_t status = 0;
  int32_t val =
      HAL_ReadSPIAutoReceivedData(m_port, buffer, numToRead, timeout, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

int SPI::GetAutoDroppedCount() {
  int32_t status = 0;
  int32_t val = HAL_GetSPIAutoDroppedCount(m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

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

void SPI::FreeAccumulator() {
  m_accum.reset(nullptr);
  FreeAuto();
}

void SPI::ResetAccumulator() {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_value = 0;
  m_accum->m_count = 0;
  m_accum->m_lastValue = 0;
  m_accum->m_lastTimestamp = 0;
  m_accum->m_integratedValue = 0;
}

void SPI::SetAccumulatorCenter(int center) {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_center = center;
}

void SPI::SetAccumulatorDeadband(int deadband) {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_deadband = deadband;
}

int SPI::GetAccumulatorLastValue() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_lastValue;
}

int64_t SPI::GetAccumulatorValue() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_value;
}

int64_t SPI::GetAccumulatorCount() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_count;
}

double SPI::GetAccumulatorAverage() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  if (m_accum->m_count == 0) return 0.0;
  return static_cast<double>(m_accum->m_value) / m_accum->m_count;
}

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

void SPI::SetAccumulatorIntegratedCenter(double center) {
  if (!m_accum) return;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->m_integratedCenter = center;
}

double SPI::GetAccumulatorIntegratedValue() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  return m_accum->m_integratedValue;
}

double SPI::GetAccumulatorIntegratedAverage() const {
  if (!m_accum) return 0;
  std::lock_guard<wpi::mutex> lock(m_accum->m_mutex);
  m_accum->Update();
  if (m_accum->m_count <= 1) return 0.0;
  // count-1 due to not integrating the first value received
  return m_accum->m_integratedValue / (m_accum->m_count - 1);
}
