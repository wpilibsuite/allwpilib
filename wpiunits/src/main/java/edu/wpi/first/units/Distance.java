// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of linear dimension.
 *
 * <p>This is the base type for units of linear dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Distance&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Meters} and {@link Units#Inches}) can be found in the
 * {@link Units} class.
 */
public class Distance extends Unit<Distance> {
  Distance(Distance baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  Distance(
      Distance baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Creates a linear velocity unit derived from this one. Can be chained to denote velocity,
   * acceleration, jerk, etc.
   *
   * <pre>
   *   Meters.per(Second) // linear velocity
   *   Feet.per(Second).per(Second).of(32) // roughly 1G of acceleration
   * </pre>
   *
   * @param period the time period of the velocity, such as seconds or milliseconds
   * @return a linear velocity unit corresponding to the rate of change of distance over time
   */
  public LinearVelocity per(Time period) {
    return LinearVelocity.combine(this, period);
  }

  /**
   * Creates an energy unit derived from this one.
   *
   * <pre>
   *   Meters.times(Newtons) // force
   *   Centimeters.times(Newtons) // force
   * </pre>
   *
   * @param force the force applied at this distance.
   * @return an energy unit corresponding to force times distance.
   */
  public Energy times(Force force) {
    return Energy.combine(force, this);
  }

  /**
   * Creates an angular momentum unit derived from this one.
   *
   * <pre>
   *   Meters.times(KilogramMetersPerSecond) // angular momentum
   * </pre>
   *
   * @param linearMomentum the linear momentum acting at this position.
   * @return an angular momentum unit corresponding to distance times linear momentum.
   */
  public AngularMomentum times(LinearMomentum linearMomentum) {
    return AngularMomentum.combine(this, linearMomentum);
  }
}
