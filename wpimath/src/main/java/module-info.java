// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib math module. This defines APIs for geometry, kinematics, odometry, path planning,
 * control algorithms, and physics simulators for simple mechanisms.
 */
open module wpilib.math {
  requires com.fasterxml.jackson.annotation;
  requires transitive ejml.all;
  requires us.hebi.quickbuf.runtime;
  requires transitive wpilib.units;
  requires wpilib.util;

  exports edu.wpi.first.math;
  exports edu.wpi.first.math.controller;
  exports edu.wpi.first.math.controller.proto;
  exports edu.wpi.first.math.controller.struct;
  exports edu.wpi.first.math.estimator;
  exports edu.wpi.first.math.filter;
  exports edu.wpi.first.math.geometry;
  exports edu.wpi.first.math.geometry.proto;
  exports edu.wpi.first.math.geometry.struct;
  exports edu.wpi.first.math.interpolation;
  exports edu.wpi.first.math.jni; // Probably unnecessary
  exports edu.wpi.first.math.kinematics;
  exports edu.wpi.first.math.kinematics.proto;
  exports edu.wpi.first.math.kinematics.struct;
  exports edu.wpi.first.math.optimization;
  exports edu.wpi.first.math.path;
  exports edu.wpi.first.math.proto;
  exports edu.wpi.first.math.spline;
  exports edu.wpi.first.math.spline.proto;
  exports edu.wpi.first.math.spline.struct;
  exports edu.wpi.first.math.struct;
  exports edu.wpi.first.math.system;
  exports edu.wpi.first.math.system.plant;
  exports edu.wpi.first.math.system.plant.proto;
  exports edu.wpi.first.math.system.plant.struct;
  exports edu.wpi.first.math.system.proto;
  exports edu.wpi.first.math.system.struct;
  exports edu.wpi.first.math.trajectory;
  exports edu.wpi.first.math.trajectory.constraint;
  exports edu.wpi.first.math.trajectory.proto;
  exports edu.wpi.first.math.util;
  exports edu.wpi.first.math.numbers;
}
