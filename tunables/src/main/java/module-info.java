// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Tunables is WPILib's runtime adjustment API for values that robot programs want to expose to
 * dashboards or debug tools. It lets code declare adjustable values, grouped complex objects, and
 * chooser-style selections while routing storage and updates through pluggable backends.
 */
module wpilib.tunable {
  requires io.avaje.json;
  requires io.avaje.jsonb;
  requires transitive wpilib.util;

  exports org.wpilib.tunable;
  exports org.wpilib.tunable.util;
}
