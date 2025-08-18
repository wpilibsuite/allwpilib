// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.vision;

import org.opencv.core.Mat;

/**
 * A vision pipeline is responsible for running a group of OpenCV algorithms to extract data from an
 * image.
 *
 * @see VisionRunner
 * @see VisionThread
 */
@FunctionalInterface
public interface VisionPipeline {
  /**
   * Processes the image input and sets the result objects. Implementations should make these
   * objects accessible.
   *
   * @param image The image to process.
   */
  void process(Mat image);
}
