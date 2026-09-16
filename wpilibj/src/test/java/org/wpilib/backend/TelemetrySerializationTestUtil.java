// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.util.List;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.proto.detail.ProtobufTranslation2d;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;
import us.hebi.quickbuf.Descriptors.Descriptor;

final class TelemetrySerializationTestUtil {
  private TelemetrySerializationTestUtil() {}

  static final class Translation2dStructWrapper implements Struct<Translation2d> {
    Translation2dStructWrapper(String typeName) {
      m_typeName = typeName;
    }

    @Override
    public Class<Translation2d> getTypeClass() {
      return Translation2d.class;
    }

    @Override
    public String getTypeName() {
      return m_typeName;
    }

    @Override
    public int getSize() {
      return Translation2d.struct.getSize();
    }

    @Override
    public String getSchema() {
      return Translation2d.struct.getSchema();
    }

    @Override
    public Translation2d unpack(ByteBuffer bb) {
      return Translation2d.struct.unpack(bb);
    }

    @Override
    public void pack(ByteBuffer bb, Translation2d value) {
      if (m_throwOnPack) {
        throw new IllegalStateException("broken struct pack");
      }
      Translation2d.struct.pack(bb, value);
    }

    void setThrowOnPack(boolean throwOnPack) {
      m_throwOnPack = throwOnPack;
    }

    private final String m_typeName;
    private boolean m_throwOnPack;
  }

  static final class Translation2dProtobufWrapper
      implements Protobuf<Translation2d, ProtobufTranslation2d> {
    @Override
    public Class<Translation2d> getTypeClass() {
      return Translation2d.class;
    }

    @Override
    public Descriptor getDescriptor() {
      return Translation2d.proto.getDescriptor();
    }

    @Override
    public ProtobufTranslation2d createMessage() {
      return Translation2d.proto.createMessage();
    }

    @Override
    public Translation2d unpack(ProtobufTranslation2d msg) {
      return Translation2d.proto.unpack(msg);
    }

    @Override
    public void pack(ProtobufTranslation2d msg, Translation2d value) {
      if (m_throwOnPack) {
        throw new IllegalStateException("broken protobuf pack");
      }
      Translation2d.proto.pack(msg, value);
    }

    void setThrowOnPack(boolean throwOnPack) {
      m_throwOnPack = throwOnPack;
    }

    private boolean m_throwOnPack;
  }

  static void assertWarning(List<String> warnings, String path, String msg) {
    assertTrue(
        warnings.stream().anyMatch(warning -> warning.contains(path) && warning.contains(msg)),
        () -> "Missing warning containing '" + path + "' and '" + msg + "' in " + warnings);
  }
}
