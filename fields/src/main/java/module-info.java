// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** Defines APIs for structured game field definitions and their JSON serialization support. */
module wpilib.gamefields {
  requires io.avaje.json;
  requires io.avaje.jsonb;

  exports org.wpilib.fields;

  provides io.avaje.jsonb.spi.JsonbExtension with
      org.wpilib.fields.jsonb.GeneratedJsonComponent;
}
