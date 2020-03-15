// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * Class to read quadrature encoders.
 *
 * <p>Quadrature encoders are devices that count shaft rotation and can sense direction. The output
 * of the Encoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating Encoders, a direction can be supplied that inverts the
 * sense of the output to make code more readable if the encoder is mounted such that forward
 * movement generates negative values. Quadrature encoders have two digital outputs, an A Channel
 * and a B Channel, that are out of phase with each other for direction sensing.
 *
 * <p>All encoders will immediately start counting - reset() them if you need them to be zeroed
 * before use.
 *
 * @deprecated use {@link QuadratureEncoder}
 */
@Deprecated(forRemoval = true)
public class Encoder extends QuadratureEncoder {
  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param channelB The b channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   */
  public Encoder(final int channelA, final int channelB, boolean reverseDirection) {
    super(channelA, channelB, reverseDirection, EncodingType.k4X);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   */
  public Encoder(final int channelA, final int channelB) {
    super(channelA, channelB, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *     selected, then an encoder FPGA object is used and the returned counts will be 4x the
   *     encoder spec'd value since all rising and falling edges are counted. If 1X or 2X are
   *     selected then a m_counter object will be used and the returned value will either exactly
   *     match the spec'd count or be double (2x) the spec'd count.
   */
  public Encoder(final int channelA, final int channelB, boolean reverseDirection,
                 final EncodingType encodingType) {

    super(channelA, channelB, reverseDirection, encodingType);

  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels. Using an index pulse forces 4x
   * encoding
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   * @param indexChannel The index channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   */
  public Encoder(final int channelA, final int channelB, final int indexChannel,
                 boolean reverseDirection) {
    super(channelA, channelB, indexChannel, reverseDirection);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels. Using an index pulse forces 4x
   * encoding
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   * @param indexChannel The index channel digital input channel.
   */
  public Encoder(final int channelA, final int channelB, final int indexChannel) {
    super(channelA, channelB, indexChannel, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA The source that should be used for the a channel.
   * @param sourceB the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, boolean reverseDirection) {
    super(sourceA, sourceB, reverseDirection, EncodingType.k4X);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA The source that should be used for the a channel.
   * @param sourceB the source that should be used for the b channel.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB) {
    super(sourceA, sourceB, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA The source that should be used for the a channel.
   * @param sourceB the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *     selected, then an encoder FPGA object is used and the returned counts will be 4x the
   *     encoder spec'd value since all rising and falling edges are counted. If 1X or 2X are
   *     selected then a m_counter object will be used and the returned value will either exactly
   *     match the spec'd count or be double (2x) the spec'd count.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, boolean reverseDirection,
                 final EncodingType encodingType) {

    super(sourceA, sourceB, reverseDirection, encodingType);
  }

  /**
   * Encoder constructor. Construct a Encoder given a, b and index channels as digital inputs. This
   * is used in the case where the digital inputs are shared. The Encoder class will not allocate
   * the digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA The source that should be used for the a channel.
   * @param sourceB the source that should be used for the b channel.
   * @param indexSource the source that should be used for the index channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *     if necessary so forward represents positive values.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource,
                 boolean reverseDirection) {
    super(sourceA, sourceB, indexSource, reverseDirection);
  }

  /**
   * Encoder constructor. Construct a Encoder given a, b and index channels as digital inputs. This
   * is used in the case where the digital inputs are shared. The Encoder class will not allocate
   * the digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA     The source that should be used for the a channel.
   * @param sourceB     the source that should be used for the b channel.
   * @param indexSource the source that should be used for the index channel.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource) {
    super(sourceA, sourceB, indexSource, false);
  }
}
