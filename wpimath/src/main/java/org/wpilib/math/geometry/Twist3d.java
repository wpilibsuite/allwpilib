// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import java.util.Objects;
import org.wpilib.math.geometry.proto.Twist3dProto;
import org.wpilib.math.geometry.struct.Twist3dStruct;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.util.Nat;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * A change in distance along a 3D arc since the last pose update. We can use ideas from
 * differential calculus to create new Pose3d objects from a Twist3d and vice versa.
 *
 * <p>A Twist can be used to represent a difference between two poses.
 */
public final class Twist3d implements ProtobufSerializable, StructSerializable {
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
    // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf

    var u = VecBuilder.fill(dx, dy, dz);
    var rvec = VecBuilder.fill(rx, ry, rz);
    var omega = GeometryUtil.rotationVectorToMatrix(rvec);
    var omegaSq = omega.times(omega);
    double theta = rvec.norm();
    double thetaSq = theta * theta;

    double A;
    double B;
    double C;
    if (Math.abs(theta) < 1E-7) {
      // Taylor Expansions around θ = 0
      // A = 1/1! - θ²/3! + θ⁴/5!
      // B = 1/2! - θ²/4! + θ⁴/6!
      // C = 1/3! - θ²/5! + θ⁴/7!
      // sources:
      // A:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
      // B:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      // C:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      A = 1 - thetaSq / 6 + thetaSq * thetaSq / 120;
      B = 1 / 2.0 - thetaSq / 24 + thetaSq * thetaSq / 720;
      C = 1 / 6.0 - thetaSq / 120 + thetaSq * thetaSq / 5040;
    } else {
      // A = sinθ/θ
      // B = (1 - cosθ)/θ²
      // C = (1 - A)/θ²
      A = Math.sin(theta) / theta;
      B = (1 - Math.cos(theta)) / thetaSq;
      C = (1 - A) / thetaSq;
    }

    var R = Matrix.eye(Nat.N3()).plus(omega.times(A)).plus(omegaSq.times(B));
    var V = Matrix.eye(Nat.N3()).plus(omega.times(B)).plus(omegaSq.times(C));

    var translation_component = V.times(u);

    return new Transform3d(
        new Translation3d(
            translation_component.get(0, 0),
            translation_component.get(1, 0),
            translation_component.get(2, 0)),
        new Rotation3d(R));
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
