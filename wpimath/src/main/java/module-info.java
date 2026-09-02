// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines APIs for geometry, kinematics, odometry, path planning, control algorithms, and physics
 * simulators for simple mechanisms.
 */
open module wpilib.math {
  requires transitive ejml.java9module;
  requires transitive us.hebi.quickbuf.runtime;
  requires transitive wpilib.units;
  requires transitive io.avaje.jsonb;
  requires wpilib.annotation;
  requires transitive wpilib.telemetry;
  requires transitive wpilib.tunable;
  requires wpilib.util;

  exports org.wpilib.math.autodiff;
  exports org.wpilib.math.controller;
  exports org.wpilib.math.controller.proto;
  exports org.wpilib.math.controller.proto.detail;
  exports org.wpilib.math.controller.struct;
  exports org.wpilib.math.estimator;
  exports org.wpilib.math.filter;
  // org.wpilib.math.filter.internal is not exported
  exports org.wpilib.math.geometry;
  exports org.wpilib.math.geometry.proto;
  exports org.wpilib.math.geometry.proto.detail;
  exports org.wpilib.math.geometry.struct;
  exports org.wpilib.math.interpolation;
  exports org.wpilib.math.jni;
  exports org.wpilib.math.kinematics;
  exports org.wpilib.math.kinematics.proto;
  exports org.wpilib.math.kinematics.proto.detail;
  exports org.wpilib.math.kinematics.struct;
  exports org.wpilib.math.linalg;
  exports org.wpilib.math.linalg.proto;
  exports org.wpilib.math.linalg.proto.detail;
  exports org.wpilib.math.linalg.struct;
  exports org.wpilib.math.numbers;
  exports org.wpilib.math.optimization;
  exports org.wpilib.math.optimization.ocp;
  exports org.wpilib.math.optimization.solver;
  exports org.wpilib.math.path;
  exports org.wpilib.math.random;
  exports org.wpilib.math.shape;
  exports org.wpilib.math.shape.proto;
  exports org.wpilib.math.shape.proto.detail;
  exports org.wpilib.math.shape.struct;
  exports org.wpilib.math.spline;
  exports org.wpilib.math.spline.proto;
  exports org.wpilib.math.spline.proto.detail;
  exports org.wpilib.math.spline.struct;
  exports org.wpilib.math.system;
  exports org.wpilib.math.system.proto;
  exports org.wpilib.math.system.proto.detail;
  exports org.wpilib.math.system.struct;
  exports org.wpilib.math.trajectory;
  exports org.wpilib.math.trajectory.constraint;
  exports org.wpilib.math.trajectory.proto;
  exports org.wpilib.math.trajectory.proto.detail;
  exports org.wpilib.math.trajectory.struct;
  exports org.wpilib.math.util;

  provides io.avaje.jsonb.spi.JsonbExtension with
      org.wpilib.math.jsonb.GeneratedJsonComponent;
}
