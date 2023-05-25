// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.proto.Geometry2D.ProtobufTwist2d;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;
import java.util.Objects;
import us.hebi.quickbuf.Descriptors.Descriptor;

/**
 * A change in distance along a 2D arc since the last pose update. We can use ideas from
 * differential calculus to create new Pose2d objects from a Twist2d and vice versa.
 *
 * <p>A Twist can be used to represent a difference between two poses.
 */
public class Twist2d {
  /** Linear "dx" component. */
  public double dx;

  /** Linear "dy" component. */
  public double dy;

  /** Angular "dtheta" component (radians). */
  public double dtheta;

  public Twist2d() {}

  /**
   * Constructs a Twist2d with the given values.
   *
   * @param dx Change in x direction relative to robot.
   * @param dy Change in y direction relative to robot.
   * @param dtheta Change in angle relative to robot.
   */
  public Twist2d(double dx, double dy, double dtheta) {
    this.dx = dx;
    this.dy = dy;
    this.dtheta = dtheta;
  }

  @Override
  public String toString() {
    return String.format("Twist2d(dX: %.2f, dY: %.2f, dTheta: %.2f)", dx, dy, dtheta);
  }

  /**
   * Checks equality between this Twist2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Twist2d) {
      return Math.abs(((Twist2d) obj).dx - dx) < 1E-9
          && Math.abs(((Twist2d) obj).dy - dy) < 1E-9
          && Math.abs(((Twist2d) obj).dtheta - dtheta) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(dx, dy, dtheta);
  }

  public static final class AStruct implements Struct<Twist2d> {
    @Override
    public Class<Twist2d> getTypeClass() {
      return Twist2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:Twist2d";
    }

    @Override
    public int getSize() {
      return kSizeDouble * 3;
    }

    @Override
    public String getSchema() {
      return "double dx;double dy;double dtheta";
    }

    @Override
    public Twist2d unpack(ByteBuffer bb) {
      double dx = bb.getDouble();
      double dy = bb.getDouble();
      double dtheta = bb.getDouble();
      return new Twist2d(dx, dy, dtheta);
    }

    @Override
    public void pack(ByteBuffer bb, Twist2d value) {
      bb.putDouble(value.dx);
      bb.putDouble(value.dy);
      bb.putDouble(value.dtheta);
    }
  }

  public static final AStruct struct = new AStruct();

  public static final class AProto implements Protobuf<Twist2d, ProtobufTwist2d> {
    @Override
    public Class<Twist2d> getTypeClass() {
      return Twist2d.class;
    }

    @Override
    public Descriptor getDescriptor() {
      return ProtobufTwist2d.getDescriptor();
    }

    @Override
    public ProtobufTwist2d createMessage() {
      return ProtobufTwist2d.newInstance();
    }

    @Override
    public Twist2d unpack(ProtobufTwist2d msg) {
      return new Twist2d(msg.getDx(), msg.getDy(), msg.getDtheta());
    }

    @Override
    public void pack(ProtobufTwist2d msg, Twist2d value) {
      msg.setDx(value.dx).setDy(value.dy).setDtheta(value.dtheta);
    }
  }

  public static final AProto proto = new AProto();
}
