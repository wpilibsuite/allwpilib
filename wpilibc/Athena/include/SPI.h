/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "SensorBase.h"

class DigitalOutput;
class DigitalInput;

/**
 * SPI bus interface class.
 *
 * This class is intended to be used by sensor (and other SPI device) drivers.
 * It probably should not be used directly.
 *
 */
class SPI : public SensorBase {
 public:
  enum Port { kOnboardCS0, kOnboardCS1, kOnboardCS2, kOnboardCS3, kMXP };
  SPI(Port SPIport);
  virtual ~SPI();

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

  virtual int32_t Write(uint8_t* data, uint8_t size);
  virtual int32_t Read(bool initiate, uint8_t* dataReceived, uint8_t size);
  virtual int32_t Transaction(uint8_t* dataToSend, uint8_t* dataReceived,
                              uint8_t size);

  void InitAccumulator(double period, uint32_t cmd, uint8_t xfer_size,
                       uint32_t valid_mask, uint32_t valid_value,
                       uint8_t data_shift, uint8_t data_size, bool is_signed,
                       bool big_endian);
  void FreeAccumulator();
  void ResetAccumulator();
  void SetAccumulatorCenter(int32_t center);
  void SetAccumulatorDeadband(int32_t deadband);
  int32_t GetAccumulatorLastValue() const;
  int64_t GetAccumulatorValue() const;
  uint32_t GetAccumulatorCount() const;
  double GetAccumulatorAverage() const;
  void GetAccumulatorOutput(int64_t &value, uint32_t &count) const;

 protected:
  uint8_t m_port;
  bool m_msbFirst = false; // default little-endian
  bool m_sampleOnTrailing = false; // default data updated on falling edge
  bool m_clk_idle_high = false; // default clock active high

 private:
  void Init();
};
