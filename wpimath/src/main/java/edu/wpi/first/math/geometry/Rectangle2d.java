// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import java.util.Objects;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.proto.Rectangle2dProto;
import edu.wpi.first.math.geometry.struct.Rectangle2dStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** 
 * Represents a 2d rectangular space containing translational, rotational, and scaling components 
 */
public class Rectangle2d implements ProtobufSerializable, StructSerializable {
  private final Pose2d m_center;
  private final double m_xWidth, m_yWidth;

  /**
   * Constructs a rectangle at the specified position with the specified width and height.
   * 
   * @param center The position (translation and rotation) of the rectangle.
   * @param xWidth The x size component of the rectangle, in unrotated coordinate frame.
   * @param yWidth The y size component of the rectangle, in unrotated coordinate frame.
   */
  public Rectangle2d(Pose2d center, double xWidth, double yWidth) {
    // Safety check size
    if (xWidth < 0 || yWidth < 0) {
      throw new IllegalArgumentException("Rectangle2d dimensions cannot be less than 0!");
    }

    m_center = center;
    m_xWidth = xWidth;
    m_yWidth = yWidth;
  }

  /**
   * Constructs a rectangle at the specified position and rotation with the specified width and
   * height.
   * 
   * @param center The center of the rectangle.
   * @param width The x size component of the rectangle, in unrotated coordinate frame.
   * @param height The y size component of the rectangle, in unrotated coordinate frame.
   * @param rotation The rotation of the rectangle.
   */
  public Rectangle2d(Translation2d center, double xWidth, double yWidth, Rotation2d rotation) {
    this(new Pose2d(center, rotation), xWidth, yWidth);
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
   * Returns the x size component of the rectangle.
   * 
   * @return The x size component of the rectangle.
   */
  public double getXWidth() {
    return m_xWidth;
  }

  /**
   * Returns the y size component of the rectangle.
   * 
   * @return The y size component of the rectangle.
   */
  public double getYWidth() {
    return m_yWidth;
  }

  /**
   * Transforms the center of the rectangle and returns the new rectangle.
   * 
   * @param other The transform to transform by.
   * @return The transformed rectangle
   */
  public Rectangle2d transformBy(Transform2d other) {
    return new Rectangle2d(m_center.transformBy(other), m_xWidth, m_yWidth);
  }

  /**
   * Rotates the center of the rectangle and returns the new rectangle.
   * 
   * @param other The rotation to transform by.
   * @return The rotated rectangle.
   */
  public Rectangle2d rotateBy(Rotation2d other) {
    return new Rectangle2d(m_center.rotateBy(other), m_xWidth, m_yWidth);
  }

  /**
   * Checks if a point is intersected by the rectangle's perimeter.
   * 
   * @param point The point to check.
   * @return True, if the rectangle's perimeter intersects the point.
   */
  public boolean intersectsPoint(Translation2d point) {
    // Move the point into the rectangle's coordinate frame
    point = point.minus(m_center.getTranslation());
    point = point.rotateBy(m_center.getRotation().unaryMinus());

    if (Math.abs(point.getX()) == m_xWidth / 2.0) {
      // Point rests on left/right perimeter
      return (Math.abs(point.getY()) <= m_yWidth / 2.0);
    } else if (Math.abs(point.getY()) == m_yWidth / 2.0) {
      // Point rests on top/bottom perimeter
      return (Math.abs(point.getX()) <= m_xWidth / 2.0);
    }

    return false;
  }

  /**
   * Checks if a point is contained within the rectangle.
   * This is inclusive, if the point lies on the perimeter it will return true.
   * 
   * @param point The point to check.
   * @return True, if the rectangle contains the point or the perimeter intersects the point.
   */
  public boolean containsPoint(Translation2d point) {
    // Rotate the point into the rectangle's coordinate frame
    point = point.rotateAround(m_center.getTranslation(), m_center.getRotation().unaryMinus());
    
    // Check if within bounding box
    return (
      point.getX() >= (m_center.getX() - m_xWidth/2.0) && 
      point.getX() <= (m_center.getX() + m_xWidth/2.0) &&
      point.getY() >= (m_center.getY() - m_yWidth/2.0) &&
      point.getY() <= (m_center.getY() + m_yWidth/2.0));
  }

  /**
   * Returns the distance between the perimeter of the rectangle and the point.
   * 
   * @param point The point to check.
   * @return The distance (0, if the point is contained by the rectangle)
   */
  public double distanceToPoint(Translation2d point) {
    if (containsPoint(point)) return 0.0;

    // Move the point into the rectangle's coordinate frame
    point = point.minus(m_center.getTranslation());
    point = point.rotateBy(m_center.getRotation().unaryMinus());

    // Find x and y distances
    double dx = Math.max(0.0, Math.abs(point.getX()) - m_xWidth/2.0);
    double dy = Math.max(0.0, Math.abs(point.getY()) - m_yWidth/2.0);

    // Distance formula
    return Math.hypot(dx, dy);
  }

  /**
   * Returns the nearest point that is contained within the rectangle.
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
      MathUtil.clamp(point.getX(), m_center.getX() - m_xWidth/2.0, m_center.getX() + m_xWidth/2.0),
      MathUtil.clamp(point.getY(), m_center.getY() - m_yWidth/2.0, m_center.getY() + m_yWidth/2.0)
    );

    // Undo rotation
    return point.rotateAround(m_center.getTranslation(), m_center.getRotation());
  }

  @Override
  public String toString() {
    return String.format("Rectangle2d(center: %s, x: %.2f, y: %.2f)",
      m_center, m_xWidth, m_yWidth);
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
          && ((Rectangle2d) obj).getXWidth() == m_xWidth
          && ((Rectangle2d) obj).getYWidth() == m_yWidth;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_center, m_xWidth, m_yWidth);
  }

  /** Rectangle2d protobuf for serialization. */
  public static final Rectangle2dProto proto = new Rectangle2dProto();

  /** Rectangle2d struct for serialization. */
  public static final Rectangle2dStruct struct = new Rectangle2dStruct();
}
