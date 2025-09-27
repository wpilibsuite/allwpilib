// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.geometry.proto.Twist3dProto;
import edu.wpi.first.math.geometry.struct.Twist3dStruct;
import edu.wpi.first.math.jni.Twist3dJNI;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * A change in distance along a 3D arc since the last pose update. We can use ideas from
 * differential calculus to create new Pose3d objects from a Twist3d and vice versa.
 *
 * <p>A Twist can be used to represent a difference between two poses.
 */
public class Twist3d implements ProtobufSerializable, StructSerializable {
  /** Linear "dx" component. */
  public double dx;

  /** Linear "dy" component. */
  public double dy;

  /** Linear "dz" component. */
  public double dz;

  /** Rotation vector x component (radians). */
  public double rx;

  /** Rotation vector y component (radians). */
  public double ry;

  /** Rotation vector z component (radians). */
  public double rz;

  /** Default constructor. */
  public Twist3d() {}

  /**
   * Constructs a Twist3d with the given values.
   *
   * @param dx Change in x direction relative to robot.
   * @param dy Change in y direction relative to robot.
   * @param dz Change in z direction relative to robot.
   * @param rx Rotation vector x component.
   * @param ry Rotation vector y component.
   * @param rz Rotation vector z component.
   */
  public Twist3d(double dx, double dy, double dz, double rx, double ry, double rz) {
    this.dx = dx;
    this.dy = dy;
    this.dz = dz;
    this.rx = rx;
    this.ry = ry;
    this.rz = rz;
  }

  /**
   * Obtain a new Transform3d from a (constant curvature) velocity.
   *
   * <p>See <a href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">Controls
   * Engineering in the FIRST Robotics Competition</a> section 10.2 "Pose exponential" for a
   * derivation.
   *
   * <p>The twist is a change in pose in the robot's coordinate frame since the previous pose
   * update. When the user runs exp() on the twist, the user will receive the pose delta.
   *
   * <p>"Exp" represents the pose exponential, which is solving a differential equation moving the
   * pose forward in time.
   *
   * @return The pose delta of the robot.
   */
  public Transform3d exp() {
    double[] resultArray = Twist3dJNI.exp(dx, dy, dz, rx, ry, rz);
    return new Transform3d(
        resultArray[0],
        resultArray[1],
        resultArray[2],
        new Rotation3d(
            new Quaternion(resultArray[3], resultArray[4], resultArray[5], resultArray[6])));
  }

  @Override
  public String toString() {
    return String.format(
        "Twist3d(dX: %.2f, dY: %.2f, dZ: %.2f, rX: %.2f, rY: %.2f, rZ: %.2f)",
        dx, dy, dz, rx, ry, rz);
  }

  /**
   * Checks equality between this Twist3d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Twist3d other
        && Math.abs(other.dx - dx) < 1E-9
        && Math.abs(other.dy - dy) < 1E-9
        && Math.abs(other.dz - dz) < 1E-9
        && Math.abs(other.rx - rx) < 1E-9
        && Math.abs(other.ry - ry) < 1E-9
        && Math.abs(other.rz - rz) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(dx, dy, dz, rx, ry, rz);
  }

  /** Twist3d protobuf for serialization. */
  public static final Twist3dProto proto = new Twist3dProto();

  /** Twist3d struct for serialization. */
  public static final Twist3dStruct struct = new Twist3dStruct();
}
