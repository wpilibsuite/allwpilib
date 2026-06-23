// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

module wpilib.gamefields {
  requires io.avaje.json;
  requires io.avaje.jsonb;
  requires io.avaje.inject;
  requires io.avaje.jsonb.plugin;

  exports org.wpilib.fields;

  provides io.avaje.jsonb.spi.JsonbExtension with
      org.wpilib.fields.jsonb.GeneratedJsonComponent;
}
