// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;

/** Represents a 3D pose containing translational and rotational elements. */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Pose3d implements Interpolatable<Pose3d> {
  private final Translation3d m_translation;
  private final Rotation3d m_rotation;

  /** Constructs a pose at the origin facing toward the positive X axis. */
  public Pose3d() {
    m_translation = new Translation3d();
    m_rotation = new Rotation3d();
  }

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  @JsonCreator
  public Pose3d(
      @JsonProperty(required = true, value = "translation") Translation3d translation,
      @JsonProperty(required = true, value = "rotation") Rotation3d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Constructs a pose with x, y, and z translations instead of a separate Translation3d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param z The z component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  public Pose3d(double x, double y, double z, Rotation3d rotation) {
    m_translation = new Translation3d(x, y, z);
    m_rotation = rotation;
  }

  /**
   * Constructs a 3D pose from a 2D pose in the X-Y plane.
   *
   * @param pose The 2D pose.
   */
  public Pose3d(Pose2d pose) {
    m_translation = new Translation3d(pose.getX(), pose.getY(), 0.0);
    m_rotation = new Rotation3d(0.0, 0.0, pose.getRotation().getRadians());
  }

  /**
   * Transforms the pose by the given transformation and returns the new transformed pose.
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose3d plus(Transform3d other) {
    return transformBy(other);
  }

  /**
   * Returns the Transform3d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  public Transform3d minus(Pose3d other) {
    final var pose = this.relativeTo(other);
    return new Transform3d(pose.getTranslation(), pose.getRotation());
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the pose.
   */
  @JsonProperty
  public Translation3d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the X component of the pose's translation.
   *
   * @return The x component of the pose's translation.
   */
  public double getX() {
    return m_translation.getX();
  }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  public double getY() {
    return m_translation.getY();
  }

  /**
   * Returns the Z component of the pose's translation.
   *
   * @return The z component of the pose's translation.
   */
  public double getZ() {
    return m_translation.getZ();
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return The rotational component of the pose.
   */
  @JsonProperty
  public Rotation3d getRotation() {
    return m_rotation;
  }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Pose3d.
   */
  public Pose3d times(double scalar) {
    return new Pose3d(m_translation.times(scalar), m_rotation.times(scalar));
  }

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Pose3d.
   */
  public Pose3d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose. See + operator for
   * the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose3d transformBy(Transform3d other) {
    return new Pose3d(
        m_translation.plus(other.getTranslation().rotateBy(m_rotation)),
        other.getRotation().plus(m_rotation));
  }

  /**
   * Returns the current pose relative to the given pose.
   *
   * <p>This function can often be used for trajectory tracking or pose stabilization algorithms to
   * get the error between the reference and the current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that the current pose will
   *     be converted into.
   * @return The current pose relative to the new origin pose.
   */
  public Pose3d relativeTo(Pose3d other) {
    var transform = new Transform3d(other, this);
    return new Pose3d(transform.getTranslation(), transform.getRotation());
  }

  /**
   * Obtain a new Pose3d from a (constant curvature) velocity.
   *
   * <p>The twist is a change in pose in the robot's coordinate frame since the previous pose
   * update. When the user runs exp() on the previous known field-relative pose with the argument
   * being the twist, the user will receive the new field-relative pose.
   *
   * <p>"Exp" represents the pose exponential, which is solving a differential equation moving the
   * pose forward in time.
   *
   * @param twist The change in pose in the robot's coordinate frame since the previous pose update.
   *     For example, if a non-holonomic robot moves forward 0.01 meters and changes angle by 0.5
   *     degrees since the previous pose update, the twist would be Twist3d(0.01, 0.0, 0.0, new new
   *     Rotation3d(0.0, 0.0, Units.degreesToRadians(0.5))).
   * @return The new pose of the robot.
   */
  public Pose3d exp(Twist3d twist) {
    final var Omega = rotationVectorToMatrix(VecBuilder.fill(twist.rx, twist.ry, twist.rz));
    final var OmegaSq = Omega.times(Omega);

    double thetaSq = twist.rx * twist.rx + twist.ry * twist.ry + twist.rz * twist.rz;

    // Get left Jacobian of SO3. See first line in right column of
    // http://asrl.utias.utoronto.ca/~tdb/bib/barfoot_ser17_identities.pdf
    Matrix<N3, N3> J;
    if (thetaSq < 1E-9 * 1E-9) {
      // J = I + 0.5ω
      J = Matrix.eye(Nat.N3()).plus(Omega.times(0.5));
    } else {
      double theta = Math.sqrt(thetaSq);
      // J = I + (1 − cos(θ))/θ² ω + (θ − sin(θ))/θ³ ω²
      J =
          Matrix.eye(Nat.N3())
              .plus(Omega.times((1.0 - Math.cos(theta)) / thetaSq))
              .plus(OmegaSq.times((theta - Math.sin(theta)) / (thetaSq * theta)));
    }

    // Get translation component
    final var translation =
        J.times(new MatBuilder<>(Nat.N3(), Nat.N1()).fill(twist.dx, twist.dy, twist.dz));

    final var transform =
        new Transform3d(
            new Translation3d(translation.get(0, 0), translation.get(1, 0), translation.get(2, 0)),
            new Rotation3d(twist.rx, twist.ry, twist.rz));

    return this.plus(transform);
  }

  /**
   * Returns a Twist3d that maps this pose to the end pose. If c is the output of a.Log(b), then
   * a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   * @return The twist that maps this to end.
   */
  public Twist3d log(Pose3d end) {
    final var transform = end.relativeTo(this);

    final var rotVec = transform.getRotation().getQuaternion().toRotationVector();

    final var Omega = rotationVectorToMatrix(rotVec);
    final var OmegaSq = Omega.times(Omega);

    double thetaSq =
        rotVec.get(0, 0) * rotVec.get(0, 0)
            + rotVec.get(1, 0) * rotVec.get(1, 0)
            + rotVec.get(2, 0) * rotVec.get(2, 0);

    // Get left Jacobian inverse of SO3. See fourth line in right column of
    // http://asrl.utias.utoronto.ca/~tdb/bib/barfoot_ser17_identities.pdf
    Matrix<N3, N3> Jinv;
    if (thetaSq < 1E-9 * 1E-9) {
      // J⁻¹ = I − 0.5ω + 1/12 ω²
      Jinv = Matrix.eye(Nat.N3()).minus(Omega.times(0.5)).plus(OmegaSq.times(1.0 / 12.0));
    } else {
      double theta = Math.sqrt(thetaSq);
      double halfTheta = 0.5 * theta;

      // J⁻¹ = I − 0.5ω + (1 − 0.5θ cos(θ/2) / sin(θ/2))/θ² ω²
      Jinv =
          Matrix.eye(Nat.N3())
              .minus(Omega.times(0.5))
              .plus(
                  OmegaSq.times(
                      (1.0 - 0.5 * theta * Math.cos(halfTheta) / Math.sin(halfTheta)) / thetaSq));
    }

    // Get dtranslation component
    final var dtranslation =
        Jinv.times(
            new MatBuilder<>(Nat.N3(), Nat.N1())
                .fill(transform.getX(), transform.getY(), transform.getZ()));

    return new Twist3d(
        dtranslation.get(0, 0),
        dtranslation.get(1, 0),
        dtranslation.get(2, 0),
        rotVec.get(0, 0),
        rotVec.get(1, 0),
        rotVec.get(2, 0));
  }

  /**
   * Returns a Pose2d representing this Pose3d projected into the X-Y plane.
   *
   * @return A Pose2d representing this Pose3d projected into the X-Y plane.
   */
  public Pose2d toPose2d() {
    return new Pose2d(m_translation.toTranslation2d(), m_rotation.toRotation2d());
  }

  @Override
  public String toString() {
    return String.format("Pose3d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Pose3d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Pose3d) {
      return ((Pose3d) obj).m_translation.equals(m_translation)
          && ((Pose3d) obj).m_rotation.equals(m_rotation);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }

  @Override
  public Pose3d interpolate(Pose3d endValue, double t) {
    if (t < 0) {
      return this;
    } else if (t >= 1) {
      return endValue;
    } else {
      var twist = this.log(endValue);
      var scaledTwist =
          new Twist3d(
              twist.dx * t, twist.dy * t, twist.dz * t, twist.rx * t, twist.ry * t, twist.rz * t);
      return this.exp(scaledTwist);
    }
  }

  /**
   * Applies the hat operator to a rotation vector.
   *
   * <p>It takes a rotation vector and returns the corresponding matrix representation of the Lie
   * algebra element (a 3x3 rotation matrix).
   *
   * @param rotation The rotation vector.
   * @return The rotation vector as a 3x3 rotation matrix.
   */
  private Matrix<N3, N3> rotationVectorToMatrix(Vector<N3> rotation) {
    // Given a rotation vector <a, b, c>,
    //         [ 0 -c  b]
    // Omega = [ c  0 -a]
    //         [-b  a  0]
    return new MatBuilder<>(Nat.N3(), Nat.N3())
        .fill(
            0.0,
            -rotation.get(2, 0),
            rotation.get(1, 0),
            rotation.get(2, 0),
            0.0,
            -rotation.get(0, 0),
            -rotation.get(1, 0),
            rotation.get(0, 0),
            0.0);
  }
}
