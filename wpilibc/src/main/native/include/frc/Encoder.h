/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/Types.h>

#include "frc/Counter.h"
#include "frc/CounterBase.h"
#include "frc/ErrorBase.h"
#include "frc/PIDSource.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class DigitalSource;
class DigitalGlitchFilter;
class SendableBuilder;
class DMA;
class DMASample;

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
class Encoder : public ErrorBase,
                public CounterBase,
                public PIDSource,
                public Sendable,
                public SendableHelper<Encoder> {
  friend class DMA;
  friend class DMASample;

 public:
  enum IndexingType {
    kResetWhileHigh,
    kResetWhileLow,
    kResetOnFallingEdge,
    kResetOnRisingEdge
  };

  /**
   * Encoder constructor.
   *
   * Construct a Encoder given a and b channels.
   *
   * The counter will start counting immediately.
   *
   * @param aChannel         The a channel DIO channel. 0-9 are on-board, 10-25
   *                         are on the MXP port
   * @param bChannel         The b channel DIO channel. 0-9 are on-board, 10-25
   *                         are on the MXP port
   * @param reverseDirection represents the orientation of the encoder and
   *                         inverts the output values if necessary so forward
   *                         represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  Encoder(int aChannel, int bChannel, bool reverseDirection = false,
          EncodingType encodingType = k4X);

  /**
   * Encoder constructor.
   *
   * Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not
   * allocate the digital inputs and assume that they already are counted.
   *
   * The counter will start counting immediately.
   *
   * @param aSource          The source that should be used for the a channel.
   * @param bSource          the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and
   *                         inverts the output values if necessary so forward
   *                         represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  Encoder(DigitalSource* aSource, DigitalSource* bSource,
          bool reverseDirection = false, EncodingType encodingType = k4X);

  /**
   * Encoder constructor.
   *
   * Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not
   * allocate the digital inputs and assume that they already are counted.
   *
   * The counter will start counting immediately.
   *
   * @param aSource          The source that should be used for the a channel.
   * @param bSource          the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and
   *                         inverts the output values if necessary so forward
   *                         represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  Encoder(DigitalSource& aSource, DigitalSource& bSource,
          bool reverseDirection = false, EncodingType encodingType = k4X);

  Encoder(std::shared_ptr<DigitalSource> aSource,
          std::shared_ptr<DigitalSource> bSource, bool reverseDirection = false,
          EncodingType encodingType = k4X);

  ~Encoder() override;

  Encoder(Encoder&&) = default;
  Encoder& operator=(Encoder&&) = default;

  // CounterBase interface
  /**
   * Gets the current count.
   *
   * Returns the current count on the Encoder. This method compensates for the
   * decoding type.
   *
   * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale
   *         factor.
   */
  int Get() const override;

  /**
   * Reset the Encoder distance to zero.
   *
   * Resets the current count to zero on the encoder.
   */
  void Reset() override;

  /**
   * Returns the period of the most recent pulse.
   *
   * Returns the period of the most recent Encoder pulse in seconds. This method
   * compensates for the decoding type.
   *
   * Warning: This returns unscaled periods. Use GetRate() for rates that are
   * scaled using the value from SetDistancePerPulse().
   *
   * @return Period in seconds of the most recent pulse.
   */
  double GetPeriod() const override;

  /**
   * Sets the maximum period for stopped detection.
   *
   * Sets the value that represents the maximum period of the Encoder before it
   * will assume that the attached device is stopped. This timeout allows users
   * to determine if the wheels or other shaft has stopped rotating.
   * This method compensates for the decoding type.
   *
   * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled
   *             periods and SetMinRate() scales using value from
   *             SetDistancePerPulse().
   *
   * @param maxPeriod The maximum time between rising and falling edges before
   *                  the FPGA will report the device stopped. This is expressed
   *                  in seconds.
   */
  void SetMaxPeriod(double maxPeriod) override;

  /**
   * Determine if the encoder is stopped.
   *
   * Using the MaxPeriod value, a boolean is returned that is true if the
   * encoder is considered stopped and false if it is still moving. A stopped
   * encoder is one where the most recent pulse width exceeds the MaxPeriod.
   *
   * @return True if the encoder is considered stopped.
   */
  bool GetStopped() const override;

  /**
   * The last direction the encoder value changed.
   *
   * @return The last direction the encoder value changed.
   */
  bool GetDirection() const override;

  /**
   * Gets the raw value from the encoder.
   *
   * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
   * factor.
   *
   * @return Current raw count from the encoder
   */
  int GetRaw() const;

  /**
   * The encoding scale factor 1x, 2x, or 4x, per the requested encodingType.
   *
   * Used to divide raw edge counts down to spec'd counts.
   */
  int GetEncodingScale() const;

  /**
   * Get the distance the robot has driven since the last reset.
   *
   * @return The distance driven since the last reset as scaled by the value
   *         from SetDistancePerPulse().
   */
  double GetDistance() const;

  /**
   * Get the current rate of the encoder.
   *
   * Units are distance per second as scaled by the value from
   * SetDistancePerPulse().
   *
   * @return The current rate of the encoder.
   */
  double GetRate() const;

  /**
   * Set the minimum rate of the device before the hardware reports it stopped.
   *
   * @param minRate The minimum rate.  The units are in distance per second as
   *                scaled by the value from SetDistancePerPulse().
   */
  void SetMinRate(double minRate);

  /**
   * Set the distance per pulse for this encoder.
   *
   * This sets the multiplier used to determine the distance driven based on the
   * count value from the encoder.
   *
   * Do not include the decoding type in this scale.  The library already
   * compensates for the decoding type.
   *
   * Set this value based on the encoder's rated Pulses per Revolution and
   * factor in gearing reductions following the encoder shaft.
   *
   * This distance can be in any units you like, linear or angular.
   *
   * @param distancePerPulse The scale factor that will be used to convert
   *                         pulses to useful units.
   */
  void SetDistancePerPulse(double distancePerPulse);

  /**
   * Get the distance per pulse for this encoder.
   *
   * @return The scale factor that will be used to convert pulses to useful
   *         units.
   */
  double GetDistancePerPulse() const;

  /**
   * Set the direction sensing for this encoder.
   *
   * This sets the direction sensing on the encoder so that it could count in
   * the correct software direction regardless of the mounting.
   *
   * @param reverseDirection true if the encoder direction should be reversed
   */
  void SetReverseDirection(bool reverseDirection);

  /**
   * Set the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period.
   *
   * Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @param samplesToAverage The number of samples to average from 1 to 127.
   */
  void SetSamplesToAverage(int samplesToAverage);

  /**
   * Get the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period.
   *
   * Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @return The number of samples being averaged (from 1 to 127)
   */
  int GetSamplesToAverage() const;

  double PIDGet() override;

  /**
   * Set the index source for the encoder.
   *
   * When this source is activated, the encoder count automatically resets.
   *
   * @param channel A DIO channel to set as the encoder index
   * @param type    The state that will cause the encoder to reset
   */
  void SetIndexSource(int channel, IndexingType type = kResetOnRisingEdge);

  /**
   * Set the index source for the encoder.
   *
   * When this source is activated, the encoder count automatically resets.
   *
   * @param channel A digital source to set as the encoder index
   * @param type    The state that will cause the encoder to reset
   */
  void SetIndexSource(const DigitalSource& source,
                      IndexingType type = kResetOnRisingEdge);

  /**
   * Indicates this encoder is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetFPGAIndex() const;

  void InitSendable(SendableBuilder& builder) override;

 private:
  /**
   * Common initialization code for Encoders.
   *
   * This code allocates resources for Encoders and is common to all
   * constructors. The counter will start counting immediately.
   *
   * @param reverseDirection If true, counts down instead of up (this is all
   *                         relative)
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  void InitEncoder(bool reverseDirection, EncodingType encodingType);

  /**
   * The scale needed to convert a raw counter value into a number of encoder
   * pulses.
   */
  double DecodingScaleFactor() const;

  std::shared_ptr<DigitalSource> m_aSource;  // The A phase of the quad encoder
  std::shared_ptr<DigitalSource> m_bSource;  // The B phase of the quad encoder
  std::shared_ptr<DigitalSource> m_indexSource = nullptr;
  hal::Handle<HAL_EncoderHandle> m_encoder;

  friend class DigitalGlitchFilter;
};

}  // namespace frc
