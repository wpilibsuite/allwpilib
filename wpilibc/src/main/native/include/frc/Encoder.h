// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <wpi/deprecated.h>
#include "QuadratureEncoder.h"

namespace frc {

/**
 * Class to read quad encoders.
 * Deprecated : use QuadratureEncoder instead.
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

class Encoder : public QuadratureEncoder {
  public:

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
  WPI_DEPRECATED("use QuadratureEncoder instead")
  Encoder(int aChannel, int bChannel, bool reverseDirection = false, 
        EncodingType encodingType = EncodingType::k4X);

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
  WPI_DEPRECATED("use QuadratureEncoder instead")
  Encoder(DigitalSource* aSource, DigitalSource* bSource,
          bool reverseDirection = false, EncodingType encodingType = EncodingType::k4X);

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
  WPI_DEPRECATED("use QuadratureEncoder instead")
  Encoder(DigitalSource& aSource, DigitalSource& bSource, bool reverseDirection = false,
          EncodingType encodingType = EncodingType::k4X);

  Encoder(std::shared_ptr<DigitalSource> aSource, std::shared_ptr<DigitalSource> bSource,
          bool reverseDirection = false, EncodingType encodingType = EncodingType::k4X);


  

} ;}
