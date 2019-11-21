/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import java.io.IOException;
import java.util.Objects;

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import com.fasterxml.jackson.databind.deser.std.StdDeserializer;
import com.fasterxml.jackson.databind.ser.std.StdSerializer;

/**
 * Represents a 2d pose containing translational and rotational elements.
 */
@JsonSerialize(using = Pose2d.PoseSerializer.class)
@JsonDeserialize(using = Pose2d.PoseDeserializer.class)
public class Pose2d {
  private final Translation2d m_translation;
  private final Rotation2d m_rotation;

  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   * (Translation2d{0, 0} and Rotation{0})
   */
  public Pose2d() {
    m_translation = new Translation2d();
    m_rotation = new Rotation2d();
  }

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation    The rotational component of the pose.
   */
  public Pose2d(Translation2d translation, Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Convenience constructors that takes in x and y values directly instead of
   * having to construct a Translation2d.
   *
   * @param x        The x component of the translational component of the pose.
   * @param y        The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  @SuppressWarnings("ParameterName")
  public Pose2d(double x, double y, Rotation2d rotation) {
    m_translation = new Translation2d(x, y);
    m_rotation = rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose.
   *
   * <p>The matrix multiplication is as follows
   * [x_new]    [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new]    [0,    0,   1][transform.t]
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d plus(Transform2d other) {
    return transformBy(other);
  }

  /**
   * Returns the Transform2d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  public Transform2d minus(Pose2d other) {
    final var pose = this.relativeTo(other);
    return new Transform2d(pose.getTranslation(), pose.getRotation());
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the pose.
   */
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return The rotational component of the pose.
   */
  public Rotation2d getRotation() {
    return m_rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose.
   * See + operator for the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d transformBy(Transform2d other) {
    return new Pose2d(m_translation.plus(other.getTranslation().rotateBy(m_rotation)),
        m_rotation.plus(other.getRotation()));
  }

  /**
   * Returns the other pose relative to the current pose.
   *
   * <p>This function can often be used for trajectory tracking or pose
   * stabilization algorithms to get the error between the reference and the
   * current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that
   *              the current pose will be converted into.
   * @return The current pose relative to the new origin pose.
   */
  public Pose2d relativeTo(Pose2d other) {
    var transform = new Transform2d(other, this);
    return new Pose2d(transform.getTranslation(), transform.getRotation());
  }

  /**
   * Obtain a new Pose2d from a (constant curvature) velocity.
   *
   * <p>See <a href="https://file.tavsys.net/control/state-space-guide.pdf">
   * Controls Engineering in the FIRST Robotics Competition</a>
   * section on nonlinear pose estimation for derivation.
   *
   * <p>The twist is a change in pose in the robot's coordinate frame since the
   * previous pose update. When the user runs exp() on the previous known
   * field-relative pose with the argument being the twist, the user will
   * receive the new field-relative pose.
   *
   * <p>"Exp" represents the pose exponential, which is solving a differential
   * equation moving the pose forward in time.
   *
   * @param twist The change in pose in the robot's coordinate frame since the
   *              previous pose update. For example, if a non-holonomic robot moves forward
   *              0.01 meters and changes angle by 0.5 degrees since the previous pose update,
   *              the twist would be Twist2d{0.01, 0.0, toRadians(0.5)}
   * @return The new pose of the robot.
   */
  @SuppressWarnings("LocalVariableName")
  public Pose2d exp(Twist2d twist) {
    double dx = twist.dx;
    double dy = twist.dy;
    double dtheta = twist.dtheta;

    double sinTheta = Math.sin(dtheta);
    double cosTheta = Math.cos(dtheta);

    double s;
    double c;
    if (Math.abs(dtheta) < 1E-9) {
      s = 1.0 - 1.0 / 6.0 * dtheta * dtheta;
      c = 0.5 * dtheta;
    } else {
      s = sinTheta / dtheta;
      c = (1 - cosTheta) / dtheta;
    }
    var transform = new Transform2d(new Translation2d(dx * s - dy * c, dx * c + dy * s),
        new Rotation2d(cosTheta, sinTheta));

    return this.plus(transform);
  }

  /**
   * Returns a Twist2d that maps this pose to the end pose. If c is the output
   * of a.Log(b), then a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   * @return The twist that maps this to end.
   */
  public Twist2d log(Pose2d end) {
    final var transform = end.relativeTo(this);
    final var dtheta = transform.getRotation().getRadians();
    final var halfDtheta = dtheta / 2.0;

    final var cosMinusOne = transform.getRotation().getCos() - 1;

    double halfThetaByTanOfHalfDtheta;
    if (Math.abs(cosMinusOne) < 1E-9) {
      halfThetaByTanOfHalfDtheta = 1.0 - 1.0 / 12.0 * dtheta * dtheta;
    } else {
      halfThetaByTanOfHalfDtheta = -(halfDtheta * transform.getRotation().getSin()) / cosMinusOne;
    }

    Translation2d translationPart = transform.getTranslation().rotateBy(
        new Rotation2d(halfThetaByTanOfHalfDtheta, -halfDtheta)
    ).times(Math.hypot(halfThetaByTanOfHalfDtheta, halfDtheta));

    return new Twist2d(translationPart.getX(), translationPart.getY(), dtheta);
  }

  @Override
  public String toString() {
    return String.format("Pose2d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Pose2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Pose2d) {
      return ((Pose2d) obj).m_translation.equals(m_translation)
          && ((Pose2d) obj).m_rotation.equals(m_rotation);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }

  static class PoseSerializer extends StdSerializer<Pose2d> {
    PoseSerializer() {
      super(Pose2d.class);
    }

    @Override
    public void serialize(
            Pose2d value, JsonGenerator jgen, SerializerProvider provider)
            throws IOException, JsonProcessingException {

      jgen.writeStartObject();
      jgen.writeObjectField("translation", value.m_translation);
      jgen.writeObjectField("rotation", value.m_rotation);
      jgen.writeEndObject();
    }
  }

  static class PoseDeserializer extends StdDeserializer<Pose2d> {
    PoseDeserializer() {
      super(Pose2d.class);
    }

    @Override
    public Pose2d deserialize(JsonParser jp, DeserializationContext ctxt)
            throws IOException, JsonProcessingException {
      JsonNode node = jp.getCodec().readTree(jp);

      Translation2d translation =
              jp.getCodec().treeToValue(node.get("translation"), Translation2d.class);
      Rotation2d rotation = jp.getCodec().treeToValue(node.get("rotation"), Rotation2d.class);
      return new Pose2d(translation, rotation);
    }
  }
}
