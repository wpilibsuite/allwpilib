/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.sun.jna.Structure;

/**
 *
 * @author dtjones
 */
public class ShapeDetectionOptions extends Structure {

    int mode; //unsigned int  Specifies the method used when looking for the shape in the image. Combine values from the GeometricMatchingMode enumeration to specify the value of this element.
    int angleRanges; //RangeFloat*  An array of angle ranges, in degrees, where each range specifies how much you expect the shape to be rotated in the image. To decrease the search time, limit the degrees of rotation in which you expect to find the shape in the image. Set this element to NULL to allow all angles. This function ignores this range if mode does not include IMAQ_GEOMETRIC_MATCH_ROTATION_INVARIANT.
    int numAngleRanges;  //int  The size of the orientationRanges array.
    float scaleRangeMin; // RangeFloat  A range that specifies the sizes of the shapes you expect to be in the image, expressed as a ratio percentage representing the size of the pattern in the image divided by size of the original pattern multiplied by 100. This function ignores this range if mode does not include IMAQ_GEOMETRIC_MATCH_SCALE_INVARIANT
    float scaleRangeMax;
    double minMatchScore; // double  The minimum score a match can have for the function to consider the match valid. Acceptable values range from 0 to 1,000.

    public static final int IMAQ_GEOMETRIC_MATCH_SHIFT_INVARIANT = 0; // Searches for occurrences of the pattern in the image, assuming that the pattern is not rotated more than plus or minus 5 degrees.
    public static final int IMAQ_GEOMETRIC_MATCH_ROTATION_INVARIANT = 1; // Searches for occurrences of the pattern in the image with reduced restriction on the rotation of the pattern.
    public static final int IMAQ_GEOMETRIC_MATCH_SCALE_INVARIANT = 2; // Searches for occurrences of the pattern in the image with reduced restriction on the size of the pattern.
    public static final int IMAQ_GEOMETRIC_MATCH_OCCLUSION_INVARIANT = 4; // Searches for occurrences of the pattern in the image, allowing for a specified percentage of the pattern to be occluded.


    public ShapeDetectionOptions(int mode, int angleRanges, int numAngleRanges, float scaleRangeMin, float scaleRangeMax, double minMatchScore) {
        this.mode = mode;
        this.angleRanges = angleRanges;
        this.numAngleRanges = numAngleRanges;
        this.scaleRangeMin = scaleRangeMin;
        this.scaleRangeMax = scaleRangeMax;
        this.minMatchScore = minMatchScore;
        allocateMemory();
        write();
    }


    public void read() {
        mode = backingNativeMemory.getInt(0);
        angleRanges = backingNativeMemory.getInt(4);
        numAngleRanges = backingNativeMemory.getInt(8);
        scaleRangeMin = backingNativeMemory.getFloat(12);
        scaleRangeMax = backingNativeMemory.getFloat(16);
        minMatchScore = backingNativeMemory.getDouble(20);
    }

    public void write() {
        backingNativeMemory.setInt(0, mode);
        backingNativeMemory.setInt(4, angleRanges);
        backingNativeMemory.setInt(8, numAngleRanges);
        backingNativeMemory.setFloat(12, scaleRangeMin);
        backingNativeMemory.setFloat(16, scaleRangeMax);
        backingNativeMemory.setDouble(20, minMatchScore);


    }

    public int size() {
        return 28;
    }

    public void free() {
        release();
    }

}
