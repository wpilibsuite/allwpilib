/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include <llvm/ArrayRef.h>

#include "ErrorBase.h"

enum HAL_SPIPort : int32_t;

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

  explicit SPI(Port port);
  ~SPI() override;

  SPI(const SPI&) = delete;
  SPI& operator=(const SPI&) = delete;

  void SetClockRate(double hz);

  void SetMSBFirst();
  void SetLSBFirst();

  void SetSampleDataOnFalling();
  void SetSampleDataOnRising();

  void SetClockActiveLow();
  void SetClockActiveHigh();

  void SetChipSelectActiveHigh();
  void SetChipSelectActiveLow();

  virtual int Write(uint8_t* data, int size);
  virtual int Read(bool initiate, uint8_t* dataReceived, int size);
  virtual int Transaction(uint8_t* dataToSend, uint8_t* dataReceived, int size);

  void InitAuto(int bufferSize);
  void FreeAuto();
  void SetAutoTransmitData(llvm::ArrayRef<uint8_t> dataToSend, int zeroSize);
  void StartAutoRate(double period);
  void StartAutoTrigger(DigitalSource& source, bool rising, bool falling);
  void StopAuto();
  void ForceAutoRead();
  int ReadAutoReceivedData(uint8_t* buffer, int numToRead, double timeout);
  int GetAutoDroppedCount();

  void InitAccumulator(double period, int cmd, int xferSize, int validMask,
                       int validValue, int dataShift, int dataSize,
                       bool isSigned, bool bigEndian);
  void FreeAccumulator();
  void ResetAccumulator();
  void SetAccumulatorCenter(int center);
  void SetAccumulatorDeadband(int deadband);
  int GetAccumulatorLastValue() const;
  int64_t GetAccumulatorValue() const;
  int64_t GetAccumulatorCount() const;
  double GetAccumulatorAverage() const;
  void GetAccumulatorOutput(int64_t& value, int64_t& count) const;

 protected:
  HAL_SPIPort m_port;
  bool m_msbFirst = false;          // Default little-endian
  bool m_sampleOnTrailing = false;  // Default data updated on falling edge
  bool m_clk_idle_high = false;     // Default clock active high

 private:
  void Init();

  class Accumulator;
  std::unique_ptr<Accumulator> m_accum;
};

}  // namespace frc
