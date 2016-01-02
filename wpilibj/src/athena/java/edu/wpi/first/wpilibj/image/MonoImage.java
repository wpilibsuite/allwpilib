/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.ni.vision.NIVision;
import com.ni.vision.NIVision.EllipseDescriptor;
import com.ni.vision.NIVision.CurveOptions;
import com.ni.vision.NIVision.ShapeDetectionOptions;
import com.ni.vision.NIVision.ROI;
import com.ni.vision.NIVision.DetectEllipsesResult;

/**
 * A grey scale image represented at a byte per pixel.
 *$
 * @author dtjones
 */
public class MonoImage extends ImageBase {

  /**
   * Create a new 0x0 image.
   */
  public MonoImage() throws NIVisionException {
    super(NIVision.ImageType.IMAGE_U8);
  }

  MonoImage(MonoImage sourceImage) {
    super(sourceImage);
  }

  public DetectEllipsesResult detectEllipses(EllipseDescriptor ellipseDescriptor,
      CurveOptions curveOptions, ShapeDetectionOptions shapeDetectionOptions, ROI roi)
      throws NIVisionException {
    return NIVision.imaqDetectEllipses(image, ellipseDescriptor, curveOptions,
        shapeDetectionOptions, roi);
  }

  public DetectEllipsesResult detectEllipses(EllipseDescriptor ellipseDescriptor)
      throws NIVisionException {
    return NIVision.imaqDetectEllipses(image, ellipseDescriptor, null, null, null);
  }
}
