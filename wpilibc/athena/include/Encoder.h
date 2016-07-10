/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "Counter.h"
#include "CounterBase.h"
#include "HAL/Encoder.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "PIDSource.h"
#include "SensorBase.h"

class DigitalSource;
class DigitalGlitchFilter;

/**
 * Class to read quad encoders.
 * Quadrature encoders are devices that count shaft rotation and can sense
 * direction. The output of the QuadEncoder class is an integer that can count
 * either up or down, and can go negative for reverse direction counting. When
 * creating QuadEncoders, a direction is supplied that changes the sense of the
 * output to make code more readable if the encoder is mounted such that forward
 * movement generates negative values. Quadrature encoders have two digital
 * outputs, an A Channel and a B Channel that are out of phase with each other
 * to allow the FPGA to do direction sensing.
 *
 * All encoders will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Encoder : public SensorBase,
                public CounterBase,
                public PIDSource,
                public LiveWindowSendable {
 public:
  enum IndexingType {
    kResetWhileHigh,
    kResetWhileLow,
    kResetOnFallingEdge,
    kResetOnRisingEdge
  };

  Encoder(uint32_t aChannel, uint32_t bChannel, bool reverseDirection = false,
          EncodingType encodingType = k4X);
  Encoder(std::shared_ptr<DigitalSource> aSource,
          std::shared_ptr<DigitalSource> bSource, bool reverseDirection = false,
          EncodingType encodingType = k4X);
  Encoder(DigitalSource* aSource, DigitalSource* bSource,
          bool reverseDirection = false, EncodingType encodingType = k4X);
  Encoder(DigitalSource& aSource, DigitalSource& bSource,
          bool reverseDirection = false, EncodingType encodingType = k4X);
  virtual ~Encoder();

  // CounterBase interface
  int32_t Get() const override;
  int32_t GetRaw() const;
  int32_t GetEncodingScale() const;
  void Reset() override;
  double GetPeriod() const override;
  void SetMaxPeriod(double maxPeriod) override;
  bool GetStopped() const override;
  bool GetDirection() const override;

  float GetDistance() const;
  float GetRate() const;
  void SetMinRate(float minRate);
  void SetDistancePerPulse(float distancePerPulse);
  void SetReverseDirection(bool reverseDirection);
  void SetSamplesToAverage(int samplesToAverage);
  int GetSamplesToAverage() const;
  double PIDGet() override;

  void SetIndexSource(uint32_t channel, IndexingType type = kResetOnRisingEdge);
  DEPRECATED("Use pass-by-reference instead.")
  void SetIndexSource(DigitalSource* source,
                      IndexingType type = kResetOnRisingEdge);
  void SetIndexSource(const DigitalSource& source,
                      IndexingType type = kResetOnRisingEdge);

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  int32_t GetFPGAIndex() const;

 private:
  void InitEncoder(bool reverseDirection, EncodingType encodingType);

  float DecodingScaleFactor() const;

  std::shared_ptr<DigitalSource> m_aSource;  // the A phase of the quad encoder
  std::shared_ptr<DigitalSource> m_bSource;  // the B phase of the quad encoder
  std::unique_ptr<DigitalSource> m_indexSource = nullptr;
  HAL_EncoderHandle m_encoder = HAL_kInvalidHandle;

  std::shared_ptr<ITable> m_table;
  friend class DigitalGlitchFilter;
};
