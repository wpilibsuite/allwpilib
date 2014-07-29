/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.camera;

import com.sun.jna.BlockingFunction;
import com.sun.jna.Function;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.TaskExecutor;
import com.sun.squawk.VM;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.image.ColorImage;
import edu.wpi.first.wpilibj.image.HSLImage;
import edu.wpi.first.wpilibj.image.NIVisionException;
import edu.wpi.first.wpilibj.util.BoundaryException;

//TODO figure out where to use finally to free resources
//TODO go through old camera code and make sure all features are implemented
//TODO make work with all three passwords
//TODO get images of different types
//TODO continue attempting to connect until succesful
//TODO optimize and use Pointers in all locations which make sense - possibly JNA memcpy?
/**
 * This class is a singleton used to configure and get images from the axis camera.
 * @author dtjones
 */
public class AxisCamera {

    private static AxisCamera m_instance = null;

    /**
     * Enumaration representing the different values which exposure may be set to.
     */
    public static class ExposureT {

        /**
         * The integer value of the enumeration.
         */
        public final int value;
        static final ExposureT[] allValues = new ExposureT[4];
        /**
         * The Axis camera automatically determines what exposure level to use.
         */
        public static final ExposureT automatic = new ExposureT(0);
        /**
         * Hold the current exposure level.
         */
        public static final ExposureT hold = new ExposureT(1);
        /**
         * Set exposure for flicker free 50 hz.
         */
        public static final ExposureT flickerfree50 = new ExposureT(2);
        /**
         * Set exposure for flicker free 60 hz.
         */
        public static final ExposureT flickerfree60 = new ExposureT(3);

        private ExposureT(int value) {
            this.value = value;
            allValues[value] = this;
        }

        private static ExposureT get(int val) {
            return allValues[val];
        }
    }

    /**
     * Enumeration representing the different values which white balence may be
     * set to.
     */
    public static class WhiteBalanceT {

        /**
         * The integer value of the enumeration.
         */
        public final int value;
        static final WhiteBalanceT[] allValues = new WhiteBalanceT[7];
        /**
         * The axis camera automatically adjusts the whit balance.
         */
        public static final WhiteBalanceT automatic = new WhiteBalanceT(0);
        /**
         * Hold the current white balance.
         */
        public static final WhiteBalanceT hold = new WhiteBalanceT(1);
        /**
         * White balance for outdoors.
         */
        public static final WhiteBalanceT fixedOutdoor1 = new WhiteBalanceT(2);
        /**
         * White balance for outdoors.
         */
        public static final WhiteBalanceT fixedOutdoor2 = new WhiteBalanceT(3);
        /**
         * White balance for indoors.
         */
        public static final WhiteBalanceT fixedIndoor = new WhiteBalanceT(4);
        /**
         * White balance for fourescent lighting.
         */
        public static final WhiteBalanceT fixedFlour1 = new WhiteBalanceT(5);
        /**
         * White balance for fourescent lighting.
         */
        public static final WhiteBalanceT fixedFlour2 = new WhiteBalanceT(6);

        private WhiteBalanceT(int value) {
            this.value = value;
            allValues[value] = this;
        }

        private static WhiteBalanceT get(int value) {
            return allValues[value];
        }
    }

    /**
     * Enumeration representing the image resoultion provided by the camera.
     */
    public static class ResolutionT {

        /**
         * The integer value of the enumeration.
         */
        public final int value;
        /**
         * Number of pixels wide.
         */
        public final int width;
        /**
         * Number of pixels tall.
         */
        public final int height;
        static final ResolutionT[] allValues = new ResolutionT[4];
        /**
         * Image is 640 pixels wide by 480 tall
         */
        public static final ResolutionT k640x480 = new ResolutionT(0, 640, 480);
        /**
         * Image is 640 pixels wide by 360 tall
         */
        public static final ResolutionT k640x360 = new ResolutionT(1, 640, 360);
        /**
         * Image is 320 pixels wide by 240 tall
         */
        public static final ResolutionT k320x240 = new ResolutionT(2, 320, 240);
        /**
         * Image is 160 pixels wide by 120 tall
         */
        public static final ResolutionT k160x120 = new ResolutionT(3, 160, 120);

        private ResolutionT(int value, int horizontal, int vertical) {
            this.value = value;
            this.width = horizontal;
            this.height = vertical;
            allValues[value] = this;
        }

        private static ResolutionT get(int value) {
            return allValues[value];
        }
    }

    /**
     * Enumeration representing the orientation of the picture.
     */
    public static class RotationT {

        /**
         * The integer value of the enumeration.
         */
        public final int value;
        static final RotationT[] allValues = new RotationT[2];
        /**
         * Picture is right side up.
         */
        public static final RotationT k0 = new RotationT(0);
        /**
         * Picture is rotated 180 degrees.
         */
        public static final RotationT k180 = new RotationT(1);

        private RotationT(int value) {
            this.value = value;
            allValues[value] = this;
        }

        private static RotationT get(int value) {
            return allValues[value];
        }
    }

    /**
     * Enumeration representing the exposure priority.
     */
    public static class ExposurePriorityT {

        /**
         * The integer value of the enumeration.
         */
        public final int value;
        static final ExposurePriorityT[] allValues = new ExposurePriorityT[3];
        /**
         * Prioritize image quality.
         */
        public static final ExposurePriorityT imageQuality = new ExposurePriorityT(0);
        /**
         * No prioritization.
         */
        public static final ExposurePriorityT none = new ExposurePriorityT(50);
        /**
         * Prioritize frame rate.
         */
        public static final ExposurePriorityT frameRate = new ExposurePriorityT(100);

        private ExposurePriorityT(int value) {
            this.value = value;
            allValues[value / 50] = this;
        }

        private static ExposurePriorityT get(int value) {
            return allValues[value / 50];
        }
    }

    /**
     * Get a reference to the AxisCamera, or initialize the AxisCamera if it
     * has not yet been initialized. If the camera is connected to the
     * Ethernet switch on the robot, then this address should be 10.x.y.11
     * where x.y are your team number subnet address (same as the other IP
     * addresses on the robot network).
     * @param address A string containing the IP address for the camera in the
     * form "10.x.y.2" for cameras on the Ethernet switch or "192.168.0.90"
     * for cameras connected to the 2nd Ethernet port on an 8-slot cRIO.
     * @return A reference to the AxisCamera.
     */
    public static synchronized AxisCamera getInstance(String address) {
        if (m_instance == null) {
            m_instance = new AxisCamera(address);
    		// XXX: Resource Reporting Fixes
//            UsageReporting.report(UsageReporting.kResourceType_AxisCamera, 2);
        }
        return m_instance;
    }

    /**
     * Get a reference to the AxisCamera, or initialize the AxisCamera if it
     * has not yet been initialized. By default this will connect to a camera
     * with an IP address of 10.x.y.11 with the preference that the camera be
     * connected to the Ethernet switch on the robot rather than port 2 of the
     * 8-slot cRIO.
     * @return A reference to the AxisCamera.
     */
    public static synchronized AxisCamera getInstance() {
        if (m_instance == null) {
            DriverStation.getInstance().waitForData();
            int teamNumber = DriverStation.getInstance().getTeamNumber();
            String address = "10."+(teamNumber/100)+"."+(teamNumber%100)+".11";
            m_instance = new AxisCamera(address);
            UsageReporting.report(UsageReporting.kResourceType_AxisCamera, 1);
        }

        return m_instance;
    }
    private static final Function cameraStartFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraStart");

    /**
     * Axis camera constructor that calls the C++ library to actually create the instance.
     * @param IPAddress
     */
    AxisCamera(String IPAddress) {
        Pointer ptr = Pointer.createStringBuffer(IPAddress);
        cameraStartFn.call1(ptr);
    }
    private static final TaskExecutor cameraTaskExecutor = new TaskExecutor("camera task executor");
    private static final BlockingFunction getImageFn = NativeLibrary.getDefaultInstance().getBlockingFunction("AxisCameraGetImage");

    static {
        getImageFn.setTaskExecutor(cameraTaskExecutor);
    }

    /**
     * Get an image from the camera. Be sure to free the image when you are done with it.
     * @return A new image from the camera.
     */
    public ColorImage getImage() throws AxisCameraException, NIVisionException {
        ColorImage image = new HSLImage();
        if (getImageFn.call1(image.image) == 0) {
            image.free();
            throw new AxisCameraException("No image available");
        }
        return image;
    }
    // Mid-stream gets & writes
    private static final Function writeBrightnessFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteBrightness");

    /**
     * Write the brightness for the camera to use.
     * @param brightness This must be an integer between 0 and 100, with 100 being the brightest
     */
    public void writeBrightness(int brightness) {
        BoundaryException.assertWithinBounds(brightness, 0, 100);
        writeBrightnessFn.call1(brightness);
    }
    private static final Function getBrightnessFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetBrightness");

    /**
     * Get the current brightness of the AxisCamera
     * @return An integer representing the current brightness of the axis
     * camera with 0 being the darkest and 100 being the brightest.
     */
    public int getBrightness() {
        return getBrightnessFn.call0();
    }
    private static final Function writeWhiteBalenceFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteWhiteBalance");

    /**
     * Write the WhiteBalance for the camera to use.
     * @param whiteBalance The value to set the white balance to on the camera.
     */
    public void writeWhiteBalance(WhiteBalanceT whiteBalance) {
        writeWhiteBalenceFn.call1(whiteBalance.value);
    }
    private static final Function getWhiteBalanceFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetWhiteBalance");

    /**
     * Get the white balance set on the camera.
     * @return The white balance currently set on the camera.
     */
    public WhiteBalanceT getWhiteBalance() {
        return WhiteBalanceT.get(getWhiteBalanceFn.call0());
    }
    private static final Function writeColorLevelFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteColorLevel");

    /**
     * Set the color level of images returned from the camera.
     * @param value This must be an integer from 0 to 100 with 0 being black and white.
     */
    public void writeColorLevel(int value) {
        BoundaryException.assertWithinBounds(value, 0, 100);
        writeColorLevelFn.call1(value);
    }
    private static final Function getColorLevelFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetColorLevel");

    /**
     * Get the color level of images being retunred from the camera.
     * @return The color level of images being returned from the camera. 0 is black and white.
     */
    public int getColorLevel() {
        return getColorLevelFn.call0();
    }
    private static final Function writeExposureControlFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteExposureControl");

    /**
     * Write the exposure mode for the camera to use.
     * @param value The expsure mode for the camera to use.
     */
    public void writeExposureControl(ExposureT value) {
        writeExposureControlFn.call1(value.value);
    }
    private static final Function getExposureControlFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetExposureControl");

    /**
     * Get the exposure mode that the camera is using.
     * @return The exposure mode that the camera is using.
     */
    public ExposureT getExposureControl() {
        return ExposureT.get(getExposureControlFn.call0());
    }
    private static final Function writeExposurePriorityFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteExposurePriority");

    /**
     * Write the exposure priority for the camera to use.
     * @param value The exposure priority to use.
     */
    public void writeExposurePriority(ExposurePriorityT value) {
        writeExposurePriorityFn.call1(value.value);
    }
    private static final Function getExposurePriorityFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetExposurePriority");

    /**
     * Get the exposure priority that the camera is using.
     * @return The exposure priority that the camera is using
     */
    public ExposurePriorityT getExposurePriority() {
        return ExposurePriorityT.get(getExposurePriorityFn.call0());
    }
    // New-Stream gets & writes
    private static final Function writeResolutionFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteResolution");

    /**
     * Set the resolution of the images to return.
     * @param value The resolution imaga for the camera to return.
     */
    public void writeResolution(ResolutionT value) {
        writeResolutionFn.call1(value.value);
    }
    private static final Function getResolutionFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetResolution");

    /**
     * Get the resolution fo the images that the camera is returning.
     * @return  The resolution of the images that the camera is returning.
     */
    public ResolutionT getResolution() {
        return ResolutionT.get(getResolutionFn.call0());
    }
    private static final Function writeCompressionFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteCompression");

    /**
     * Write the level of JPEG compression to use on images returned from the camera.
     * @param value The level of JPEG compression to use from 0 to 100 with 0 being the least compression.
     */
    public void writeCompression(int value) {
        BoundaryException.assertWithinBounds(value, 0, 100);
        writeCompressionFn.call1(value);
    }
    private static final Function getCompressionFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetCompression");

    /**
     * Get the compression of the images eing returned by the camera.
     * @return The level of compression of the image being returned from the
     * camera with 0 being the least and 100 being the greatest.
     */
    public int getCompression() {
        return getCompressionFn.call0();
    }
    private static final Function writeRotationFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteRotation");

    /**
     * Write the rotation of images for the camera to return.
     * @param value The rotation to be applied to images from the camera.
     */
    public void writeRotation(RotationT value) {
        writeRotationFn.call1(value.value);
    }
    private static final Function getRotationFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetRotation");

    /**
     * Get the rotation of the images returned from the camera.
     * @return The rotation of the images returned from the camera.
     */
    public RotationT getRotation() {
        return RotationT.get(getRotationFn.call0());
    }
    private static final Function freshImageFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraFreshImage");

    /**
     * Has the current image from the camera been retrieved yet.
     * @return true if the latest image from the camera has not been retrieved yet.
     */
    public boolean freshImage() {
        return freshImageFn.call0() == 0 ? false : true;
    }
    private static final Function getMaxFPSFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraGetMaxFPS");

    /**
     * Get the maximum frames per second that the camera will generate.
     * @return the max fps.
     */
    public int getMaxFPS() {
        return getMaxFPSFn.call0();
    }
    private static final Function writeMaxFPSFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraWriteMaxFPS");

    /**
     * Set the maximum frames per second that the camera will generate.
     * @param value the new max fps
     */
    public void writeMaxFPS(int value) {
        writeMaxFPSFn.call1(value);
    }
    private static final Function deleteInstanceFn = NativeLibrary.getDefaultInstance().getFunction("AxisCameraDeleteInstance");

    static {
        VM.addShutdownHook(new Thread() {

            public void run() {
                deleteInstanceFn.call0();
            }
        });
    }
}
