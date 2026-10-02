// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.proto.detail.ProtobufTranslation2d;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import us.hebi.quickbuf.Descriptors.Descriptor;

public record ProtoOnlyTranslation(Translation2d value) implements ProtobufSerializable {
  public ProtoOnlyTranslation(double x, double y) {
    this(new Translation2d(x, y));
  }

  public static final Protobuf<ProtoOnlyTranslation, ProtobufTranslation2d> proto =
      new Protobuf<ProtoOnlyTranslation, ProtobufTranslation2d>() {
        @Override
        public Class<ProtoOnlyTranslation> getTypeClass() {
          return ProtoOnlyTranslation.class;
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
        public ProtoOnlyTranslation unpack(ProtobufTranslation2d msg) {
          return new ProtoOnlyTranslation(Translation2d.proto.unpack(msg));
        }

        @Override
        public void pack(ProtobufTranslation2d msg, ProtoOnlyTranslation value) {
          Translation2d.proto.pack(msg, value.value());
        }
      };
}
