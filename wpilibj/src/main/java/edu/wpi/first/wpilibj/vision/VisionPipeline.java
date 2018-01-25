/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

import org.opencv.core.Mat;

/**
 * A vision pipeline is responsible for running a group of
 * OpenCV algorithms to extract data from an image.
 *
 * @see VisionRunner
 * @see VisionThread
 */
public interface VisionPipeline {

  /**
   * Processes the image input and sets the result objects.
   * Implementations should make these objects accessible.
   */
  void process(Mat image);

}
