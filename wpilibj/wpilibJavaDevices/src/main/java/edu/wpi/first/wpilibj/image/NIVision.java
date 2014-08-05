/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.image;

import com.sun.jna.BlockingFunction;
import com.sun.jna.Function;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.TaskExecutor;
import com.sun.jna.ptr.IntByReference;
import com.sun.squawk.microedition.io.FileConnection;
import edu.wpi.first.wpilibj.util.BoundaryException;
import java.util.Vector;

/**
 * Class for interfacing with the NIVision libraries
 * @author dtjones
 */
public class NIVision {

    private NIVision() {
    }

    private static final TaskExecutor taskExecutor = new TaskExecutor("nivision task");

    /**
     * Enumeration representing the possible types of imaq images
     */
    public static class ImageType {

        public final int value;
        /** The image type is 8-bit unsigned integer grayscale. */
        public static final ImageType imaqImageU8 = new ImageType(0);
        /** The image type is 16-bit unsigned integer grayscale. */
        public static final ImageType imaqImageU16 = new ImageType(7);
        /** The image type is 16-bit signed integer grayscale. */
        public static final ImageType imaqImageI16 = new ImageType(1);
        /** The image type is 32-bit floating-point grayscale. */
        public static final ImageType imaqImageSGL = new ImageType(2);
        /** The image type is complex. */
        public static final ImageType imaqImageComplex = new ImageType(3);
        /** The image type is RGB color. */
        public static final ImageType imaqImageRGB = new ImageType(4);
        /** The image type is HSL color. */
        public static final ImageType imaqImageHSL = new ImageType(5);
        /** The image type is 64-bit unsigned RGB color. */
        public static final ImageType imaqImageRGBU64 = new ImageType(6);
        /** Reserved */
        public static final ImageType imaqImageTypeSizeGuard = new ImageType(0xFFFFFFFF);

        private ImageType(int value) {
            this.value = value;
        }
    }

    /**
     * Enumerations representing the possible color spaces to operate in
     */
    public static class ColorMode {

        public final int value;
        /** The function operates in the RGB (Red, Blue, Green) color space. */
        public static final ColorMode IMAQ_RGB = new ColorMode(0);
        /** The function operates in the HSL (Hue, Saturation, Luminance) color space. */
        public static final ColorMode IMAQ_HSL = new ColorMode(1);
        /** The function operates in the HSV (Hue, Saturation, Value) color space. */
        public static final ColorMode IMAQ_HSV = new ColorMode(2);
        /** The function operates in the HSI (Hue, Saturation, Intensity) color space. */
        public static final ColorMode IMAQ_HSI = new ColorMode(3);
        /** The function operates in the CIE L*a*b* color space. */
        public static final ColorMode IMAQ_CIE = new ColorMode(4);
        /** The function operates in the CIE XYZ color space. */
        public static final ColorMode IMAQ_CIEXYZ = new ColorMode(5);
        public static final ColorMode IMAQ_COLOR_MODE_SIZE_GUARD = new ColorMode(0xFFFFFFFF);

        private ColorMode(int value) {
            this.value = value;
        }
    }

    public static class MeasurementType {

        public final int value;
        /** X-coordinate of the point representing the average position of the total particle mass, assuming every point in the particle has a constant density. */
        public static final MeasurementType IMAQ_MT_CENTER_OF_MASS_X = new MeasurementType(0);
        /** Y-coordinate of the point representing the average position of the total particle mass, assuming every point in the particle has a constant density. */
        public static final MeasurementType IMAQ_MT_CENTER_OF_MASS_Y = new MeasurementType(1);
        /** X-coordinate of the highest, leftmost particle pixel. */
        public static final MeasurementType IMAQ_MT_FIRST_PIXEL_X = new MeasurementType(2);
        /** Y-coordinate of the highest, leftmost particle pixel. */
        public static final MeasurementType IMAQ_MT_FIRST_PIXEL_Y = new MeasurementType(3);
        /** X-coordinate of the leftmost particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_LEFT = new MeasurementType(4);
        /** Y-coordinate of highest particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_TOP = new MeasurementType(5);
        /** X-coordinate of the rightmost particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_RIGHT = new MeasurementType(6);
        /** Y-coordinate of the lowest particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_BOTTOM = new MeasurementType(7);
        /** X-coordinate of the start of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER_START_X = new MeasurementType(8);
        /** Y-coordinate of the start of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER_START_Y = new MeasurementType(9);
        /** X-coordinate of the end of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER_END_X = new MeasurementType(10);
        /** Y-coordinate of the end of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER_END_Y = new MeasurementType(11);
        /** X-coordinate of the leftmost pixel in the longest row of contiguous pixels in the particle. */
        public static final MeasurementType IMAQ_MT_MAX_HORIZ_SEGMENT_LENGTH_LEFT = new MeasurementType(12);
        /** X-coordinate of the rightmost pixel in the longest row of contiguous pixels in the particle. */
        public static final MeasurementType IMAQ_MT_MAX_HORIZ_SEGMENT_LENGTH_RIGHT = new MeasurementType(13);
        /** Y-coordinate of all of the pixels in the longest row of contiguous pixels in the particle. */
        public static final MeasurementType IMAQ_MT_MAX_HORIZ_SEGMENT_LENGTH_ROW = new MeasurementType(14);
        /** Distance between the x-coordinate of the leftmost particle point and the x-coordinate of the rightmost particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_WIDTH = new MeasurementType(16);
        /** Distance between the y-coordinate of highest particle point and the y-coordinate of the lowest particle point. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_HEIGHT = new MeasurementType(17);
        /** Distance between opposite corners of the bounding rectangle. */
        public static final MeasurementType IMAQ_MT_BOUNDING_RECT_DIAGONAL = new MeasurementType(18);
        /** Length of the outer boundary of the particle. */
        public static final MeasurementType IMAQ_MT_PERIMETER = new MeasurementType(19);
        /** Perimeter of the smallest convex polygon containing all points in the particle. */
        public static final MeasurementType IMAQ_MT_CONVEX_HULL_PERIMETER = new MeasurementType(20);
        /** Sum of the perimeters of each hole in the particle. */
        public static final MeasurementType IMAQ_MT_HOLES_PERIMETER = new MeasurementType(21);
        /** Distance between the start and end of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER = new MeasurementType(22);
        /** Length of the major axis of the ellipse with the same perimeter and area as the particle. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_ELLIPSE_MAJOR_AXIS = new MeasurementType(23);
        /** Length of the minor axis of the ellipse with the same perimeter and area as the particle. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_ELLIPSE_MINOR_AXIS = new MeasurementType(24);
        /** Length of the minor axis of the ellipse with the same area as the particle, and Major Axis equal in length to the Max Feret Diameter. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_ELLIPSE_MINOR_AXIS_FERET = new MeasurementType(25);
        /** Longest side of the rectangle with the same perimeter and area as the particle. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_RECT_LONG_SIDE = new MeasurementType(26);
        /** Shortest side of the rectangle with the same perimeter and area as the particle. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE = new MeasurementType(27);
        /** Distance between opposite corners of the rectangle with the same perimeter and area as the particle. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_RECT_DIAGONAL = new MeasurementType(28);
        /** Shortest side of the rectangle with the same area as the particle, and longest side equal in length to the Max Feret Diameter. */
        public static final MeasurementType IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE_FERET = new MeasurementType(29);
        /** Average length of a horizontal segment in the particle. */
        public static final MeasurementType IMAQ_MT_AVERAGE_HORIZ_SEGMENT_LENGTH = new MeasurementType(30);
        /** Average length of a vertical segment in the particle. */
        public static final MeasurementType IMAQ_MT_AVERAGE_VERT_SEGMENT_LENGTH = new MeasurementType(31);
        /** The particle area divided by the particle perimeter. */
        public static final MeasurementType IMAQ_MT_HYDRAULIC_RADIUS = new MeasurementType(32);
        /** Diameter of a disk with the same area as the particle. */
        public static final MeasurementType IMAQ_MT_WADDEL_DISK_DIAMETER = new MeasurementType(33);
        /** Area of the particle. */
        public static final MeasurementType IMAQ_MT_AREA = new MeasurementType(35);
        /** Sum of the areas of each hole in the particle. */
        public static final MeasurementType IMAQ_MT_HOLES_AREA = new MeasurementType(36);
        /** Area of a particle that completely covers the image. */
        public static final MeasurementType IMAQ_MT_PARTICLE_AND_HOLES_AREA = new MeasurementType(37);
        /** Area of the smallest convex polygon containing all points in the particle. */
        public static final MeasurementType IMAQ_MT_CONVEX_HULL_AREA = new MeasurementType(38);
        /** Area of the image. */
        public static final MeasurementType IMAQ_MT_IMAGE_AREA = new MeasurementType(39);
        /** Number of holes in the particle. */
        public static final MeasurementType IMAQ_MT_NUMBER_OF_HOLES = new MeasurementType(41);
        /** Number of horizontal segments in the particle. */
        public static final MeasurementType IMAQ_MT_NUMBER_OF_HORIZ_SEGMENTS = new MeasurementType(42);
        /** Number of vertical segments in the particle. */
        public static final MeasurementType IMAQ_MT_NUMBER_OF_VERT_SEGMENTS = new MeasurementType(43);
        /** The angle of the line that passes through the particle Center of Mass about which the particle has the lowest moment of inertia. */
        public static final MeasurementType IMAQ_MT_ORIENTATION = new MeasurementType(45);
        /** The angle of the line segment connecting the two perimeter points that are the furthest apart. */
        public static final MeasurementType IMAQ_MT_MAX_FERET_DIAMETER_ORIENTATION = new MeasurementType(46);
        /** Percentage of the particle Area covering the Image Area. */
        public static final MeasurementType IMAQ_MT_AREA_BY_IMAGE_AREA = new MeasurementType(48);
        /** Percentage of the particle Area in relation to its Particle and Holes Area. */
        public static final MeasurementType IMAQ_MT_AREA_BY_PARTICLE_AND_HOLES_AREA = new MeasurementType(49);
        /** Equivalent Ellipse Major Axis divided by Equivalent Ellipse Minor Axis. */
        public static final MeasurementType IMAQ_MT_RATIO_OF_EQUIVALENT_ELLIPSE_AXES = new MeasurementType(50);
        /** Equivalent Rect Long Side divided by Equivalent Rect Short Side. */
        public static final MeasurementType IMAQ_MT_RATIO_OF_EQUIVALENT_RECT_SIDES = new MeasurementType(51);
        /** Max Feret Diameter divided by Equivalent Rect Short Side (Feret). */
        public static final MeasurementType IMAQ_MT_ELONGATION_FACTOR = new MeasurementType(53);
        /** Area divided by the product of Bounding Rect Width and Bounding Rect Height. */
        public static final MeasurementType IMAQ_MT_COMPACTNESS_FACTOR = new MeasurementType(54);
        /** Perimeter divided by the circumference of a circle with the same area. */
        public static final MeasurementType IMAQ_MT_HEYWOOD_CIRCULARITY_FACTOR = new MeasurementType(55);
        /** Factor relating area to moment of inertia. */
        public static final MeasurementType IMAQ_MT_TYPE_FACTOR = new MeasurementType(56);
        /** The sum of all x-coordinates in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_X = new MeasurementType(58);
        /** The sum of all y-coordinates in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_Y = new MeasurementType(59);
        /** The sum of all x-coordinates squared in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_XX = new MeasurementType(60);
        /** The sum of all x-coordinates times y-coordinates in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_XY = new MeasurementType(61);
        /** The sum of all y-coordinates squared in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_YY = new MeasurementType(62);
        /** The sum of all x-coordinates cubed in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_XXX = new MeasurementType(63);
        /** The sum of all x-coordinates squared times y-coordinates in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_XXY = new MeasurementType(64);
        /** The sum of all x-coordinates times y-coordinates squared in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_XYY = new MeasurementType(65);
        /** The sum of all y-coordinates cubed in the particle. */
        public static final MeasurementType IMAQ_MT_SUM_YYY = new MeasurementType(66);
        /** The moment of inertia in the x-direction twice. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_XX = new MeasurementType(68);
        /** The moment of inertia in the x and y directions. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_XY = new MeasurementType(69);
        /** The moment of inertia in the y-direction twice. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_YY = new MeasurementType(70);
        /** The moment of inertia in the x-direction three times. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_XXX = new MeasurementType(71);
        /** The moment of inertia in the x-direction twice and the y-direction once. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_XXY = new MeasurementType(72);
        /** The moment of inertia in the x-direction once and the y-direction twice. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_XYY = new MeasurementType(73);
        /** The moment of inertia in the y-direction three times. */
        public static final MeasurementType IMAQ_MT_MOMENT_OF_INERTIA_YYY = new MeasurementType(74);
        /** The normalized moment of inertia in the x-direction twice. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_XX = new MeasurementType(75);
        /** The normalized moment of inertia in the x- and y-directions. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_XY = new MeasurementType(76);
        /** The normalized moment of inertia in the y-direction twice. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_YY = new MeasurementType(77);
        /** The normalized moment of inertia in the x-direction three times. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_XXX = new MeasurementType(78);
        /** The normalized moment of inertia in the x-direction twice and the y-direction once. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_XXY = new MeasurementType(79);
        /** The normalized moment of inertia in the x-direction once and the y-direction twice. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_XYY = new MeasurementType(80);
        /** The normalized moment of inertia in the y-direction three times. */
        public static final MeasurementType IMAQ_MT_NORM_MOMENT_OF_INERTIA_YYY = new MeasurementType(81);
        /** The first Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_1 = new MeasurementType(82);
        /** The second Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_2 = new MeasurementType(83);
        /** The third Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_3 = new MeasurementType(84);
        /** The fourth Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_4 = new MeasurementType(85);
        /** The fifth Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_5 = new MeasurementType(86);
        /** The sixth Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_6 = new MeasurementType(87);
        /** The seventh Hu moment. */
        public static final MeasurementType IMAQ_MT_HU_MOMENT_7 = new MeasurementType(88);

        public static final MeasurementType IMAQ_MEASUREMENT_TYPE_SIZE_GUARD = new MeasurementType(0xFFFFFFFF);

        private MeasurementType(int value) {
            this.value = value;
        }
    }

    public static class Range extends Structure {

        int lower;
        int upper;

        public void read() {
            lower = backingNativeMemory.getInt(0);
            upper = backingNativeMemory.getInt(4);
        }

        public void write() {
            backingNativeMemory.setInt(0, lower);
            backingNativeMemory.setInt(4, upper);
        }

        public int size() {
            return 8;
        }

        /**
         * Free the memory used by this range
         */
        public void free() {
            release();
        }

        /**
         * Create a new range with the specified upper and lower boundaries
         * @param lower The lower limit
         * @param upper The upper limit
         */
        public Range(final int lower, final int upper) {
            allocateMemory();
            set(lower, upper);
        }

        /**
         * Set the upper and lower boundaries
         * @param lower The lower limit
         * @param upper The upper limit
         */
        public void set(final int lower, final int upper) {
            if (lower > upper) {
                throw new BoundaryException("Lower boundary is greater than upper");
            }
            this.lower = lower;
            this.upper = upper;
            write();
        }

        /**
         * Get the lower boundary
         * @return The lower boundary.
         */
        public int getLower() {
            read();
            return lower;
        }

        /**
         * Get the upper boundary
         * @return The upper boundary.
         */
        public int getUpper() {
            read();
            return upper;
        }
    }

    public static class Rect extends Structure {

        int top;
        int left;
        int height;
        int width;

        public void read() {
            top = backingNativeMemory.getInt(0);
            left = backingNativeMemory.getInt(4);
            height = backingNativeMemory.getInt(8);
            width = backingNativeMemory.getInt(12);
        }

        public void write() {
            backingNativeMemory.setInt(0, top);
            backingNativeMemory.setInt(4, left);
            backingNativeMemory.setInt(8, height);
            backingNativeMemory.setInt(12, width);
        }

        public int size() {
            return 16;
        }

        /**
         * Free the memory used by this range
         */
        public void free() {
            release();
        }

        /**
         * Create a new Rect with the specified dimensions
         * @param top  The top of the rectangle
         * @param left The left edge of the rectangle
         * @param height The height of the rectangle
         * @param width The width of the rectangle
         */
        public Rect(final int top, final int left, final int height, final int width) {
            allocateMemory();
            set(top, left, height, width);
        }

        /**
         * Set the Rect dimensions
         * @param top  The top of the rectangle
         * @param left The left edge of the rectangle
         * @param height The height of the rectangle
         * @param width The width of the rectangle
         */
        public void set(final int top, final int left, final int height, final int width) {
            this.top = top;
            this.left = left;
            this.height = height;
            this.width = width;
            write();
        }

        /**
         * Get the top of the rectangle
         * @return The top edge of the rectangle in pixels, measured from the top of the image
         */
        public int getTop() {
            read();
            return top;
        }

        /**
         * Get the left edge of the rectangle
         * @return The left edge of the rectangle in pixels measured from the left of the image
         */
        public int getLeft() {
            read();
            return left;
        }

        /**
         * Get the width of the rectangle
         * @return The width of the rectangle in pixels
         */
        public int getWidth() {
            read();
            return width;
        }

        /**
         * Get the height of the rectangle
         * @return The height edge of the rectangle in pixels
         */
        public int getHeight() {
            read();
            return height;
        }

    }

    //============================================================================
    //  Value Type handling
    // Parameters of "type" PixelValue, RGBValue, HSLValue, etc, are passed as an encoded int from Java code.
    //============================================================================
    public int encodeRGBValue(int r, int g, int b, int a) {
        return (a & 0xff << 24) | (r & 0xff << 16) | (g & 0xff << 8) | (b & 0xff);
    }

    public int rgbGetR(int rgbvalue) {
        return (rgbvalue >>> 16) & 0xFF;
    }

    public int rgbGetG(int rgbvalue) {
        return (rgbvalue >>> 8) & 0xFF;
    }

    public int rgbGetB(int rgbvalue) {
        return (rgbvalue >>> 0) & 0xFF;
    }

    public int rgbGetA(int rgbvalue) {
        return (rgbvalue >>> 24) & 0xFF;
    }

    public int encodeHSLValue(int h, int s, int l, int a) {
        return (a & 0xff << 24) | (h & 0xff << 16) | (s & 0xff << 8) | (l & 0xff);
    }

    public int hslGetH(int hslvalue) {
        return (hslvalue >>> 16) & 0xFF;
    }

    public int hslGetS(int hslvalue) {
        return (hslvalue >>> 8) & 0xFF;
    }

    public int hslGetL(int hslvalue) {
        return (hslvalue >>> 0) & 0xFF;
    }

    public int hslGetA(int hslvalue) {
        return (hslvalue >>> 24) & 0xFF;
    }

    public int encodeHSVValue(int h, int s, int v, int a) {
        return (a & 0xff << 24) | (h & 0xff << 16) | (s & 0xff << 8) | (v & 0xff);
    }

    public int hsvGetH(int hsvvalue) {
        return (hsvvalue >>> 16) & 0xFF;
    }

    public int hsvGetS(int hsvvalue) {
        return (hsvvalue >>> 8) & 0xFF;
    }

    public int hsvGetV(int hsvvalue) {
        return (hsvvalue >>> 0) & 0xFF;
    }

    public int hsvGetA(int hsvvalue) {
        return (hsvvalue >>> 24) & 0xFF;
    }

    public int encodeGreyscaleValue(float greyscale) {
        return Float.floatToIntBits(greyscale);
    }

    public float decodeGreyscaleValue(int pixelValue) {
        return Float.intBitsToFloat(pixelValue);
    }

    //============================================================================
    //  Acquisition functions
    //============================================================================
    //IMAQ_FUNC Image* IMAQ_STDCALL imaqCopyFromRing(SESSION_ID sessionID, Image* image, int imageToCopy, int* imageNumber, Rect rect);
    //IMAQ_FUNC Image* IMAQ_STDCALL imaqEasyAcquire(const char* interfaceName);
    //IMAQ_FUNC Image* IMAQ_STDCALL imaqExtractFromRing(SESSION_ID sessionID, int imageToExtract, int* imageNumber);
    //IMAQ_FUNC Image* IMAQ_STDCALL imaqGrab(SESSION_ID sessionID, Image* image, int immediate);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqReleaseImage(SESSION_ID sessionID);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetupGrab(SESSION_ID sessionID, Rect rect);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetupRing(SESSION_ID sessionID, Image** images, int numImages, int skipCount, Rect rect);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetupSequence(SESSION_ID sessionID, Image** images, int numImages, int skipCount, Rect rect);
    //IMAQ_FUNC Image* IMAQ_STDCALL imaqSnap(SESSION_ID sessionID, Image* image, Rect rect);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqStartAcquisition(SESSION_ID sessionID);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqStopAcquisition(SESSION_ID sessionID);

    //============================================================================
    //  Arithmetic functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqAbsoluteDifference(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAbsoluteDifferenceConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAdd(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAddConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAverage(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAverageConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqDivide2(Image* dest, const Image* sourceA, const Image* sourceB, RoundingMode roundingMode);
    //IMAQ_FUNC int IMAQ_STDCALL imaqDivideConstant2(Image* dest, const Image* source, PixelValue value, RoundingMode roundingMode);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMax(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMaxConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMin(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMinConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqModulo(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqModuloConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMulDiv(Image* dest, const Image* sourceA, const Image* sourceB, float value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMultiply(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMultiplyConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqSubtract(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqSubtractConstant(Image* dest, const Image* source, PixelValue value);

    //============================================================================
    //  Spatial Filters functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqCannyEdgeFilter(Image* dest, const Image* source, const CannyOptions* options);
    //IMAQ_FUNC int IMAQ_STDCALL imaqConvolve2(Image* dest, Image* source, float* kernel, int matrixRows, int matrixCols, float normalize, Image* mask, RoundingMode roundingMode);
    //IMAQ_FUNC int IMAQ_STDCALL imaqCorrelate(Image* dest, Image* source, const Image* templateImage, Rect rect);
    //IMAQ_FUNC int IMAQ_STDCALL imaqEdgeFilter(Image* dest, Image* source, OutlineMethod method, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqLowPass(Image* dest, Image* source, int width, int height, float tolerance, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMedianFilter(Image* dest, Image* source, int width, int height, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqNthOrderFilter(Image* dest, Image* source, int width, int height, int n, const Image* mask);

    //============================================================================
    //  Drawing functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqDrawLineOnImage(Image* dest, const Image* source, DrawMode mode, Point start, Point end, float newPixelValue);
    //IMAQ_FUNC int IMAQ_STDCALL imaqDrawShapeOnImage(Image* dest, const Image* source, Rect rect, DrawMode mode, ShapeMode shape, float newPixelValue);
    //IMAQ_FUNC int IMAQ_STDCALL imaqDrawTextOnImage(Image* dest, const Image* source, Point coord, const char* text, const DrawTextOptions* options, int* fontNameUsed);

    //============================================================================
    //  Interlacing functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqInterlaceCombine(Image* frame, const Image* odd, const Image* even);
    //IMAQ_FUNC int IMAQ_STDCALL imaqInterlaceSeparate(const Image* frame, Image* odd, Image* even);

    //============================================================================
    //  Image Information functions
    //============================================================================
    //IMAQ_FUNC char** IMAQ_STDCALL imaqEnumerateCustomKeys(const Image* image, unsigned int* size);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetBitDepth(const Image* image, unsigned int* bitDepth);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetBytesPerPixel(const Image* image, int* byteCount);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetImageInfo(const Image* image, ImageInfo* info);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetImageType(const Image* image, ImageType* type);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetMaskOffset(const Image* image, Point* offset);
    //IMAQ_FUNC void*  IMAQ_STDCALL imaqGetPixelAddress(const Image* image, Point pixel);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetVisionInfoTypes(const Image* image, unsigned int* present);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqIsImageEmpty(const Image* image, int* empty);
    //IMAQ_FUNC void*  IMAQ_STDCALL imaqReadCustomData(const Image* image, const char* key, unsigned int* size);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqRemoveCustomData(Image* image, const char* key);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqRemoveVisionInfo2(const Image* image, unsigned int info);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetBitDepth(Image* image, unsigned int bitDepth);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetImageSize(Image* image, int width, int height);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetMaskOffset(Image* image, Point offset);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqWriteCustomData(Image* image, const char* key, const void* data, unsigned int size);

    // TODO: Make non-blocking...
    private static final BlockingFunction imaqGetImageSizeFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqGetImageSize");
    static {
        imaqGetImageSizeFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Get the height of an image.
     * @param image The image to get the height of.
     * @return The height of the image.
     */
    public static int getHeight(Pointer image)  throws NIVisionException {
        Pointer i = new Pointer(4);
        int val;
        try {
            assertCleanStatus(imaqGetImageSizeFn.call3(image, 0, i));
        } finally {
            val = i.getInt(0);
            i.free();
        }
        return val;
    }

    /**
     * Get the width of an image.
     * @param image The image to get the width of.
     * @return The width of the image.
     */
    public static int getWidth(Pointer image)  throws NIVisionException {
        Pointer i = new Pointer(4);
        int val;
        try {
            assertCleanStatus(imaqGetImageSizeFn.call3(image, i, 0));
        } finally {
            val = i.getInt(0);
            i.free();
        }
        return val;
    }

    //============================================================================
    //  Morphology functions
    //============================================================================
    //IMAQ_FUNC int           IMAQ_STDCALL imaqConvexHull(Image* dest, Image* source, int connectivity8);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqDanielssonDistance(Image* dest, Image* source);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqFillHoles(Image* dest, const Image* source, int connectivity8);
    //IMAQ_FUNC CircleReport* IMAQ_STDCALL imaqFindCircles(Image* dest, Image* source, float minRadius, float maxRadius, int* numCircles);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqLabel2(Image* dest, Image* source, int connectivity8, int* particleCount);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqRejectBorder(Image* dest, Image* source, int connectivity8);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSegmentation(Image* dest, Image* source);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSeparation(Image* dest, Image* source, int erosions, const StructuringElement* structuringElement);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSimpleDistance(Image* dest, Image* source, const StructuringElement* structuringElement);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSizeFilter(Image* dest, Image* source, int connectivity8, int erosions, SizeType keepSize, const StructuringElement* structuringElement);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSkeleton(Image* dest, Image* source, SkeletonMethod method);


    /**
     * Convex hull operation
     */
    private static final BlockingFunction imaqConvexHullFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqConvexHull");
    static {
        imaqConvexHullFn.setTaskExecutor(taskExecutor);
    }

    public static void convexHull(Pointer dest, Pointer source, int connectivity8) throws NIVisionException {
        assertCleanStatus(imaqConvexHullFn.call3(dest.address().toUWord().toPrimitive(),
                          source.address().toUWord().toPrimitive(),
                          connectivity8));
    }

    /**
     * size filter function
     */
    private static final BlockingFunction imaqSizeFilterFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqSizeFilter");
    static {
        imaqSizeFilterFn.setTaskExecutor(taskExecutor);
    }

    public static void sizeFilter(Pointer dest, Pointer source, boolean connectivity8, int erosions, boolean keepLarger) throws NIVisionException {
        assertCleanStatus(imaqSizeFilterFn.call6(dest.address().toUWord().toPrimitive(),
                          source.address().toUWord().toPrimitive(),
                          connectivity8 ? 1 : 0, erosions, keepLarger ? 0 : 1, 0));
    }

    //============================================================================
    //  Logical functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqAnd(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqAndConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqCompare(Image* dest, const Image* source, const Image* compareImage, ComparisonFunction compare);
    //IMAQ_FUNC int IMAQ_STDCALL imaqCompareConstant(Image* dest, const Image* source, PixelValue value, ComparisonFunction compare);
    //IMAQ_FUNC int IMAQ_STDCALL imaqLogicalDifference(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqLogicalDifferenceConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqNand(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqNandConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqNor(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqNorConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOr(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOrConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqXnor(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqXnorConstant(Image* dest, const Image* source, PixelValue value);
    //IMAQ_FUNC int IMAQ_STDCALL imaqXor(Image* dest, const Image* sourceA, const Image* sourceB);
    //IMAQ_FUNC int IMAQ_STDCALL imaqXorConstant(Image* dest, const Image* source, PixelValue value);

    //============================================================================
    //  Particle Analysis functions
    //============================================================================

    private static final BlockingFunction imaqCountParticlesFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqCountParticles");
    static {
        imaqCountParticlesFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Counts the number of particles in a binary image.
     * @param image The image to count the particles in
     * @return The number of particles
     */
    public static int countParticles(Pointer image)  throws NIVisionException {
        IntByReference i = new IntByReference(0);
        int val;
        try {
            assertCleanStatus(imaqCountParticlesFn.call3(image, 1, i.getPointer()));
        } finally {
            val = i.getValue();
            i.free();
        }
        return val;
    }

    private static final BlockingFunction imaqMeasureParticleFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqMeasureParticle");
    static {
        imaqMeasureParticleFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Returns a measurement associated with a particle
     * @param image The image containing the particle to get information about.
     * @param particleNum The number of the particle to get information about.
     * @param calibrated Specifies whether to return the measurement as a real-world value.
     * @param type The measurement to make on the particle.
     * @return The value of the requested measurement.
     */
    public static double MeasureParticle (Pointer image, int particleNum, boolean calibrated, MeasurementType type)  throws NIVisionException {
        Pointer l = new Pointer(8);
        double val;
        try {
            assertCleanStatus(imaqMeasureParticleFn.call5(image, particleNum, calibrated?1:0, type.value, l));
        } finally {
            val = l.getDouble(0);
            l.free();
        }
        return val;
    }

    //IMAQ_FUNC int IMAQ_STDCALL imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles);

    private static final BlockingFunction imaqParticleFilter4Fn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqParticleFilter4");

    public static int particleFilter(Pointer dest, Pointer source, CriteriaCollection collection) throws NIVisionException {
        Pointer particleFilterOptions = new Pointer(16);
        particleFilterOptions.setInt(0, 0);
        particleFilterOptions.setInt(4, 0);
        particleFilterOptions.setInt(8, 0);
        particleFilterOptions.setInt(12, 1);
        IntByReference i = new IntByReference(0);
        Pointer criteriaArray = collection.getCriteriaArray();
        assertCleanStatus(imaqParticleFilter4Fn.call7(dest.address().toUWord().toPrimitive(),   // dest image
                          source.address().toUWord().toPrimitive(),   // source image
                          criteriaArray.address().toUWord().toPrimitive(),  // array of criteria
                          collection.getNumberOfCriteria(),           // number of criteria in the array
                          particleFilterOptions.address().toUWord().toPrimitive(),  // particle filter options
                          0,                                          // Region of interest
                          i.getPointer().address().toUWord().toPrimitive()));  // returned number of particles
        int numberOfParticles = i.getValue();
        i.free();
        criteriaArray.free();
        particleFilterOptions.free();
        return numberOfParticles;
    }

    //============================================================================
    //  Analytic Geometry functions
    //============================================================================
    //IMAQ_FUNC int           IMAQ_STDCALL imaqBuildCoordinateSystem(const Point* points, ReferenceMode mode, AxisOrientation orientation, CoordinateSystem* system);
    //IMAQ_FUNC BestCircle2*  IMAQ_STDCALL imaqFitCircle2(const PointFloat* points, int numPoints, const FitCircleOptions* options);
    //IMAQ_FUNC BestEllipse2* IMAQ_STDCALL imaqFitEllipse2(const PointFloat* points, int numPoints, const FitEllipseOptions* options);
    //IMAQ_FUNC BestLine*     IMAQ_STDCALL imaqFitLine(const PointFloat* points, int numPoints, const FitLineOptions* options);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetAngle(PointFloat start1, PointFloat end1, PointFloat start2, PointFloat end2, float* angle);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetBisectingLine(PointFloat start1, PointFloat end1, PointFloat start2, PointFloat end2, PointFloat* bisectStart, PointFloat* bisectEnd);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetDistance(PointFloat point1, PointFloat point2, float* distance);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetIntersection(PointFloat start1, PointFloat end1, PointFloat start2, PointFloat end2, PointFloat* intersection);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetMidLine(PointFloat refLineStart, PointFloat refLineEnd, PointFloat point, PointFloat* midLineStart, PointFloat* midLineEnd);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetPerpendicularLine(PointFloat refLineStart, PointFloat refLineEnd, PointFloat point, PointFloat* perpLineStart, PointFloat* perpLineEnd, double* distance);
    //IMAQ_FUNC SegmentInfo*  IMAQ_STDCALL imaqGetPointsOnContour(const Image* image, int* numSegments);
    //IMAQ_FUNC Point*        IMAQ_STDCALL imaqGetPointsOnLine(Point start, Point end, int* numPoints);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetPolygonArea(const PointFloat* points, int numPoints, float* area);
    //IMAQ_FUNC float*        IMAQ_STDCALL imaqInterpolatePoints(const Image* image, const Point* points, int numPoints, InterpolationMethod method, int subpixel, int* interpCount);

    //============================================================================
    //  Border functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqFillBorder(Image* image, BorderMethod method);
    //IMAQ_FUNC int IMAQ_STDCALL imaqGetBorderSize(const Image* image, int* borderSize);
    //IMAQ_FUNC int IMAQ_STDCALL imaqSetBorderSize(Image* image, int size);

    //============================================================================
    //  Image Management functions
    //============================================================================
    protected static final BlockingFunction imaqCreateImageFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqCreateImage");
    static {
        imaqCreateImageFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Allocates space for and creates a new imaq image
     * @param type the imaq encoding to use for the image
     * @param borderSize the size of border to use for the image
     * @return a newly allocated pointer to an imaqImage
     */
    public static Pointer imaqCreateImage(ImageType type, int borderSize) throws NIVisionException {
        int toReturn = imaqCreateImageFn.call2(type.value, borderSize);
        assertCleanStatus(toReturn);
        return new Pointer(toReturn, 0);
    }
    //IMAQ_FUNC int    IMAQ_STDCALL imaqArrayToImage(Image* image, const void* array, int numCols, int numRows);
    //IMAQ_FUNC void*  IMAQ_STDCALL imaqImageToArray(const Image* image, Rect rect, int* columns, int* rows);

    //============================================================================
    //  Color Processing functions
    //============================================================================

    private static final BlockingFunction imaqColorThresholdFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqColorThreshold");
    static {
        imaqColorThresholdFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Convert the given image into a binary image true where the colors match the given thresholds
     * @param dest the 8 bit monocolor image to store the result in
     * @param source the color image source
     * @param mode The mode of color to use
     * @param plane1Range First color range
     * @param plane2Range Second color range
     * @param plane3Range Third color range
     */
    public static void colorThreshold(Pointer dest, Pointer source, ColorMode mode,
                                      Pointer plane1Range, Pointer plane2Range, Pointer plane3Range)  throws NIVisionException {
        int replaceValue = 1;
        assertCleanStatus(imaqColorThresholdFn.call7(dest.address().toUWord().toPrimitive(),
                          source.address().toUWord().toPrimitive(),
                          replaceValue, mode.value,
                          plane1Range.address().toUWord().toPrimitive(),
                          plane2Range.address().toUWord().toPrimitive(),
                          plane3Range.address().toUWord().toPrimitive()));
    }

    private static final BlockingFunction imaqColorEqualizeFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqColorEqualize");
    static {
        imaqColorEqualizeFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Calculates the histogram of each plane of a color image and redistributes
     * pixel values across the desired range while maintaining pixel value
     * groupings.
     * @param destination The destination image.
     * @param source The image to equalize.
     * @param all Set this parameter to TRUE to equalize all three planes of the
     * image. Set this parameter to FALSE to equalize only the luminance plane.
     */
    public static void colorEqualize(Pointer destination, Pointer source, boolean all)  throws NIVisionException {
        assertCleanStatus(imaqColorEqualizeFn.call3(destination, source,all?1:0));
    }

    //IMAQ_FUNC Color2                IMAQ_STDCALL imaqChangeColorSpace2(const Color2* sourceColor, ColorMode sourceSpace, ColorMode destSpace, double offset, const CIEXYZValue* whiteReference);
    //IMAQ_FUNC int                   IMAQ_STDCALL imaqColorBCGTransform(Image* dest, const Image* source, const BCGOptions* redOptions, const BCGOptions* greenOptions, const BCGOptions* blueOptions, const Image* mask);
    //IMAQ_FUNC ColorHistogramReport* IMAQ_STDCALL imaqColorHistogram2(Image* image, int numClasses, ColorMode mode, const CIEXYZValue* whiteReference, Image* mask);
    //IMAQ_FUNC int                   IMAQ_STDCALL imaqColorLookup(Image* dest, const Image* source, ColorMode mode, const Image* mask, const short* plane1, const short* plane2, const short* plane3);

    //============================================================================
    //  Transform functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqBCGTransform(Image* dest, const Image* source, const BCGOptions* options, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqEqualize(Image* dest, const Image* source, float min, float max, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqInverse(Image* dest, const Image* source, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqLookup(Image* dest, const Image* source, const short* table, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMathTransform(Image* dest, const Image* source, MathTransformMethod method, float rangeMin, float rangeMax, float power, const Image* mask);
    //IMAQ_FUNC int IMAQ_STDCALL imaqWatershedTransform(Image* dest, const Image* source, int connectivity8, int* zoneCount);


    //============================================================================
    //  Utilities functions
    //============================================================================
    //IMAQ_FUNC const float* IMAQ_STDCALL imaqGetKernel(KernelFamily family, int size, int number);
    //IMAQ_FUNC Annulus      IMAQ_STDCALL imaqMakeAnnulus(Point center, int innerRadius, int outerRadius, double startAngle, double endAngle);
    //IMAQ_FUNC Point        IMAQ_STDCALL imaqMakePoint(int xCoordinate, int yCoordinate);
    //IMAQ_FUNC PointFloat   IMAQ_STDCALL imaqMakePointFloat(float xCoordinate, float yCoordinate);
    //IMAQ_FUNC Rect         IMAQ_STDCALL imaqMakeRect(int top, int left, int height, int width);
    //IMAQ_FUNC Rect         IMAQ_STDCALL imaqMakeRectFromRotatedRect(RotatedRect rotatedRect);
    //IMAQ_FUNC RotatedRect  IMAQ_STDCALL imaqMakeRotatedRect(int top, int left, int height, int width, double angle);
    //IMAQ_FUNC RotatedRect  IMAQ_STDCALL imaqMakeRotatedRectFromRect(Rect rect);
    //IMAQ_FUNC int          IMAQ_STDCALL imaqMulticoreOptions(MulticoreOperation operation, unsigned int* customNumCores);

    //============================================================================
    //  Image Manipulation functions
    //============================================================================
    //IMAQ_FUNC int   IMAQ_STDCALL imaqCast(Image* dest, const Image* source, ImageType type, const float* lookup, int shift);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqCopyRect(Image* dest, const Image* source, Rect rect, Point destLoc);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqDuplicate(Image* dest, const Image* source);
    //IMAQ_FUNC void* IMAQ_STDCALL imaqFlatten(const Image* image, FlattenType type, CompressionType compression, int quality, unsigned int* size);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqFlip(Image* dest, const Image* source, FlipAxis axis);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqMask(Image* dest, const Image* source, const Image* mask);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqResample(Image* dest, const Image* source, int newWidth, int newHeight, InterpolationMethod method, Rect rect);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqRotate2(Image* dest, const Image* source, float angle, PixelValue fill, InterpolationMethod method, int maintainSize);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqScale(Image* dest, const Image* source, int xScale, int yScale, ScalingMode scaleMode, Rect rect);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqShift(Image* dest, const Image* source, int shiftX, int shiftY, PixelValue fill);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqTranspose(Image* dest, const Image* source);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqUnflatten(Image* image, const void* data, unsigned int size);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqUnwrapImage(Image* dest, const Image* source, Annulus annulus, RectOrientation orientation, InterpolationMethod method);
    //IMAQ_FUNC int   IMAQ_STDCALL imaqView3D(Image* dest, Image* source, const View3DOptions* options);

    //============================================================================
    //  Pattern Matching functions
    //============================================================================

    private static final BlockingFunction imaqDetectEllipsesFn =
        NativeLibrary.getDefaultInstance().getBlockingFunction("imaqDetectEllipses");
    static {
        imaqDetectEllipsesFn.setTaskExecutor(NIVision.taskExecutor);
    }
    private static Pointer numberOfEllipsesDetected = new Pointer(4);


    public static EllipseMatch[] detectEllipses(MonoImage image, EllipseDescriptor ellipseDescriptor,
            CurveOptions curveOptions, ShapeDetectionOptions shapeDetectionOptions,
            RegionOfInterest roi) throws NIVisionException {

        int curveOptionsPointer = 0;
        if (curveOptions != null)
            curveOptionsPointer = curveOptions.getPointer().address().toUWord().toPrimitive();
        int shapeDetectionOptionsPointer = 0;
        if (shapeDetectionOptions != null)
            shapeDetectionOptionsPointer = shapeDetectionOptions.getPointer().address().toUWord().toPrimitive();
        int roiPointer = 0;
        if (roi != null)
            roiPointer = roi.getPointer().address().toUWord().toPrimitive();

        int returnedAddress =
            imaqDetectEllipsesFn.call6(
                image.image.address().toUWord().toPrimitive(),
                ellipseDescriptor.getPointer().address().toUWord().toPrimitive(),
                curveOptionsPointer, shapeDetectionOptionsPointer,
                roiPointer,
                numberOfEllipsesDetected.address().toUWord().toPrimitive());

        try {
            NIVision.assertCleanStatus(returnedAddress);
        } catch (NIVisionException ex) {
            if (!ex.getMessage().equals("No error."))
                throw ex;
        }

        EllipseMatch[] matches = EllipseMatch.getMatchesFromMemory(returnedAddress, numberOfEllipsesDetected.getInt(0));
        NIVision.dispose(new Pointer(returnedAddress,0));
        return matches;
    }
    //IMAQ_FUNC CircleMatch*              IMAQ_STDCALL imaqDetectCircles(const Image* image, const CircleDescriptor* circleDescriptor, const CurveOptions* curveOptions, const ShapeDetectionOptions* shapeDetectionOptions, const ROI* roi, int* numMatchesReturned);
    //IMAQ_FUNC LineMatch*                IMAQ_STDCALL imaqDetectLines(const Image* image, const LineDescriptor* lineDescriptor, const CurveOptions* curveOptions, const ShapeDetectionOptions* shapeDetectionOptions, const ROI* roi, int* numMatchesReturned);
    //IMAQ_FUNC RectangleMatch*           IMAQ_STDCALL imaqDetectRectangles(const Image* image, const RectangleDescriptor* rectangleDescriptor, const CurveOptions* curveOptions, const ShapeDetectionOptions* shapeDetectionOptions, const ROI* roi, int* numMatchesReturned);
    //IMAQ_FUNC FeatureData*              IMAQ_STDCALL imaqGetGeometricFeaturesFromCurves(const Curve* curves, unsigned int numCurves, const FeatureType* featureTypes, unsigned int numFeatureTypes, unsigned int* numFeatures);
    //IMAQ_FUNC FeatureData*              IMAQ_STDCALL imaqGetGeometricTemplateFeatureInfo(const Image* pattern, unsigned int* numFeatures);
    //IMAQ_FUNC int                       IMAQ_STDCALL imaqLearnColorPattern(Image* image, const LearnColorPatternOptions* options);
    //IMAQ_FUNC int                       IMAQ_STDCALL imaqLearnGeometricPattern(Image* image, PointFloat originOffset, const CurveOptions* curveOptions, const LearnGeometricPatternAdvancedOptions* advancedLearnOptions, const Image* mask);
    //IMAQ_FUNC MultipleGeometricPattern* IMAQ_STDCALL imaqLearnMultipleGeometricPatterns(const Image** patterns, unsigned int numberOfPatterns, const String255* labels);
    //IMAQ_FUNC int                       IMAQ_STDCALL imaqLearnPattern3(Image* image, LearningMode learningMode, LearnPatternAdvancedOptions* advancedOptions, const Image* mask);
    //IMAQ_FUNC PatternMatch*             IMAQ_STDCALL imaqMatchColorPattern(const Image* image, Image* pattern, const MatchColorPatternOptions* options, Rect searchRect, int* numMatches);
    //IMAQ_FUNC GeometricPatternMatch2*   IMAQ_STDCALL imaqMatchGeometricPattern2(const Image* image, const Image* pattern, const CurveOptions* curveOptions, const MatchGeometricPatternOptions* matchOptions, const MatchGeometricPatternAdvancedOptions2* advancedMatchOptions, const ROI* roi, int* numMatches);
    //IMAQ_FUNC GeometricPatternMatch2*   IMAQ_STDCALL imaqMatchMultipleGeometricPatterns(const Image* image, const MultipleGeometricPattern* multiplePattern, const ROI* roi, int* numMatches);
    //IMAQ_FUNC PatternMatch*             IMAQ_STDCALL imaqMatchPattern2(const Image* image, const Image* pattern, const MatchPatternOptions* options, const MatchPatternOptions* advancedOptions, Rect searchRect, int* numMatches);
    //IMAQ_FUNC MultipleGeometricPattern* IMAQ_STDCALL imaqReadMultipleGeometricPatternFile(const char* fileName, String255 description);
    //IMAQ_FUNC PatternMatch*             IMAQ_STDCALL imaqRefineMatches(const Image* image, const Image* pattern, const PatternMatch* candidatesIn, int numCandidatesIn, MatchPatternOptions* options, MatchPatternAdvancedOptions* advancedOptions, int* numCandidatesOut);
    //IMAQ_FUNC int                       IMAQ_STDCALL imaqSetMultipleGeometricPatternsOptions(MultipleGeometricPattern* multiplePattern, const char* label, const CurveOptions* curveOptions, const MatchGeometricPatternOptions* matchOptions, const MatchGeometricPatternAdvancedOptions2* advancedMatchOptions);
    //IMAQ_FUNC int                       IMAQ_STDCALL imaqWriteMultipleGeometricPatternFile(const MultipleGeometricPattern* multiplePattern, const char* fileName, const char* description);

    //============================================================================
    //  Calibration functions
    //============================================================================
    //IMAQ_FUNC int              IMAQ_STDCALL imaqCopyCalibrationInfo2(Image* dest, Image* source, Point offset);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqCorrectCalibratedImage(Image* dest, const Image* source, PixelValue fill, InterpolationMethod method, const ROI* roi);
    //IMAQ_FUNC CalibrationInfo* IMAQ_STDCALL imaqGetCalibrationInfo2(const Image* image);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqLearnCalibrationGrid(Image* image, const ROI* roi, const LearnCalibrationOptions* options, const GridDescriptor* grid, const CoordinateSystem* system, const RangeFloat* range, float* quality);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqLearnCalibrationPoints(Image* image, const CalibrationPoints* points, const ROI* roi, const LearnCalibrationOptions* options, const GridDescriptor* grid, const CoordinateSystem* system, float* quality);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqSetCoordinateSystem(Image* image, const CoordinateSystem* system);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqSetSimpleCalibration(Image* image, ScalingMethod method, int learnTable, const GridDescriptor* grid, const CoordinateSystem* system);
    //IMAQ_FUNC TransformReport* IMAQ_STDCALL imaqTransformPixelToRealWorld(const Image* image, const PointFloat* pixelCoordinates, int numCoordinates);
    //IMAQ_FUNC TransformReport* IMAQ_STDCALL imaqTransformRealWorldToPixel(const Image* image, const PointFloat* realWorldCoordinates, int numCoordinates);

    //============================================================================
    //  Overlay functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqClearOverlay(Image* image, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqCopyOverlay(Image* dest, const Image* source, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqGetOverlayProperties(Image* image, const char* group, TransformBehaviors* transformBehaviors);
    //IMAQ_FUNC int IMAQ_STDCALL imaqMergeOverlay(Image* dest, const Image* source, const RGBValue* palette, unsigned int numColors, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayArc(Image* image, const ArcInfo* arc, const RGBValue* color, DrawMode drawMode, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayBitmap(Image* image, Point destLoc, const RGBValue* bitmap, unsigned int numCols, unsigned int numRows, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayClosedContour(Image* image, const Point* points, int numPoints, const RGBValue* color, DrawMode drawMode, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayLine(Image* image, Point start, Point end, const RGBValue* color, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayMetafile(Image* image, const void* metafile, Rect rect, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayOpenContour(Image* image, const Point* points, int numPoints, const RGBValue* color, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayOval(Image* image, Rect boundingBox, const RGBValue* color, DrawMode drawMode, char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayPoints(Image* image, const Point* points, int numPoints, const RGBValue* colors, int numColors, PointSymbol symbol, const UserPointSymbol* userSymbol, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayRect(Image* image, Rect rect, const RGBValue* color, DrawMode drawMode, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayROI(Image* image, const ROI* roi, PointSymbol symbol, const UserPointSymbol* userSymbol, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqOverlayText(Image* image, Point origin, const char* text, const RGBValue* color, const OverlayTextOptions* options, const char* group);
    //IMAQ_FUNC int IMAQ_STDCALL imaqSetOverlayProperties(Image* image, const char* group, TransformBehaviors* transformBehaviors);

    //============================================================================
    //  Color Matching functions
    //============================================================================
    //IMAQ_FUNC ColorInformation* IMAQ_STDCALL imaqLearnColor(const Image* image, const ROI* roi, ColorSensitivity sensitivity, int saturation);
    //IMAQ_FUNC int*              IMAQ_STDCALL imaqMatchColor(const Image* image, const ColorInformation* info, const ROI* roi, int* numScores);

    //============================================================================
    //  Meter functions
    //============================================================================
    //IMAQ_FUNC MeterArc* IMAQ_STDCALL imaqGetMeterArc(int lightNeedle, MeterArcMode mode, const ROI* roi, PointFloat base, PointFloat start, PointFloat end);
    //IMAQ_FUNC int       IMAQ_STDCALL imaqReadMeter(const Image* image, const MeterArc* arcInfo, double* percentage, PointFloat* endOfNeedle);

    //============================================================================
    //  Barcode I/O functions
    //============================================================================
    //IMAQ_FUNC int               IMAQ_STDCALL imaqGradeDataMatrixBarcodeAIM(const Image* image, AIMGradeReport* report);
    //IMAQ_FUNC BarcodeInfo*      IMAQ_STDCALL imaqReadBarcode(const Image* image, BarcodeType type, const ROI* roi, int validate);
    //IMAQ_FUNC DataMatrixReport* IMAQ_STDCALL imaqReadDataMatrixBarcode2(Image* image, const ROI* roi, DataMatrixGradingMode prepareForGrading, const DataMatrixDescriptionOptions* descriptionOptions, const DataMatrixSizeOptions* sizeOptions, const DataMatrixSearchOptions* searchOptions);
    //IMAQ_FUNC Barcode2DInfo*    IMAQ_STDCALL imaqReadPDF417Barcode(const Image* image, const ROI* roi, Barcode2DSearchMode searchMode, unsigned int* numBarcodes);
    //IMAQ_FUNC QRCodeReport*     IMAQ_STDCALL imaqReadQRCode(Image* image, const ROI* roi, QRGradingMode reserved, const QRCodeDescriptionOptions* descriptionOptions, const QRCodeSizeOptions* sizeOptions, const QRCodeSearchOptions* searchOptions);

    //============================================================================
    //  Shape Matching functions
    //============================================================================
    //IMAQ_FUNC ShapeReport* IMAQ_STDCALL imaqMatchShape(Image* dest, Image* source, const Image* templateImage, int scaleInvariant, int connectivity8, double tolerance, int* numMatches);

    //============================================================================
    //  Contours functions
    //============================================================================
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddAnnulusContour(ROI* roi, Annulus annulus);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddClosedContour(ROI* roi, const Point* points, int numPoints);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddLineContour(ROI* roi, Point start, Point end);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddOpenContour(ROI* roi, const Point* points, int numPoints);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddOvalContour(ROI* roi, Rect boundingBox);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddPointContour(ROI* roi, Point point);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddRectContour(ROI* roi, Rect rect);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqAddRotatedRectContour2(ROI* roi, RotatedRect rect);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqCopyContour(ROI* destRoi, const ROI* sourceRoi, ContourID id);
    //IMAQ_FUNC ContourID     IMAQ_STDCALL imaqGetContour(const ROI* roi, unsigned int index);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetContourColor(const ROI* roi, ContourID id, RGBValue* contourColor);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqGetContourCount(const ROI* roi);
    //IMAQ_FUNC ContourInfo2* IMAQ_STDCALL imaqGetContourInfo2(const ROI* roi, ContourID id);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqMoveContour(ROI* roi, ContourID id, int deltaX, int deltaY);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqRemoveContour(ROI* roi, ContourID id);
    //IMAQ_FUNC int           IMAQ_STDCALL imaqSetContourColor(ROI* roi, ContourID id, const RGBValue* color);

    //============================================================================
    //  Regions of Interest functions
    //============================================================================
    //IMAQ_FUNC int  IMAQ_STDCALL imaqConstructROI2(const Image* image, ROI* roi, Tool initialTool, const ToolWindowOptions* tools, const ConstructROIOptions2* options, int* okay);
    //IMAQ_FUNC ROI* IMAQ_STDCALL imaqCreateROI();
    //IMAQ_FUNC int  IMAQ_STDCALL imaqGetROIBoundingBox(const ROI* roi, Rect* boundingBox);
    //IMAQ_FUNC int  IMAQ_STDCALL imaqGetROIColor(const ROI* roi, RGBValue* roiColor);
    //IMAQ_FUNC ROI* IMAQ_STDCALL imaqGetWindowROI(int windowNumber);
    //IMAQ_FUNC int  IMAQ_STDCALL imaqSetROIColor(ROI* roi, const RGBValue* color);
    //IMAQ_FUNC int  IMAQ_STDCALL imaqSetWindowROI(int windowNumber, const ROI* roi);

    //============================================================================
    //  Image Analysis functions
    //============================================================================
    //IMAQ_FUNC int              IMAQ_STDCALL imaqCentroid(const Image* image, PointFloat* centroid, const Image* mask);
    //IMAQ_FUNC Curve*           IMAQ_STDCALL imaqExtractCurves(const Image* image, const ROI* roi, const CurveOptions* curveOptions, unsigned int* numCurves);
    //IMAQ_FUNC HistogramReport* IMAQ_STDCALL imaqHistogram(const Image* image, int numClasses, float min, float max, const Image* mask);

    private static final BlockingFunction imaqLinearAverages2Fn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqLinearAverages2");

    public static LinearAverages getLinearAverages(Pointer image, LinearAverages.LinearAveragesMode mode, Rect rect) throws NIVisionException {
        int returnedAddress = imaqLinearAverages2Fn.call3(image, mode.value, rect.getPointer());

        try {
            NIVision.assertCleanStatus(returnedAddress);
        } catch (NIVisionException ex) {
            if (!ex.getMessage().equals("No error."))
                throw ex;
        }

        LinearAverages averages = new LinearAverages(returnedAddress);
        NIVision.dispose(new Pointer(returnedAddress,0));
        return averages;
    }

    //IMAQ_FUNC LineProfile*     IMAQ_STDCALL imaqLineProfile(const Image* image, Point start, Point end);
    //IMAQ_FUNC QuantifyReport*  IMAQ_STDCALL imaqQuantify(const Image* image, const Image* mask);

    //============================================================================
    //  Error Management functions
    //============================================================================
    private static final BlockingFunction imaqGetLastErrorFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqGetLastError");
    static {
        imaqGetLastErrorFn.setTaskExecutor(taskExecutor);
    }

    public static int getLastError() {
        return imaqGetLastErrorFn.call0();
    }
    protected static void assertCleanStatus (int code) throws NIVisionException {
        if (code == 0) {
            throw new NIVisionException(imaqGetLastErrorFn.call0());
        }
    }

    //IMAQ_FUNC int         IMAQ_STDCALL imaqClearError();
    //IMAQ_FUNC char*       IMAQ_STDCALL imaqGetErrorText(int errorCode);
    //IMAQ_FUNC const char* IMAQ_STDCALL imaqGetLastErrorFunc();
    //IMAQ_FUNC int         IMAQ_STDCALL imaqSetError(int errorCode, const char* function);

    //============================================================================
    //  Threshold functions
    //============================================================================
    //IMAQ_FUNC ThresholdData* IMAQ_STDCALL imaqAutoThreshold2(Image* dest, const Image* source, int numClasses, ThresholdMethod method, const Image* mask);
    //IMAQ_FUNC int            IMAQ_STDCALL imaqLocalThreshold(Image* dest, const Image* source, unsigned int windowWidth, unsigned int windowHeight, LocalThresholdMethod method, double deviationWeight, ObjectType type, float replaceValue);
    //IMAQ_FUNC int            IMAQ_STDCALL imaqMagicWand(Image* dest, const Image* source, Point coord, float tolerance, int connectivity8, float replaceValue);
    //IMAQ_FUNC int            IMAQ_STDCALL imaqMultithreshold(Image* dest, const Image* source, const ThresholdData* ranges, int numRanges);
    //IMAQ_FUNC int            IMAQ_STDCALL imaqThreshold(Image* dest, const Image* source, float rangeMin, float rangeMax, int useNewValue, float newValue);

    //============================================================================
    //  Memory Management functions
    //============================================================================

    private static final BlockingFunction imaqDisposeFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqDispose");
    static {
        imaqDisposeFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Cleans up resources associated with images, regions of interest (ROIs),
     * arrays, and reports that you no longer need.
     * After you dispose of something, you can no longer use it.
     * @param item The image, ROI, array, or report whose memory you want to free.
     */
    public static void dispose (Pointer item)  throws NIVisionException {
        assertCleanStatus(imaqDisposeFn.call1(item));
    }

    //============================================================================
    //  File I/O functions
    //============================================================================

    private static final BlockingFunction Priv_ReadJPEGString_CFn = NativeLibrary.getDefaultInstance().getBlockingFunction("Priv_ReadJPEGString_C");
    static {
        Priv_ReadJPEGString_CFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Read a pointer to a byte array into the given image
     * @param image pointer to an imaq image object
     * @param p pointer to a byte array holding image data
     */
    public static void readJpegString(Pointer image, Pointer p) {
        Priv_ReadJPEGString_CFn.call3(image, p, p.getSize());
    }

    private static final BlockingFunction Priv_SetWriteFileAllowedFn = NativeLibrary.getDefaultInstance().getBlockingFunction("Priv_SetWriteFileAllowed");
    static {
        Priv_SetWriteFileAllowedFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Set true to be able to create files on RoboRIO
     * @param val true allows files to be created
     */
    public static void setWriteFileAllowed(boolean val) {
        Priv_SetWriteFileAllowedFn.call1(val ? 1 : 0);
    }

    private static final Function imaqWriteFileFn = NativeLibrary.getDefaultInstance().getFunction("imaqWriteFile");
//    static { imaqWriteFileFn.setTaskExecutor(taskExecutor); }

    /**
     * Write an image to the given file.
     *
     * Supported extensions:
     * .aipd or .apd AIPD
     * .bmp BMP
     * .jpg or .jpeg JPEG
     * .jp2 JPEG2000
     * .png PNG
     * .tif or .tiff TIFF
     *
     * @param image The image to write to a file.
     * @param fileName The name of the destination file.
     */
    public static void writeFile(Pointer image, String fileName) throws NIVisionException {
        Pointer p = new Pointer(fileName.length() + 1);
        p.setString(0, fileName);
        setWriteFileAllowed(true);
        try {
            assertCleanStatus(imaqWriteFileFn.call3(image, p, 0)); //zero is unused color table
        } finally {
            p.free();
        }
    }

    /**
     * Write an image to the given file.
     *
     * Supported extensions:
     * .aipd or .apd AIPD
     * .bmp BMP
     * .jpg or .jpeg JPEG
     * .jp2 JPEG2000
     * .png PNG
     * .tif or .tiff TIFF
     *
     * @param image The image to write to a file.
     * @param fileName The name of the destination file.
     * @param colorTable The color table to use for 8-bit images
     */
    public static void writeFile(Pointer image, String fileName, Pointer colorTable)  throws NIVisionException {
        Pointer p = new Pointer(fileName.length() + 1);
        p.setString(0, fileName);
        setWriteFileAllowed(true);
        try {
            assertCleanStatus(imaqWriteFileFn.call3(image, p, colorTable)); //zero is unused color table
        } finally {
            p.free();
        }
    }

    private static final BlockingFunction imaqReadFileFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqReadFile");
    static {
        imaqReadFileFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Read an image from to the given image from the given filename
     * @param image The image to store the data in.
     * @param fileName The name of the file to load.
     */
    public static void readFile(Pointer image, String fileName)  throws NIVisionException {
        Pointer p = new Pointer(fileName.length() + 1);
        p.setString(0, fileName);
        try {
            assertCleanStatus(imaqReadFileFn.call4(image, p, 0, 0)); //zeros are unused color table and num colors
        } finally {
            p.free();
        }
    }

    //IMAQ_FUNC int         IMAQ_STDCALL imaqCloseAVI(AVISession session);
    //IMAQ_FUNC AVISession  IMAQ_STDCALL imaqCreateAVI(const char* fileName, const char* compressionFilter, int quality, unsigned int framesPerSecond, unsigned int maxDataSize);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqGetAVIInfo(AVISession session, AVIInfo* info);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqGetFileInfo(const char* fileName, CalibrationUnit* calibrationUnit, float* calibrationX, float* calibrationY, int* width, int* height, ImageType* imageType);
    //IMAQ_FUNC FilterName* IMAQ_STDCALL imaqGetFilterNames(int* numFilters);
    //IMAQ_FUNC char**      IMAQ_STDCALL imaqLoadImagePopup(const char* defaultDirectory, const char* defaultFileSpec, const char* fileTypeList, const char* title, int allowMultiplePaths, ButtonLabel buttonLabel, int restrictDirectory, int restrictExtension, int allowCancel, int allowMakeDirectory, int* cancelled, int* numPaths);
    //IMAQ_FUNC AVISession  IMAQ_STDCALL imaqOpenAVI(const char* fileName);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqReadAVIFrame(Image* image, AVISession session, unsigned int frameNum, void* data, unsigned int* dataSize);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqReadVisionFile(Image* image, const char* fileName, RGBValue* colorTable, int* numColors);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteAVIFrame(Image* image, AVISession session, const void* data, unsigned int dataLength);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteBMPFile(const Image* image, const char* fileName, int compress, const RGBValue* colorTable);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteJPEGFile(const Image* image, const char* fileName, unsigned int quality, void* colorTable);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteJPEG2000File(const Image* image, const char* fileName, int lossless, float compressionRatio, const JPEG2000FileAdvancedOptions* advancedOptions, const RGBValue* colorTable);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWritePNGFile2(const Image* image, const char* fileName, unsigned int compressionSpeed, const RGBValue* colorTable, int useBitDepth);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteTIFFFile(const Image* image, const char* fileName, const TIFFFileOptions* options, const RGBValue* colorTable);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqWriteVisionFile(const Image* image, const char* fileName, const RGBValue* colorTable);


    //============================================================================
    //  Frequency Domain Analysis functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqAttenuate(Image* dest, const Image* source, AttenuateMode highlow);
    //IMAQ_FUNC int IMAQ_STDCALL imaqConjugate(Image* dest, const Image* source);
    //IMAQ_FUNC int IMAQ_STDCALL imaqFFT(Image* dest, const Image* source);
    //IMAQ_FUNC int IMAQ_STDCALL imaqFlipFrequencies(Image* dest, const Image* source);
    //IMAQ_FUNC int IMAQ_STDCALL imaqInverseFFT(Image* dest, const Image* source);
    //IMAQ_FUNC int IMAQ_STDCALL imaqTruncate(Image* dest, const Image* source, TruncateMode highlow, float ratioToKeep);

    //============================================================================
    //  Pixel Manipulation functions
    //============================================================================

    private static final BlockingFunction imaqExtractColorPlanesFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqExtractColorPlanes");
    static {
        imaqExtractColorPlanesFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Extract the color planes from the given source image into the given planes
     * @param source The color source image.
     * @param mode The color space to extract the planes of.
     * @param plane1 MonoImage destination for the first plane.
     * @param plane2 MonoImage destination for the first plane.
     * @param plane3 MonoImage destination for the first plane.
     */
    public static void extractColorPlanes(Pointer source, ColorMode mode, Pointer plane1, Pointer plane2, Pointer plane3)  throws NIVisionException {
        int plane_1 = 0;
        int plane_2 = 0;
        int plane_3 = 0;
        if (plane1 != null)
            plane_1 = plane1.address().toUWord().toPrimitive();
        if (plane2 != null)
            plane_2 = plane2.address().toUWord().toPrimitive();
        if (plane3 != null)
            plane_3 = plane3.address().toUWord().toPrimitive();
        assertCleanStatus(imaqExtractColorPlanesFn.call5(source, mode.value, plane_1, plane_2, plane_3));
    }

    private static final BlockingFunction imaqReplaceColorPlanesFn = NativeLibrary.getDefaultInstance().getBlockingFunction("imaqReplaceColorPlanes");
    static {
        imaqReplaceColorPlanesFn.setTaskExecutor(taskExecutor);
    }

    /**
     * Replaces one or more of the color planes of a color image. The plane you
     * replace may be independent of the image type. For example, you can
     * replace the green plane of an RGB image or the hue plane of an HSL image.
     * @param dest The destination image.
     * @param source The source image.
     * @param mode The color space in which the function replaces planes.
     * @param plane1 The first plane of replacement data. Set this parameter to null if you do not want to change the first plane of the source image.
     * @param plane2 The second plane of replacement data. Set this parameter to null if you do not want to change the second plane of the source image.
     * @param plane3 The third plane of replacement data. Set this parameter to null if you do not want to change the third plane of the source image.
     */
    public static void replaceColorPlanes(Pointer dest, Pointer source,
                                          ColorMode mode, Pointer plane1, Pointer plane2, Pointer plane3)  throws NIVisionException {
        int plane_1 = 0;
        int plane_2 = 0;
        int plane_3 = 0;
        if (plane1 != null)
            plane_1 = plane1.address().toUWord().toPrimitive();
        if (plane2 != null)
            plane_2 = plane2.address().toUWord().toPrimitive();
        if (plane3 != null)
            plane_3 = plane3.address().toUWord().toPrimitive();
        assertCleanStatus(imaqReplaceColorPlanesFn.call6(
                              dest.address().toUWord().toPrimitive(),
                              source.address().toUWord().toPrimitive(),
                              mode.value,
                              plane_1,
                              plane_2,
                              plane_3));
    }

    //IMAQ_FUNC int    IMAQ_STDCALL imaqArrayToComplexPlane(Image* dest, const Image* source, const float* newPixels, ComplexPlane plane);
    //IMAQ_FUNC float* IMAQ_STDCALL imaqComplexPlaneToArray(const Image* image, ComplexPlane plane, Rect rect, int* columns, int* rows);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqExtractComplexPlane(Image* dest, const Image* source, ComplexPlane plane);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqFillImage(Image* image, PixelValue value, const Image* mask);
    //IMAQ_FUNC void*  IMAQ_STDCALL imaqGetLine(const Image* image, Point start, Point end, int* numPoints);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqGetPixel(const Image* image, Point pixel, PixelValue* value);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqReplaceComplexPlane(Image* dest, const Image* source, const Image* newValues, ComplexPlane plane);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetLine(Image* image, const void* array, int arraySize, Point start, Point end);
    //IMAQ_FUNC int    IMAQ_STDCALL imaqSetPixel(Image* image, Point coord, PixelValue value);

    //============================================================================
    //  LCD functions
    //============================================================================
    //IMAQ_FUNC int        IMAQ_STDCALL imaqFindLCDSegments(ROI* roi, const Image* image, const LCDOptions* options);
    //IMAQ_FUNC LCDReport* IMAQ_STDCALL imaqReadLCD(const Image* image, const ROI* roi, const LCDOptions* options);

    //============================================================================
    //  Regions of Interest Manipulation functions
    //============================================================================
    //IMAQ_FUNC ROI*        IMAQ_STDCALL imaqMaskToROI(const Image* mask, int* withinLimit);
    //IMAQ_FUNC ROIProfile* IMAQ_STDCALL imaqROIProfile(const Image* image, const ROI* roi);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqROIToMask(Image* mask, const ROI* roi, int fillValue, const Image* imageModel, int* inSpace);
    //IMAQ_FUNC int         IMAQ_STDCALL imaqTransformROI2(ROI* roi, const CoordinateSystem* baseSystem, const CoordinateSystem* newSystem);

    //============================================================================
    //  OCR functions
    //============================================================================
    //IMAQ_FUNC CharSet*         IMAQ_STDCALL imaqCreateCharSet();
    //IMAQ_FUNC int              IMAQ_STDCALL imaqDeleteChar(CharSet* set, int index);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqGetCharCount(const CharSet* set);
    //IMAQ_FUNC CharInfo2*       IMAQ_STDCALL imaqGetCharInfo2(const CharSet* set, int index);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqReadOCRFile(const char* fileName, CharSet* set, String255 setDescription, ReadTextOptions* readOptions, OCRProcessingOptions* processingOptions, OCRSpacingOptions* spacingOptions);
    //IMAQ_FUNC ReadTextReport3* IMAQ_STDCALL imaqReadText3(const Image* image, const CharSet* set, const ROI* roi, const ReadTextOptions* readOptions, const OCRProcessingOptions* processingOptions, const OCRSpacingOptions* spacingOptions);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqRenameChar(CharSet* set, int index, const char* newCharValue);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqSetReferenceChar(const CharSet* set, int index, int isReferenceChar);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqTrainChars(const Image* image, CharSet* set, int index, const char* charValue, const ROI* roi, const OCRProcessingOptions* processingOptions, const OCRSpacingOptions* spacingOptions);
    //IMAQ_FUNC int*             IMAQ_STDCALL imaqVerifyPatterns(const Image* image, const CharSet* set, const String255* expectedPatterns, int patternCount, const ROI* roi, int* numScores);
    //IMAQ_FUNC int*             IMAQ_STDCALL imaqVerifyText(const Image* image, const CharSet* set, const char* expectedString, const ROI* roi, int* numScores);
    //IMAQ_FUNC int              IMAQ_STDCALL imaqWriteOCRFile(const char* fileName, const CharSet* set, const char* setDescription, const ReadTextOptions* readOptions, const OCRProcessingOptions* processingOptions, const OCRSpacingOptions* spacingOptions);

    //============================================================================
    //  Classification functions
    //============================================================================
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqAddClassifierSample(Image* image, ClassifierSession* session, const ROI* roi, const char* sampleClass, double* featureVector, unsigned int vectorSize);
    //IMAQ_FUNC ClassifierReport*              IMAQ_STDCALL imaqClassify(Image* image, const ClassifierSession* session, const ROI* roi, double* featureVector, unsigned int vectorSize);
    //IMAQ_FUNC ClassifierSession*             IMAQ_STDCALL imaqCreateClassifier(ClassifierType type);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqDeleteClassifierSample(ClassifierSession* session, int index);
    //IMAQ_FUNC ClassifierAccuracyReport*      IMAQ_STDCALL imaqGetClassifierAccuracy(const ClassifierSession* session);
    //IMAQ_FUNC ClassifierSampleInfo*          IMAQ_STDCALL imaqGetClassifierSampleInfo(const ClassifierSession* session, int index, int* numSamples);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqGetNearestNeighborOptions(const ClassifierSession* session, NearestNeighborOptions* options);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqGetParticleClassifierOptions(const ClassifierSession* session, ParticleClassifierPreprocessingOptions* preprocessingOptions, ParticleClassifierOptions* options);
    //IMAQ_FUNC ClassifierSession*             IMAQ_STDCALL imaqReadClassifierFile(ClassifierSession* session, const char* fileName, ReadClassifierFileMode mode, ClassifierType* type, ClassifierEngineType* engine, String255 description);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqRelabelClassifierSample(ClassifierSession* session, int index, const char* newClass);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqSetParticleClassifierOptions(ClassifierSession* session, const ParticleClassifierPreprocessingOptions* preprocessingOptions, const ParticleClassifierOptions* options);
    //IMAQ_FUNC NearestNeighborTrainingReport* IMAQ_STDCALL imaqTrainNearestNeighborClassifier(ClassifierSession* session, const NearestNeighborOptions* options);
    //IMAQ_FUNC int                            IMAQ_STDCALL imaqWriteClassifierFile(const ClassifierSession* session, const char* fileName, WriteClassifierFileMode mode, const String255 description);

    //============================================================================
    //  Inspection functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqCompareGoldenTemplate(const Image* image, Image* goldenTemplate, Image* brightDefects, Image* darkDefects, const InspectionAlignment* alignment, const InspectionOptions* options);
    //IMAQ_FUNC int IMAQ_STDCALL imaqLearnGoldenTemplate(Image* goldenTemplate, PointFloat originOffset, const Image* mask);

    //============================================================================
    //  Morphology functions
    //============================================================================
    //IMAQ_FUNC int IMAQ_STDCALL imaqGrayMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement);

}
