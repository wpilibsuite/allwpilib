package edu.wpi.first.wpilibj;

import si.uom.SI;
import si.uom.quantity.AngularSpeed;

import javax.measure.Quantity;
import javax.measure.Unit;
import javax.measure.quantity.Angle;

public class AngleEncoder extends QuantityEncoder<Angle, AngularSpeed> {

  public AngleEncoder(int channelA, int channelB, Quantity<Angle> quantityPerPulse) {
    super(channelA, channelB, quantityPerPulse);
  }

  public AngleEncoder(int channelA, int channelB, int indexChannel, Quantity<Angle>
      quantityPerPulse) {
    super(channelA, channelB, indexChannel, quantityPerPulse);
  }

  public AngleEncoder(DigitalSource sourceA, DigitalSource sourceB, Quantity<Angle>
      quantityPerPulse) {
    super(sourceA, sourceB, quantityPerPulse);
  }

  public AngleEncoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource,
                      Quantity<Angle> quantityPerPulse) {
    super(sourceA, sourceB, indexSource, quantityPerPulse);
  }

  @Override
  public Unit<Angle> getUnit() {
    return SI.RADIAN;
  }

  @Override
  public Unit<AngularSpeed> getSpeedUnit() {
    return SI.RADIAN.divide(SI.SECOND).asType(AngularSpeed.class);
  }
}
