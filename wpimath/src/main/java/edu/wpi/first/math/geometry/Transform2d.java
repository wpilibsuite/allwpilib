// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.proto.Geometry2D.ProtobufTransform2d;
import edu.wpi.first.units.Angle;
import edu.wpi.first.units.Distance;
import edu.wpi.first.units.Measure;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;
import java.util.Objects;
import us.hebi.quickbuf.Descriptors.Descriptor;

/** Represents a transformation for a Pose2d in the pose's frame. */
public class Transform2d {
  private final Translation2d m_translation;
  private final Rotation2d m_rotation;

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last The final pose for the transformation.
   */
  public Transform2d(Pose2d initial, Pose2d last) {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    m_translation =
        last.getTranslation()
            .minus(initial.getTranslation())
            .rotateBy(initial.getRotation().unaryMinus());

    m_rotation = last.getRotation().minus(initial.getRotation());
  }

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  public Transform2d(Translation2d translation, Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Constructs a transform with x and y translations instead of a separate Translation2d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform2d(double x, double y, Rotation2d rotation) {
    m_translation = new Translation2d(x, y);
    m_rotation = rotation;
  }

  /**
   * Constructs a transform with x and y translations instead of a separate Translation2d. The X and
   * Y translations will be converted to and tracked as meters.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform2d(Measure<Distance> x, Measure<Distance> y, Measure<Angle> rotation) {
    this(x.in(Meters), y.in(Meters), new Rotation2d(rotation));
  }

  /** Constructs the identity transform -- maps an initial pose to itself. */
  public Transform2d() {
    m_translation = new Translation2d();
    m_rotation = new Rotation2d();
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  public Transform2d times(double scalar) {
    return new Transform2d(m_translation.times(scalar), m_rotation.times(scalar));
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  public Transform2d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to the orientation of
   * the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  public Transform2d plus(Transform2d other) {
    return new Transform2d(new Pose2d(), new Pose2d().transformBy(this).transformBy(other));
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the transform.
   */
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  public double getX() {
    return m_translation.getX();
  }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  public double getY() {
    return m_translation.getY();
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  public Rotation2d getRotation() {
    return m_rotation;
  }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  public Transform2d inverse() {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return new Transform2d(
        getTranslation().unaryMinus().rotateBy(getRotation().unaryMinus()),
        getRotation().unaryMinus());
  }

  @Override
  public String toString() {
    return String.format("Transform2d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Transform2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Transform2d) {
      return ((Transform2d) obj).m_translation.equals(m_translation)
          && ((Transform2d) obj).m_rotation.equals(m_rotation);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }

  public static final class AStruct implements Struct<Transform2d> {
    @Override
    public Class<Transform2d> getTypeClass() {
      return Transform2d.class;
    }

    @Override
    public String getTypeString() {
      return "struct:Transform2d";
    }

    @Override
    public int getSize() {
      return Translation2d.struct.getSize() + Rotation2d.struct.getSize();
    }

    @Override
    public String getSchema() {
      return "Translation2d translation;Rotation2d rotation";
    }

    @Override
    public Struct<?>[] getNested() {
      return new Struct<?>[] {Translation2d.struct, Rotation2d.struct};
    }

    @Override
    public Transform2d unpack(ByteBuffer bb) {
      Translation2d translation = Translation2d.struct.unpack(bb);
      Rotation2d rotation = Rotation2d.struct.unpack(bb);
      return new Transform2d(translation, rotation);
    }

    @Override
    public void pack(ByteBuffer bb, Transform2d value) {
      Translation2d.struct.pack(bb, value.m_translation);
      Rotation2d.struct.pack(bb, value.m_rotation);
    }
  }

  public static final AStruct struct = new AStruct();

  public static final class AProto implements Protobuf<Transform2d, ProtobufTransform2d> {
    @Override
    public Class<Transform2d> getTypeClass() {
      return Transform2d.class;
    }

    @Override
    public Descriptor getDescriptor() {
      return ProtobufTransform2d.getDescriptor();
    }

    @Override
    public Protobuf<?, ?>[] getNested() {
      return new Protobuf<?, ?>[] {Translation2d.proto, Rotation2d.proto};
    }

    @Override
    public ProtobufTransform2d createMessage() {
      return ProtobufTransform2d.newInstance();
    }

    @Override
    public Transform2d unpack(ProtobufTransform2d msg) {
      return new Transform2d(
          Translation2d.proto.unpack(msg.getTranslation()),
          Rotation2d.proto.unpack(msg.getRotation()));
    }

    @Override
    public void pack(ProtobufTransform2d msg, Transform2d value) {
      Translation2d.proto.pack(msg.getMutableTranslation(), value.m_translation);
      Rotation2d.proto.pack(msg.getMutableRotation(), value.m_rotation);
    }
  }

  public static final AProto proto = new AProto();
}
