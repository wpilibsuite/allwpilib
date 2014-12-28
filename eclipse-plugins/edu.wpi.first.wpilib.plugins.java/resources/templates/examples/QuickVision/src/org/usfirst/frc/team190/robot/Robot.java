package $package;

import edu.wpi.first.wpilibj.CameraServer;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Timer;

/**
 * This is a demo program showing the use of the CameraServer class.
 * With start automatic capture, there is no opportunity to process the image.
 * Look at the IntermediateVision sample for how to process the image before sending it to the FRC PC Dashboard.
 */
public class Robot extends SampleRobot {

    CameraServer server;

    public Robot() {
        server = CameraServer.getInstance();
        server.setQuality(50);
        //the camera name (ex "cam0") can be found through the roborio web interface
        server.startAutomaticCapture("cam0");
    }

    /**
     * start up automatic capture you should see the video stream from the
     * webcam in your FRC PC Dashboard.
     */
    public void operatorControl() {

        while (isOperatorControl() && isEnabled()) {
            /** robot code here! **/
            Timer.delay(0.005);		// wait for a motor update time
        }
    }

}