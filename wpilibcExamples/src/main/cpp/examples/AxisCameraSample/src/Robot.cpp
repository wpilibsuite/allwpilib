/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <CameraServer.h>
#include <IterativeRobot.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * This is a demo program showing the use of OpenCV to do vision processing. The
 * image is acquired from the Axis camera, then a rectangle is put on the image
 * and
 * sent to the dashboard. OpenCV has many methods for different types of
 * processing.
 */
class Robot : public frc::IterativeRobot {
private:
	static void VisionThread() {
		// Get the Axis camera from CameraServer
		cs::AxisCamera camera =
				CameraServer::GetInstance()->AddAxisCamera(
						"axis-camera.local");
		// Set the resolution
		camera.SetResolution(640, 480);

		// Get a CvSink. This will capture Mats from the Camera
		cs::CvSink cvSink = CameraServer::GetInstance()->GetVideo();
		// Setup a CvSource. This will send images back to the Dashboard
		cs::CvSource outputStream =
				CameraServer::GetInstance()->PutVideo(
						"Rectangle", 640, 480);

		// Mats are very memory expensive. Lets reuse this Mat.
		cv::Mat mat;

		while (true) {
			// Tell the CvSink to grab a frame from the camera and
			// put it
			// in the source mat.  If there is an error notify the
			// output.
			if (cvSink.GrabFrame(mat) == 0) {
				// Send the output the error.
				outputStream.NotifyError(cvSink.GetError());
				// skip the rest of the current iteration
				continue;
			}
			// Put a rectangle on the image
			rectangle(mat, cv::Point(100, 100), cv::Point(400, 400),
					cv::Scalar(255, 255, 255), 5);
			// Give the output stream a new image to display
			outputStream.PutFrame(mat);
		}
	}

	void RobotInit() {
		// We need to run our vision program in a separate Thread.
		// If not, our robot program will not run
		std::thread visionThread(VisionThread);
		visionThread.detach();
	}
};

START_ROBOT_CLASS(Robot)
