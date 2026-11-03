// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Compile-time checks implemented as a javac plugin for WPILib annotations. {@link
 * org.wpilib.javacplugin.WPILibJavacPlugin} is the plugin entrypoint; GradleRIO bundles it with the
 * wpilibAnnotations dependencies group that template programs specify in the dependencies block of
 * the build.gradle file. Teams do not need to call it directly, since the Java runtime will
 * automatically load it from the annotation processor path.
 */
package org.wpilib.javacplugin;
