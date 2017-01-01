/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include "CounterBase.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "PIDSource.h"
#include "SensorBase.h"
#include "simulation/SimEncoder.h"

namespace frc {

/**
 * Class to read quad encoders.
 *
 * Quadrature encoders are devices that count shaft rotation and can sense
 * direction. The output of the QuadEncoder class is an integer that can count
 * either up or down, and can go negative for reverse direction counting. When
 * creating QuadEncoders, a direction is supplied that changes the sense of the
 * output to make code more readable if the encoder is mounted such that
 * forward movement generates negative values. Quadrature encoders have two
 * digital outputs, an A Channel and a B Channel that are out of phase with
 * each other to allow the FPGA to do direction sensing.
 *
 * All encoders will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Encoder : public SensorBase,
                public CounterBase,
                public PIDSource,
                public LiveWindowSendable {
 public:
  Encoder(int aChannel, int bChannel, bool reverseDirection = false,
          EncodingType encodingType = k4X);
  // TODO: [Not Supported] Encoder(DigitalSource *aSource, DigitalSource
  // *bSource, bool reverseDirection=false, EncodingType encodingType = k4X);
  // TODO: [Not Supported] Encoder(DigitalSource &aSource, DigitalSource
  // &bSource, bool reverseDirection=false, EncodingType encodingType = k4X);
  virtual ~Encoder() = default;

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
  void SetPIDSourceType(PIDSourceType pidSource);
  double PIDGet() override;

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  int FPGAEncoderIndex() const { return 0; }

 private:
  void InitEncoder(int channelA, int channelB, bool reverseDirection,
                   EncodingType encodingType);
  double DecodingScaleFactor() const;

  // the A phase of the quad encoder
  // TODO: [Not Supported] DigitalSource *m_aSource;
  // the B phase of the quad encoder
  // TODO: [Not Supported] DigitalSource *m_bSource;
  // was the A source allocated locally?
  // TODO: [Not Supported] bool m_allocatedASource;
  // was the B source allocated locally?
  // TODO: [Not Supported] bool m_allocatedBSource;
  int channelA, channelB;
  double m_distancePerPulse;    // distance of travel for each encoder tick
  EncodingType m_encodingType;  // Encoding type
  int m_encodingScale;          // 1x, 2x, or 4x, per the encodingType
  bool m_reverseDirection;
  SimEncoder* impl;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
