package $package;

import com.ni.vision.NIVision;
import com.ni.vision.NIVision.DrawMode;
import com.ni.vision.NIVision.Image;
import com.ni.vision.NIVision.ShapeMode;

import edu.wpi.first.wpilibj.CameraServer;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.vision.AxisCamera;

/**
 * This demo shows the use of the AxisCamera class.
 * Uses AxisCamera class to manually acquire a new image each frame, and annotate the image by drawing
 * a circle on it, and show it on the FRC Dashboard.
 */

public class Robot extends SampleRobot {
    int session;
    Image frame;
    AxisCamera camera;

    public void robotInit() {

        frame = NIVision.imaqCreateImage(NIVision.ImageType.IMAGE_RGB, 0);

        // open the camera at the IP address assigned. This is the IP address that the camera
        // can be accessed through the web interface.
        camera = new AxisCamera("10.1.91.100");
    }

    public void operatorControl() {

        /**
         * grab an image from the camera, draw the circle, and provide it for the camera server
         * which will in turn send it to the dashboard.
         */
        NIVision.Rect rect = new NIVision.Rect(10, 10, 100, 100);

        while (isOperatorControl() && isEnabled()) {
            camera.getImage(frame);
            NIVision.imaqDrawShapeOnImage(frame, frame, rect,
                    DrawMode.DRAW_VALUE, ShapeMode.SHAPE_OVAL, 0.0f);

            CameraServer.getInstance().setImage(frame);

            /** robot code here! **/
            Timer.delay(0.005);		// wait for a motor update time
        }
    }

    public void test() {
    }
}
