// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

@SuppressWarnings("requires-transitive-automatic")
open module wpilib.apriltag {
  requires transitive org.opencv;
  requires wpilib.math;
  requires wpilib.util;
  requires io.avaje.json;
  requires io.avaje.jsonb;
  requires io.avaje.jsonb.plugin;
  requires io.avaje.inject;

  exports org.wpilib.vision.apriltag;
  exports org.wpilib.vision.apriltag.jni;

  provides io.avaje.jsonb.spi.JsonbExtension with
      org.wpilib.vision.apriltag.ApriltagJsonComponent,
      org.wpilib.vision.apriltag.jsonb.GeneratedJsonComponent;
}
