package edu.wpi.first.wpilibj;

import si.uom.quantity.AngularSpeed;
import tec.uom.se.quantity.Quantities;
import tec.uom.se.unit.Units;

import javax.measure.Quantity;
import javax.measure.Unit;
import javax.measure.quantity.Angle;

public class AngleEncoder extends QuantityEncoder<Angle, AngularSpeed> {

  private static final Unit<Angle> kValueUnit = Units.RADIAN;
  private static final Unit<AngularSpeed> kSpeedUnit
      = Units.RADIAN.divide(Units.SECOND).asType(AngularSpeed.class);

  public AngleEncoder(int channelA, int channelB, double quantityPerPulse, Unit<Angle> unit) {
    super(channelA, channelB, Quantities.getQuantity(quantityPerPulse, unit),
        kValueUnit, kSpeedUnit);
  }

  public AngleEncoder(int channelA, int channelB, Quantity<Angle> quantityPerPulse) {
    super(channelA, channelB, quantityPerPulse, kValueUnit, kSpeedUnit);
  }
}
