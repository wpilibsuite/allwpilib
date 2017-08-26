/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <HAL/Encoder.h>

#include "Counter.h"
#include "CounterBase.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "PIDSource.h"
#include "SensorBase.h"

namespace frc {

class DigitalSource;
class DigitalGlitchFilter;

/**
 * Class to read quad encoders.
 *
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

  Encoder(int aChannel, int bChannel, bool reverseDirection = false,
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
  int Get() const override;
  int GetRaw() const;
  int GetEncodingScale() const;
  void Reset() override;
  double GetPeriod() const override;
  void SetMaxPeriod(double maxPeriod) override;
  bool GetStopped() const override;
  bool GetDirection() const override;

  double GetDistance() const;
  double GetRate() const;
  void SetMinRate(double minRate);
  void SetDistancePerPulse(double distancePerPulse);
  void SetReverseDirection(bool reverseDirection);
  void SetSamplesToAverage(int samplesToAverage);
  int GetSamplesToAverage() const;
  double PIDGet() override;

  void SetIndexSource(int channel, IndexingType type = kResetOnRisingEdge);
  void SetIndexSource(const DigitalSource& source,
                      IndexingType type = kResetOnRisingEdge);

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  int GetFPGAIndex() const;

 private:
  void InitEncoder(bool reverseDirection, EncodingType encodingType);

  double DecodingScaleFactor() const;

  std::shared_ptr<DigitalSource> m_aSource;  // the A phase of the quad encoder
  std::shared_ptr<DigitalSource> m_bSource;  // the B phase of the quad encoder
  std::unique_ptr<DigitalSource> m_indexSource = nullptr;
  HAL_EncoderHandle m_encoder = HAL_kInvalidHandle;

  std::shared_ptr<ITable> m_table;
  friend class DigitalGlitchFilter;
};

}  // namespace frc
