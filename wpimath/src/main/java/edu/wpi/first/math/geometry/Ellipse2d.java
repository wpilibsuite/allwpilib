package edu.wpi.first.math.geometry;

/** 
 * Represents a 2d ellipse space containing translational, rotational, and scaling components 
 */
public class Ellipse2d {
  private final Pose2d m_center;
  private final double m_horizontalSemiAxis, m_verticalSemiAxis;

  /**
   * Constructs an ellipse from two semi-axes, a horizontal and vertical one.
   * 
   * <p>The orientation of these radii may be changed via the rotational component of the center.
   * 
   * @param center The center of the ellipse.
   * @param horizontalSemiAxis The horizontal (x-axis parallel) semi-axis.
   * @param semiMajorAxis The vertical (y-axis parallel) semi-axis.
   */
  public Ellipse2d(Pose2d center, double horizontalSemiAxis, double verticalSemiAxis) {
    // Safety check
    if (horizontalSemiAxis <= 0 || verticalSemiAxis <= 0) {
      throw new IllegalArgumentException("Ellipse2d semi-axes must be positive");
    }

    m_center = center;
    m_horizontalSemiAxis = horizontalSemiAxis;
    m_verticalSemiAxis = verticalSemiAxis;
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
   * Returns the horizontal semi-axis.
   * 
   * @return The horizontal semi-axis.
   */
  public double getHorizontalSemiAxis() {
    return m_horizontalSemiAxis;
  }

  /**
   * Returns the vertical semi-axis.
   * 
   * @return The vertical semi-axis.
   */
  public double getVerticalSemiAxis() {
    return m_verticalSemiAxis;
  }

  /**
   * Returns either of the focal points of the ellipse.
   * In a perfect circle, this will always return the center.
   * 
   * @param first Whether to return the first (-c) or second (+c) focal point.
   * @return A focal point.
   */
  public Translation2d getFocalPoint(boolean first) {
    double a = Math.max(m_horizontalSemiAxis, m_verticalSemiAxis); // Major semi-axis
    double b = Math.min(m_horizontalSemiAxis, m_verticalSemiAxis); // Minor semi-axis
    double c = Math.sqrt(a*a - b*b);

    c = (first ? -c : c);

    if (m_horizontalSemiAxis > m_verticalSemiAxis) {
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
      m_horizontalSemiAxis, 
      m_verticalSemiAxis);
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

    return (x * x) / (m_horizontalSemiAxis * m_horizontalSemiAxis) +
           (y * y) / (m_verticalSemiAxis * m_verticalSemiAxis);
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
   * This is inclusive, if the point lies on the circumference it will return {@code true}.
   * 
   * @param point The point to check.
   * @return True, if the point is within or on the ellipse.
   */
  public boolean containsPoint(Translation2d point) {
    return solveEllipseEquation(point) <= 1.0;
  }

  /**
   * Finds the minimum distance between the ellipse and the point.
   * If the point lies within the ellipse, it returns 0.0.
   * 
   * @param point The point to check.
   * @return The distance.
   */
  public double distanceToPoint(Translation2d point) {
    
  }
}
