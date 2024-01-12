package edu.wpi.first.math.trajectory;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.Velocity;
import java.util.Objects;

/** Profile state. */
public class ProfileState {
  /** The position at this state. */
  public double position;

  /** The velocity at this state. */
  public double velocity;

  /** Default constructor. */
  public ProfileState() {}

  /**
   * Construct a state within an exponential profile.
   *
   * @param position The position at this state.
   * @param velocity The velocity at this state.
   */
  public ProfileState(double position, double velocity) {
    this.position = position;
    this.velocity = velocity;
  }

  /**
   * Constructs constraints for a Trapezoid Profile.
   *
   * @param <U> Unit type.
   * @param position The position at this state.
   * @param velocity The velocity at this state.
   */
  public <U extends Unit<U>> ProfileState(Measure<U> position, Measure<Velocity<U>> velocity) {
    this(position.baseUnitMagnitude(), velocity.baseUnitMagnitude());
  }

  @Override
  public boolean equals(Object other) {
    if (other instanceof ProfileState) {
      ProfileState rhs = (ProfileState) other;
      return this.position == rhs.position && this.velocity == rhs.velocity;
    } else {
      return false;
    }
  }

  @Override
  public int hashCode() {
    return Objects.hash(position, velocity);
  }

  @Override
  public String toString() {
      return "Position: " + position + ", Velocity: " + velocity;
  }
}
