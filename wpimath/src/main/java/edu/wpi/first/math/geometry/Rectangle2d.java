// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import java.util.Objects;

import edu.wpi.first.math.geometry.proto.Rectangle2dProto;
import edu.wpi.first.math.geometry.struct.Rectangle2dStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** 
 * Represents a 2d rectangular space containing translational, rotational, and scaling components 
 */
public class Rectangle2d implements ProtobufSerializable, StructSerializable {
  private final Pose2d m_center;
  private final double m_width, m_height;

  /**
   * Constructs a rectangle at the specified position with the specified width and height.
   * 
   * @param center The position (translation and rotation) of the rectangle.
   * @param width The width (x size component) of the rectangle.
   * @param height The height (y size component) of the rectangle.
   */
  public Rectangle2d(Pose2d center, double width, double height) {
    // Safety check size
    if (width < 0 || height < 0) {
      throw new IllegalArgumentException("Rectangle2d dimensions cannot be less than 0!");
    }

    m_center = center;
    m_width = width;
    m_height = height;
  }

  /**
   * Constructs a rectangle at the specified position and rotation with the specified width and
   * height.
   * 
   * @param center The center of the rectangle.
   * @param width The width (x size component) of the rectangle, in unrotated coordinate frame.
   * @param height The height (y size component) of the rectangle, in unrotated coordinate frame.
   * @param rotation The rotation of the rectangle.
   */
  public Rectangle2d(Translation2d center, double width, double height, Rotation2d rotation) {
    this(new Pose2d(center, rotation), width, height);
  }

  /**
   * Creates an unrotated rectangle from the given corners.
   * The corners should be diagonally opposite of each other.
   * 
   * @param cornerA The first corner of the rectangle.
   * @param cornerB The second corner of the rectangle.
   */
  public Rectangle2d(Translation2d cornerA, Translation2d cornerB) {
    this(
      cornerA.plus(cornerB).div(2.0),
      Math.abs(cornerA.getX() - cornerB.getX()),
      Math.abs(cornerA.getY() - cornerB.getY()),
      new Rotation2d()
    );
  }

  /**
   * Returns the center of the rectangle.
   * 
   * @return The center of the rectangle.
   */
  public Pose2d getCenter() {
    return m_center;
  }

  /**
   * Returns the rotational component of the rectangle.
   * 
   * @return The rotational component of the rectangle.
   */
  public Rotation2d getRotation() {
    return m_center.getRotation();
  }

  /**
   * Returns the width (x size component) of the rectangle.
   * 
   * @return The width (x size component) of the rectangle.
   */
  public double getWidth() {
    return m_width;
  }

  /**
   * Returns the height (y size component) of the rectangle.
   * 
   * @return The height (y size component) of the rectangle.
   */
  public double getHeight() {
    return m_height;
  }

  /**
   * Transforms the center of the rectangle and returns the new rectangle.
   * 
   * @param other The transform to transform by.
   * @return The transformed rectangle
   */
  public Rectangle2d transformBy(Transform2d other) {
    return new Rectangle2d(m_center.transformBy(other), m_width, m_height);
  }

  /**
   * Checks if a point is intersected by this rectangle's perimeter.
   * 
   * @param point The point to check.
   * @return True, if this rectangle's perimeter intersects the point.
   */
  public boolean intersectsPoint(Translation2d point) {
    // Rotate the point by the inverse of the rectangle's rotation
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());

    // Half of width and height
    double w = m_width/2.0;
    double h = m_height/2.0;

    if (point.getX() == (m_center.getX() - w) || point.getX() == (m_center.getX() + w)) {
      // Rests on left/right perimeter
      return (point.getY() >= (m_center.getY() - h) && point.getY() <= (m_center.getY() - h));
    } else if (point.getY() == (m_center.getY() - h) || point.getY() == (m_center.getY() + h)) {
      // Rest on top/bottom perimeter
      return (point.getX() >= (m_center.getX() - w) && point.getX() <= (m_center.getX() + h));
    }

    return false;
  }

  /**
   * Checks if a point is contained within this rectangle.
   * This is inclusive, if the point lies on the perimeter it will return {@code true}.
   * 
   * @param point The point to check.
   * @return True, if this rectangle contains the point or the perimeter intersects the point.
   */
  public boolean containsPoint(Translation2d point) {
    // Rotate the point by the inverse of the rectangle's rotation
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());
    
    // Check if within bounding box
    return (
      point.getX() >= (m_center.getX() - m_width/2.0) && 
      point.getX() <= (m_center.getX() + m_width/2.0) &&
      point.getY() >= (m_center.getY() - m_height/2.0) &&
      point.getY() <= (m_center.getY() + m_height/2.0)
    );
  }

  /**
   * Returns the distance between the perimeter of the rectangle and the point.
   * 
   * @param point The point to check.
   * @return The distance (0, if the point is on the perimeter or contained by the rectangle)
   */
  public double distanceToPoint(Translation2d point) {
    if (containsPoint(point)) return 0.0;

    // Rotate the point by the inverse of the rectangle's rotation
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());

    // Find x and y distances
    double dx = 
      Math.max(m_center.getX() - (m_width/2.0) - point.getX(), 
      Math.max(0.0, point.getX() - m_center.getX() - (m_width/2.0)));

    double dy = 
      Math.max(m_center.getY() - (m_height/2.0) - point.getY(), 
      Math.max(0.0, point.getY() - m_center.getY() - (m_height/2.0)));

    // Distance formula
    return Math.sqrt(dx*dx + dy*dy);
  }

  /**
   * Returns the nearest point that is contained within this rectangle.
   * 
   * @param point The point that this will find the nearest point to.
   * @return A new point that is nearest to {@code point} and contained in the rectangle.
   */
  public Translation2d findNearestPoint(Translation2d point) {
    // Check if already in rectangle
    if (containsPoint(point)) return new Translation2d(point.getX(), point.getY());

    // Rotate the point by the inverse of the rectangle's rotation
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());
  
    // Find nearest point
    point = new Translation2d(
      Math.max(m_center.getX()-m_width/2.0, Math.min(point.getX(), m_center.getX()+m_width/2.0)),
      Math.max(m_center.getY()-m_height/2.0, Math.min(point.getY(), m_center.getY()+m_height/2.0))
    );

    // Undo rotation
    return point.rotateAround(m_center.getTranslation(), m_center.getRotation());
  }

  @Override
  public String toString() {
    return String.format("Rectangle2d(Center: %s, W: %.2f, H: %.2f)",
      m_center.toString(),
      m_width,
      m_height
    );
  }

  /**
   * Checks equality between this Rectangle2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Rectangle2d) {
      return ((Rectangle2d) obj).getCenter().equals(m_center)
          && ((Rectangle2d) obj).getWidth() == m_width
          && ((Rectangle2d) obj).getHeight() == m_height;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_center, m_width, m_height);
  }

  /** Rectangle2d protobuf for serialization. */
  public static final Rectangle2dProto proto = new Rectangle2dProto();

  /** Rectangle2d struct for serialization. */
  public static final Rectangle2dStruct struct = new Rectangle2dStruct();
}
