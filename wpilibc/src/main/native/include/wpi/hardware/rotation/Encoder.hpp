// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Encoder.h"
#include "wpi/hardware/discrete/CounterBase.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {
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
class Encoder : public CounterBase, public wpi::TelemetryLoggable {
 public:
  /**
   * Encoder constructor.
   *
   * Construct a Encoder given a and b channels.
   *
   * The counter will start counting immediately.
   *
   * @param aChannel         The 'a' SmartIO channel.
   * @param bChannel         The 'b' SmartIO channel.
   * @param reverseDirection represents the orientation of the encoder and
   *                         inverts the output values if necessary so forward
   *                         represents positive values.
   * @param encodingType     either X1, X2, or X4 to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  Encoder(int aChannel, int bChannel, bool reverseDirection = false,
          EncodingType encodingType = EncodingType::X4);

  Encoder(Encoder&&) = default;
  Encoder& operator=(Encoder&&) = default;

  ~Encoder() override = default;

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
   * Determine if the encoder is stopped.
   *
   * The encoder is stopped when its current rate is zero.
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
   * The scale factor used to convert raw encoder counts to scaled count values.
   *
   * Used to divide raw encoder counts down to the values returned by Get().
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
   * Indicates this encoder is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetFPGAIndex() const;

  void LogTo(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  /**
   * Common initialization code for Encoders.
   *
   * This code allocates resources for Encoders and is common to all
   * constructors. The counter will start counting immediately.
   * @param aChannel         The a channel.
   * @param bChannel         The b channel.
   * @param reverseDirection If true, counts down instead of up (this is all
   *                         relative)
   * @param encodingType     either X1, X2, or X4 to indicate 1X, 2X or 4X
   *                         decoding. If 4X is selected, then an encoder FPGA
   *                         object is used and the returned counts will be 4x
   *                         the encoder spec'd value since all rising and
   *                         falling edges are counted. If 1X or 2X are selected
   *                         then a counter object will be used and the returned
   *                         value will either exactly match the spec'd count or
   *                         be double (2x) the spec'd count.
   */
  void InitEncoder(int aChannel, int bChannel, bool reverseDirection,
                   EncodingType encodingType);

  /**
   * The scale needed to convert a raw counter value into a number of encoder
   * pulses.
   */
  double DecodingScaleFactor() const;

  wpi::util::Handle<HAL_EncoderHandle, HAL_FreeEncoder> m_encoder;
  EncodingType m_type;
};

}  // namespace wpi
