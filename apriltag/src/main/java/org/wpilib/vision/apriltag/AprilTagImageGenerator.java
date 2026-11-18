// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag;

import org.wpilib.util.RawFrame;
import org.wpilib.vision.apriltag.jni.AprilTagJNI;

/** AprilTag image generation utilities. */
public final class AprilTagImageGenerator {
  private AprilTagImageGenerator() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Generates a RawFrame containing the AprilTag with the id from family 16h5.
   *
   * @param id ID
   * @return A RawFrame containing the AprilTag image
   */
  public static RawFrame generate16h5AprilTagImage(int id) {
    RawFrame frame = new RawFrame();
    AprilTagJNI.generate16h5AprilTagImage(frame, frame.getNativeObj(), id);
    return frame;
  }

  /**
   * Generates a RawFrame containing the AprilTag with the id from family 36h11.
   *
   * @param id ID
   * @return A RawFrame containing the AprilTag image
   */
  public static RawFrame generate36h11AprilTagImage(int id) {
    RawFrame frame = new RawFrame();
    AprilTagJNI.generate36h11AprilTagImage(frame, frame.getNativeObj(), id);
    return frame;
  }
}
