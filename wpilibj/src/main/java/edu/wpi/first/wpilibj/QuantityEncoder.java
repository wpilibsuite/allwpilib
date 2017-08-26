package edu.wpi.first.wpilibj;

import tec.uom.se.quantity.Quantities;

import javax.measure.Quantity;
import javax.measure.Unit;

import java.util.function.Supplier;

/**
 * Class to read quadrature encoders.
 *
 * @param <Q> The Quantity being measured. (ex: Length, Angle)
 * @param <S> The Speed of the Quantity being measured. (ex: Speed, AngularSpeed)
 */
// Package private so it is not part of the public API
abstract class QuantityEncoder<Q extends Quantity<Q>, S extends Quantity<S>> {

  private final Encoder m_encoder;
  private final Unit<Q> m_valueUnit;
  private final Unit<S> m_speedUnit;

  public QuantityEncoder(int channelA, int channelB, Quantity<Q> quantityPerPulse,
                         Unit<Q> valueUnit, Unit<S> speedUnit) {
    m_encoder = new Encoder(channelA, channelB);
    m_valueUnit = valueUnit;
    m_speedUnit = speedUnit;

    m_encoder.setDistancePerPulse(quantityPerPulse.to(valueUnit).getValue().doubleValue());
  }

  /**
   * Get the speed of the encoder.
   *
   * @return The speed of the encoder.
   */
  public Quantity<S> getSpeed() {
    return Quantities.getQuantity(m_encoder.getRate(), m_speedUnit);
  }

  /**
   * Get the speed of the encoder.
   *
   * @param unit The unit to return as
   * @return The speed of the encoder.
   */
  public double getSpeed(Unit<S> unit) {
    return getSpeed().to(unit).getValue().doubleValue();
  }

  /**
   * A {@code Supplier} that will get the speed of the encoder.
   *
   * @return A Supplier that supplies the speed of the encoder
   */
  public Supplier<Quantity<S>> speed() {
    return this::getSpeed;
  }

  /**
   * Get the value the encoder has driven.
   *
   * @return The value driven since the last reset
   */
  public Quantity<Q> get() {
    return Quantities.getQuantity(m_encoder.getDistance(), m_valueUnit);
  }

  /**
   * Get the distance the encoder has driven.
   *
   * @param unit The unit to return as
   * @return The distance driven since the last reset
   */
  public double get(Unit<Q> unit) {
    return get().to(unit).getValue().doubleValue();
  }

  /**
   * A {@code Supplier} that will get the value of the encoder.
   *
   * @return A Supplier that supplies the value of the encoder
   */
  public Supplier<Quantity<Q>> value() {
    return this::get;
  }

}
