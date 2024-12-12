package edu.wpi.first.math.geometry;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.struct.Pose2dStruct;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * Represents a vector in a vector field. Has an origin, and x/y component vectors.
 *
 * <p>The name is not semantically correct (Vector2d would mean a vector in 2d space which would
 * only have components without an origin) but naming things is hard.
 *
 * <p>TODO: Change the name to something that is semantically correct
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Vector2d implements StructSerializable {
  private final Translation2d position;
  private final Translation2d components;

  /**
   * Constructs a vector with the specified position and component vectors.
   *
   * @param position The position of the vector.
   * @param components The X/Y component vectors.
   */
  public Vector2d(
      @JsonProperty(required = true, value = "position") Translation2d position,
      @JsonProperty(required = true, value = "components") Translation2d components) {
    this.position = position;
    this.components = components;
  }

  /**
   * Constructs a vector with the specified position, direction, and magnitude.
   *
   * @param position The position of the vector.
   * @param direction The direction of the vector.
   * @param magnitude The magnitude of the vector.
   */
  public Vector2d(Translation2d position, Rotation2d direction, double magnitude) {
    this(position, new Translation2d(magnitude, direction));
  }

  /**
   * Constructs a vector from a start and end point.
   *
   * @param start The start point.
   * @param end The end point.
   * @return A new Vector2d with the correct position and components.
   */
  public static Vector2d fromStartEndPoints(Translation2d start, Translation2d end) {
    return new Vector2d(start, end.minus(start));
  }

  /**
   * Returns the position of the vector.
   *
   * @return The position of the vector.
   */
  @JsonProperty
  public Translation2d getPosition() {
    return position;
  }

  /**
   * Returns the X component of the vector's position
   *
   * @return The Y component of the vector's position
   */
  public double getXPos() {
    return position.getX();
  }

  /**
   * Returns the Y component of the vector's position
   *
   * @return The Y component of the vector's position
   */
  public double getYPos() {
    return position.getY();
  }

  /**
   * Returns the components of the vector.
   *
   * @return The components of the vector.
   */
  @JsonProperty
  public Translation2d getComponents() {
    return components;
  }

  /**
   * Returns the X component of the vector.
   *
   * @return The X component of the vector.
   */
  public double getXComponent() {
    return components.getX();
  }

  /**
   * Returns the Y component of the vector.
   *
   * @return The Y component of the vector.
   */
  public double getYComponent() {
    return components.getY();
  }

  /**
   * Returns the magnitude of the vector.
   *
   * @return The magnitude of the vector.
   */
  public double getMagnitude() {
    return components.getNorm();
  }

  /**
   * Returns the angle of the vector.
   *
   * @return The angle of the vector.
   */
  public Rotation2d getAngle() {
    return components.getAngle();
  }

  /**
   * Multiplies the magnitude of the vector by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Vector2d.
   */
  public Vector2d times(double scalar) {
    return new Vector2d(position, components.times(scalar));
  }

  /**
   * Divides the magnitude of the vector by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Vector2d.
   */
  public Vector2d div(double scalar) {
    return new Vector2d(position, components.div(scalar));
  }

  /**
   * Moves the position of the vector by an offset and returns the new vector.
   *
   * @param offset The offset.
   * @return The new moved Vector2d.
   */
  public Vector2d move(Translation2d offset) {
    return new Vector2d(position.plus(offset), components);
  }

  /**
   * Rotates the vector around its position and returns the new vector.
   *
   * @param rotation The rotation to transform the vector by.
   * @return The new rotated vector.
   */
  public Vector2d rotateBy(Rotation2d rotation) {
    return new Vector2d(position, components.rotateBy(rotation));
  }

  /**
   * Constructs a new Pose2d with the same position and direction as the vector.
   *
   * @return The new Pose2d.
   */
  public Pose2d toPose2d() {
    return new Pose2d(position, getAngle());
  }

  @Override
  public String toString() {
    return String.format(
        "Vector2d(Position: %.2s, Components: %.2s)", position.toString(), components.toString());
  }

  @Override
  public int hashCode() {
    return Objects.hash(position, components);
  }

  // TODO: Add Protobuf serialization

  /** vector2d struct for serialization. */
  public static final Pose2dStruct struct = new Pose2dStruct();
}
