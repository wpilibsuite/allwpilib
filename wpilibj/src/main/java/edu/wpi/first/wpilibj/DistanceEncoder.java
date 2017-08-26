package edu.wpi.first.wpilibj;

import tec.uom.se.quantity.Quantities;
import tec.uom.se.unit.Units;

import javax.measure.Quantity;
import javax.measure.Unit;
import javax.measure.quantity.Length;
import javax.measure.quantity.Speed;

public class DistanceEncoder extends QuantityEncoder<Length, Speed> {

  private static final Unit<Length> kValueUnit = Units.METRE;
  private static final Unit<Speed> kSpeedUnit = Units.METRE_PER_SECOND;

  public DistanceEncoder(int channelA, int channelB, double quantityPerPulse, Unit<Length> unit) {
    super(channelA, channelB, Quantities.getQuantity(quantityPerPulse, unit),
        kValueUnit, kSpeedUnit);
  }

  public DistanceEncoder(int channelA, int channelB, Quantity<Length> quantityPerPulse) {
    super(channelA, channelB, quantityPerPulse, kValueUnit, kSpeedUnit);
  }

}
