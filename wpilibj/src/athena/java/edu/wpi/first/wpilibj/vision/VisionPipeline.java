/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

import org.opencv.core.Mat;

/**
 * A vision pipeline is responsible for running a group of OpenCV algorithms to extract data from an image.
 */
public interface VisionPipeline {

  /**
   * Sets the image input to this vision pipeline. This should be called before {@link #process()}.
   *
   * @param image the image to extract data from
   */
  void setImageInput(Mat image);

  /**
   * Processes the image input and sets the result objects. Implementations should make these objects accessible.
   */
  void process();

}
