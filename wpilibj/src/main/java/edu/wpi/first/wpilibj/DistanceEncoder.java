package edu.wpi.first.wpilibj;

import si.uom.SI;

import javax.measure.Quantity;
import javax.measure.Unit;
import javax.measure.quantity.Length;
import javax.measure.quantity.Speed;

public class DistanceEncoder extends QuantityEncoder<Length, Speed> {

  public DistanceEncoder(int channelA, int channelB, double distancePerPule, Unit<Length> unit) {
    super(channelA, channelB, distancePerPule, unit);
  }

  public DistanceEncoder(int channelA, int channelB, Quantity<Length> quantityPerPulse) {
    super(channelA, channelB, quantityPerPulse);
  }

  public DistanceEncoder(int channelA, int channelB, int indexChannel,
                         Quantity<Length> quantityPerPulse) {
    super(channelA, channelB, indexChannel, quantityPerPulse);
  }

  public DistanceEncoder(DigitalSource sourceA, DigitalSource sourceB,
                         Quantity<Length> quantityPerPulse) {
    super(sourceA, sourceB, quantityPerPulse);
  }

  public DistanceEncoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource,
                         Quantity<Length> quantityPerPulse) {
    super(sourceA, sourceB, indexSource, quantityPerPulse);
  }

  @Override
  public Unit<Length> getUnit() {
    return SI.METRE;
  }

  @Override
  public Unit<Speed> getSpeedUnit() {
    return SI.METRE_PER_SECOND;
  }
}
