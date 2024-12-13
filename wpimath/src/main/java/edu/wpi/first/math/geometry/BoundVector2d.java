package edu.wpi.first.math.geometry;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.struct.BoundVector2dStruct;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * Represents a bound vector, or a vector with a start and end point, instead of just a direction
 * and magnitude. Instead of having a defined end point, this object has a start point, and a vector
 * relative to that start point.
 *
 * <p>This object can be used to represent a force being applied from a certain location, or can be
 * used to represent a certain vector in a vector field visualization.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class BoundVector2d implements StructSerializable {
  private final Translation2d m_startPoint;
  private final Translation2d m_vector;

  /**
   * Constructs a vector with the specified start point.
   *
   * @param startPoint The position of the vector.
   * @param vector The vector originating from the start point.
   */
  public BoundVector2d(
      @JsonProperty(required = true, value = "startPoint") Translation2d startPoint,
      @JsonProperty(required = true, value = "vector") Translation2d vector) {
    this.m_startPoint = startPoint;
    this.m_vector = vector;
  }

  /**
   * Constructs a bound vector with the specified position, direction, and magnitude.
   *
   * @param position The start position of the vector.
   * @param direction The direction of the vector.
   * @param magnitude The magnitude of the vector.
   */
  public BoundVector2d(Translation2d position, Rotation2d direction, double magnitude) {
    this(position, new Translation2d(magnitude, direction));
  }

  /**
   * Constructs a vector from a start and end point.
   *
   * @param start The start point.
   * @param end The end point.
   * @return A new Vector2d with the correct position and components.
   */
  public static BoundVector2d fromStartEndPoints(Translation2d start, Translation2d end) {
    return new BoundVector2d(start, end.minus(start));
  }

  /**
   * Returns the start point of the vector.
   *
   * @return The start point of the vector.
   */
  @JsonProperty
  public Translation2d getStartPoint() {
    return m_startPoint;
  }

  /**
   * Returns the X component of the vector's start point.
   *
   * @return The X component of the vector's start point.
   */
  public double getStartX() {
    return m_startPoint.getX();
  }

  /**
   * Returns the Y component of the vector's start point.
   *
   * @return The Y component of the vector's start point.
   */
  public double getStartY() {
    return m_startPoint.getY();
  }

  /**
   * Returns the vector that represents the offset between the start and end points.
   *
   * @return The vector that represents the offset between the start and end points.
   */
  @JsonProperty
  public Translation2d getVector() {
    return m_vector;
  }

  /**
   * Returns the X component of the vector.
   *
   * @return The X component of the vector.
   */
  public double getVectorX() {
    return m_vector.getX();
  }

  /**
   * Returns the Y component of the vector.
   *
   * @return The Y component of the vector.
   */
  public double getVectorY() {
    return m_vector.getY();
  }

  /**
   * Returns the magnitude of the vector.
   *
   * @return The magnitude of the vector.
   */
  public double getMagnitude() {
    return m_vector.getNorm();
  }

  /**
   * Returns the angle the vector forms with the positive X axis.
   *
   * @return The angle the vector forms with the positive X axis.
   */
  public Rotation2d getAngle() {
    return m_vector.getAngle();
  }

  /**
   * Gets the end point of the bounded vector.
   *
   * @return The end point of the bounded vector.
   */
  public Translation2d getEndPoint() {
    return m_startPoint.plus(m_vector);
  }

  /**
   * Multiplies the magnitude of the vector by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled BoundVector2d.
   */
  public BoundVector2d times(double scalar) {
    return new BoundVector2d(m_startPoint, m_vector.times(scalar));
  }

  /**
   * Divides the magnitude of the vector by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled BoundVector2d.
   */
  public BoundVector2d div(double scalar) {
    return new BoundVector2d(m_startPoint, m_vector.div(scalar));
  }

  /**
   * Moves the start position of the vector by an offset and returns the new vector.
   *
   * @param offset The offset.
   * @return The new moved Vector2d.
   */
  public BoundVector2d move(Translation2d offset) {
    return new BoundVector2d(m_startPoint.plus(offset), m_vector);
  }

  /**
   * Rotates the vector around its start position and returns the new vector.
   *
   * @param rotation The rotation to transform the vector by.
   * @return The new rotated vector.
   */
  public BoundVector2d rotateBy(Rotation2d rotation) {
    return new BoundVector2d(m_startPoint, m_vector.rotateBy(rotation));
  }

  /**
   * Constructs a new Pose2d with the same position and direction as the vector.
   *
   * @return The new Pose2d.
   */
  public Pose2d toPose2d() {
    return new Pose2d(m_startPoint, getAngle());
  }

  @Override
  public String toString() {
    return String.format(
        "BoundVector2d(Start point: %.2s, Vector: %.2s)",
        m_startPoint.toString(), m_vector.toString());
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_startPoint, m_vector);
  }

  // TODO: Add Protobuf serialization

  /** vector2d struct for serialization. */
  public static final BoundVector2dStruct struct = new BoundVector2dStruct();
}
