// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Provides APIs for telemetry and data logging, used by code generated based on the presence of the
 * {@link org.wpilib.epilogue.Logged} annotation on classes, fields, and methods. The epilogue
 * annotation processor - in a separate module - performs the actual code generation, including
 * generating the {@code org.wpilib.epilogue.generated.Epilogue} entrypoint.
 */
open module wpilib.epilogue {
  requires transitive wpilib.telemetry;
  requires transitive wpilib.units;

  exports org.wpilib.epilogue;
  exports org.wpilib.epilogue.logging;
  exports org.wpilib.epilogue.logging.errors;
}
