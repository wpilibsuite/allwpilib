// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as a motor acting
 * against the force of gravity on a beam suspended at an angle).
 */
public class ControllerConstants {
  private ControllerConstants() {
    // prevents instantiation
  }

  public record FeedforwardConstants(double ks, double kv, double ka, double kg) {}

  public record FeedbackContants(double kp, double ki, double kd) {}
}
