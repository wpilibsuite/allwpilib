package edu.wpi.first.wpilibj;

import tec.uom.se.quantity.Quantities;

import javax.measure.Quantity;
import javax.measure.Unit;
import java.util.function.Supplier;

/**
 *
 * @param <Q> The Quantity being measured. (ex: Length, Angle)
 * @param <S> The Speed of the Quantity being measured. (ex: Speed, AngularSpeed)
 */
// Package private so it is not part of the public API
abstract class QuantityEncoder<Q extends Quantity<Q>, S extends Quantity<S>> {

  private final Encoder m_encoder;

  public QuantityEncoder(int channelA, int channelB, double distancePerPule, Unit<Q> unit) {
    m_encoder = new Encoder(channelA, channelB);

    m_encoder.setDistancePerPulse(Quantities.getQuantity(distancePerPule, unit).to(unit.getSystemUnit())
        .getValue().doubleValue());
  }

  public QuantityEncoder(int channelA, int channelB, Quantity<Q> quantityPerPulse) {
    m_encoder = new Encoder(channelA, channelB);

    m_encoder.setDistancePerPulse(quantityPerPulse.to(quantityPerPulse.getUnit().getSystemUnit())
        .getValue().doubleValue());
  }

  public QuantityEncoder(int channelA, int channelB, int indexChannel,
                      Quantity<Q> quantityPerPulse) {
    m_encoder = new Encoder(channelA, channelB, indexChannel);

    m_encoder.setDistancePerPulse(quantityPerPulse.to(quantityPerPulse.getUnit().getSystemUnit())
        .getValue().doubleValue());
  }

  public QuantityEncoder(DigitalSource sourceA, DigitalSource sourceB,
                      Quantity<Q> quantityPerPulse) {
    m_encoder = new Encoder(sourceA, sourceB);

    m_encoder.setDistancePerPulse(quantityPerPulse.to(quantityPerPulse.getUnit().getSystemUnit())
        .getValue().doubleValue());
  }

  public QuantityEncoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource,
                      Quantity<Q> quantityPerPulse) {
    m_encoder = new Encoder(sourceA, sourceB, indexSource);

    m_encoder.setDistancePerPulse(quantityPerPulse.to(quantityPerPulse.getUnit().getSystemUnit())
        .getValue().doubleValue());
  }

  /**
   * Set the minimum rate of the device before the hardware reports it stopped.
   *
   * @param minRate The minimum speed.
   */
  public void setMinimumSpeed(Quantity<S> minRate) {
    m_encoder.setMinRate(minRate.to(minRate.getUnit().getSystemUnit()).getValue().doubleValue());
  }

  /**
   * Get the speed of the encoder.
   *
   * @return The speed of the encoder.
   */
  public Quantity<S> getSpeed() {
    return Quantities.getQuantity(m_encoder.getRate(), getSpeedUnit().getSystemUnit());
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
    return Quantities.getQuantity(m_encoder.getDistance(), getUnit().getSystemUnit());
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

  public abstract Unit<Q> getUnit();

  public abstract Unit<S> getSpeedUnit();

}
