package edu.wpi.first.wpilibj.simulation.swerve;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.wpilibj.drive.Vector2d;

public class Force2d {
  Matrix<N2, N1> m;

  /**
   * Constructs a Force2d with X and Y components equal to zero.
   */
  public Force2d() {
    this(0.0, 0.0);
  }

  /**
   * Constructs a Force2d with the X and Y components equal to the
   * provided values.
   *
   * @param x The x component of the force.
   * @param y The y component of the force.
   */
  public Force2d( double x, double y) {
    m = new Matrix<>(new SimpleMatrix(2, 1));
    m.set(0, 0, x);
    m.set(1, 0, y);
  }

  /**
   * Constructs a Force2d with the provided force magnitude and angle. This is
   * essentially converting from polar coordinates to Cartesian coordinates.
   *
   * @param mag The magnititude of the force 
   * @param angle    The angle from the x-axis to the force vector.
   */
  public Force2d(double mag, Rotation2d angle) {
    this(mag * angle.getCos(), mag * angle.getSin());
  }

  /**
   * Constructs a Force2d with the provided 2-element column matrix as the x/y components
   *
   * @param m_in 2 row, 1 column input matrix
   */
  public Force2d(Matrix<N2, N1> m_in) {
    m = m_in;
  }
  
  /**
   * Constructs a Force2d with the provided vector assumed to represent the force
   *
   * @param force_vec 
   */
  public Force2d(Vector2d force_vec) {
    this(force_vec.x, force_vec.y);
  }

  /**
   * Returns the X component of the force.
   *
   * @return The x component of the force.
   */

  public double getX() {
    return m.get(0, 0);
  }

  /**
   * Returns the Y component of the force.
   *
   * @return The y component of the force.
   */

  public double getY() {
    return m.get(1, 0);
  }

  /**
   * Returns the norm, or distance from the origin to the force.
   *
   * @return The norm of the force.
   */
  public double getNorm() {
    return m.normF();
  }

  /**
   * 
   * @return a unit vector in the directino this force points
   */
  public Vector2d getUnitVector() {
    return new Vector2d(this.getX()/this.getNorm(), this.getY()/this.getNorm());
  }

  /**
   * Applies a rotation to the force in 2d space.
   *
   * <p>This multiplies the force vector by a counterclockwise rotation
   * matrix of the given angle.
   * [x_new]   [other.cos, -other.sin][x]
   * [y_new] = [other.sin,  other.cos][y]
   *
   * <p>For example, rotating a Force2d of {2, 0} by 90 degrees will return a
   * Force2d of {0, 2}.
   *
   * @param angle The rotation to rotate the force by.
   * @return The new rotated force.
   */
  public Force2d rotateBy(Rotation2d angle) {
    return new Force2d(
            this.getX() * angle.getCos() - this.getY() * angle.getSin(),
            this.getX() * angle.getSin() + this.getY() * angle.getCos()
    );
  }

  /**
   * Adds two forces in 2d space and returns the sum. This is similar to
   * vector addition.
   *
   * <p>For example, Force2d{1.0, 2.5} + Force2d{2.0, 5.5} =
   * Force2d{3.0, 8.0}
   *
   * @param other The force to add.
   * @return The sum of the forces.
   */
  public Force2d plus(Force2d other) {
    return new Force2d(this.m.plus(other.m));
  }

  /**
   * Accumulates another force into this force
   *
   *
   * @param other The force to add.
   * @return nothing (acts on this force in-place)
   */
  public void accum(Force2d other) {
    this.m = this.m.plus(other.m);
  }

  /**
   * Subtracts the other force from the other force and returns the
   * difference.
   *
   * <p>For example, Force2d{5.0, 4.0} - Force2d{1.0, 2.0} =
   * Force2d{4.0, 2.0}
   *
   * @param other The force to subtract.
   * @return The difference between the two forces.
   */
  public Force2d minus(Force2d other) {
    return new Force2d(this.m.minus(other.m));
  }

  /**
   * Returns the inverse of the current force. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or simply
   * negating both components of the force.
   *
   * @return The inverse of the current force.
   */
  public Force2d unaryMinus() {
    return new Force2d(this.m.times(-1.0));
  }

  /**
   * Multiplies the force by a scalar and returns the new force.
   *
   * <p>For example, Force2d{2.0, 2.5} * 2 = Force2d{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled force.
   */
  public Force2d times(double scalar) {
    return new Force2d(this.m.times(scalar));
  }

  /**
   * Divides the force by a scalar and returns the new force.
   *
   * <p>For example, Force2d{2.0, 2.5} / 2 = Force2d{1.0, 1.25}
   *
   * @param scalar The scalar to multiply by.
   * @return The reference to the new mutated object.
   */
  public Force2d div(double scalar) {
    return new Force2d(this.m.div(scalar));
  }

  /**
   * Creates a Vector2d object from the force this object represents
   */
  public Vector2d getVector2d(){
    return new Vector2d(this.getX(), this.getY());
  }

  @Override
  public String toString() {
    return String.format("Force2d(X: %.2f, Y: %.2f)", this.getX(), this.getY());
  }

  /**
   * Checks equality between this Force2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Force2d) {
      return this.m.isEqual(((Force2d)obj).m, 1E-9);
    } else {
      return false;
    }
  }
}
