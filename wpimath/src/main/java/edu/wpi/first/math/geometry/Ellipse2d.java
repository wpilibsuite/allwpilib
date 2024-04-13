package edu.wpi.first.math.geometry;

import java.util.Objects;

import edu.wpi.first.math.geometry.proto.Ellipse2dProto;
import edu.wpi.first.math.geometry.struct.Ellipse2dStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** 
 * Represents a 2d ellipse space containing translational, rotational, and scaling components 
 */
public class Ellipse2d implements ProtobufSerializable, StructSerializable {
  private final Pose2d m_center;
  private final double m_xSemiAxis, m_ySemiAxis;

  /**
   * Constructs an ellipse around a center point and two semi-axes, a horizontal and vertical one.
   * 
   * @param center The center of the ellipse.
   * @param xSemiAxis The x semi-axis.
   * @param ySemiAxis The y semi-axis.
   */
  public Ellipse2d(Pose2d center, double xSemiAxis, double ySemiAxis) {
    // Safety check
    if (xSemiAxis <= 0 || ySemiAxis <= 0) {
      throw new IllegalArgumentException("Ellipse2d semi-axes must be positive");
    }

    m_center = center;
    m_xSemiAxis = xSemiAxis;
    m_ySemiAxis = ySemiAxis;
  }

  /**
   * Constructs a perfectly circular ellipse with the specified radius.
   * 
   * @param center The center of the circle.
   * @param radius The radius of the circle.
   */
  public Ellipse2d(Pose2d center, double radius) {
    this(center, radius, radius);
  }

  /**
   * Returns the center of the ellipse.
   * 
   * @return The center of the ellipse.
   */
  public Pose2d getCenter() {
    return m_center;
  }

  /**
   * Returns the x semi-axis.
   * 
   * @return The x semi-axis.
   */
  public double getXSemiAxis() {
    return m_xSemiAxis;
  }

  /**
   * Returns the y semi-axis.
   * 
   * @return The y semi-axis.
   */
  public double getYSemiAxis() {
    return m_ySemiAxis;
  }

  /**
   * Returns either of the focal points of the ellipse.
   * In a perfect circle, this will always return the center.
   * 
   * @param first Whether to return the first (-c) or second (+c) focal point.
   * @return A focal point.
   */
  public Translation2d getFocalPoint(boolean first) {
    double a = Math.max(m_xSemiAxis, m_ySemiAxis); // Major semi-axis
    double b = Math.min(m_xSemiAxis, m_ySemiAxis); // Minor semi-axis
    double c = Math.hypot(a, b);

    c = (first ? -c : c);

    if (m_xSemiAxis > m_ySemiAxis) {
      Transform2d diff = new Transform2d(c, 0.0, new Rotation2d());
      return m_center.plus(diff).getTranslation();
    } else {
      Transform2d diff = new Transform2d(0.0, c, new Rotation2d());
      return m_center.plus(diff).getTranslation();
    }
  }

  /**
   * Transforms the center of the ellipse and returns the new ellipse.
   * 
   * @param other The transform to transform by.
   * @return The transformed ellipse.
   */
  public Ellipse2d transformBy(Transform2d other) {
    return new Ellipse2d(
      m_center.transformBy(other), 
      m_xSemiAxis, 
      m_ySemiAxis);
  }

  /**
   * Rotates the center of the ellipse and returns the new ellipse.
   * 
   * @param other The rotation to transform by.
   * @return The rotated ellipse.
   */
  public Ellipse2d rotateBy(Rotation2d other) {
    return new Ellipse2d(m_center.rotateBy(other), m_xSemiAxis, m_ySemiAxis);
  }

  /**
   * Solves the equation of an ellipse from the given point. This is a helper function used to
   * determine if that point lies inside of or on an ellipse.
   * 
   * <pre>
   * (x-h)^2 / a^2 + (y-k)^2 / b^2 = 1
   * </pre>
   * @param point The point to solve for.
   * @return < 1.0 if the point lies inside the ellipse, == 1.0 if a point lies on the ellipse,
   *  and > 1.0 if the point lies outsides the ellipse.
   */
  private double solveEllipseEquation(Translation2d point) {
    // Rotate the point by the inverse of the ellipse's rotation
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());

    double x = point.getX() - m_center.getX();
    double y = point.getY() - m_center.getY();

    return (x * x) / (m_xSemiAxis * m_ySemiAxis) +
           (y * y) / (m_xSemiAxis * m_ySemiAxis);
  }

  /**
   * Checks if a point is intersected by this ellipse's circumference.
   * 
   * @param point The point to check.
   * @return True, if this ellipse's circumference intersects the point.
   */
  public boolean intersectsPoint(Translation2d point) {
    return solveEllipseEquation(point) == 1.0;
  }

  /**
   * Checks if a point is contained within this ellipse.
   * This is inclusive, if the point lies on the circumference this will return {@code true}.
   * 
   * @param point The point to check.
   * @return True, if the point is within or on the ellipse.
   */
  public boolean containsPoint(Translation2d point) {
    return solveEllipseEquation(point) <= 1.0;
  }

  @Override
  public String toString() {
    return String.format("Ellipse2d(center: %s, x: %.2f, y:%.2f)",
      m_center, m_xSemiAxis, m_ySemiAxis);
  }

  /**
   * Checks equality between this Ellipse2d and another object
   * 
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Ellipse2d) {
      return ((Ellipse2d) obj).getCenter().equals(m_center)
          && ((Ellipse2d) obj).getXSemiAxis() == m_xSemiAxis
          && ((Ellipse2d) obj).getYSemiAxis() == m_ySemiAxis;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_center, m_xSemiAxis, m_ySemiAxis);
  }

  /** Ellipse2d protobuf for serialization. */
  public static final Ellipse2dProto proto = new Ellipse2dProto();

  /** Ellipse2d struct for serialization. */
  public static final Ellipse2dStruct struct = new Ellipse2dStruct();
}
