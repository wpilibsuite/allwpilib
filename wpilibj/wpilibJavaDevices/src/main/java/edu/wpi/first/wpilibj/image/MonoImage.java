/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.image;

/**
 * A grey scale image represented at a byte per pixel.
 * @author dtjones
 */
public class MonoImage extends Image {

    /**
     * Create a new 0x0 image.
     */
    public MonoImage() throws NIVisionException {
        super(NIVision.ImageType.imaqImageU8);
    }

    MonoImage(MonoImage sourceImage) {
        super(sourceImage);
    }

    public EllipseMatch[] detectEllipses(EllipseDescriptor ellipseDescriptor,
                                         CurveOptions curveOptions, ShapeDetectionOptions shapeDetectionOptions,
                                         RegionOfInterest roi) throws NIVisionException {
        return NIVision.detectEllipses(this, ellipseDescriptor, curveOptions, shapeDetectionOptions, roi);
    }

    public EllipseMatch[] detectEllipses(EllipseDescriptor ellipseDescriptor)
    throws NIVisionException {
        return NIVision.detectEllipses(this, ellipseDescriptor, null, null, null);
    }
}
