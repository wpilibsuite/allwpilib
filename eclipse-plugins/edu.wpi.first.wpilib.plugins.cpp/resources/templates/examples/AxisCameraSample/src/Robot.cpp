#include "WPILib.h"

/**
 * Uses AxisCamera class to manually acquire a new image each frame, and annotate the image by drawing
 * a circle on it, and show it on the FRC Dashboard.
 */
class AxisCameraSample : public SampleRobot
{
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;
	AxisCamera *camera;

public:
	void RobotInit() override {
	    // create an image
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);

		// open the camera at the IP address assigned. This is the IP address that the camera
		// can be accessed through the web interface.
		camera = new AxisCamera("10.1.91.103");
	}

	void OperatorControl() override {
        // grab an image, draw the circle, and provide it for the camera server which will
        // in turn send it to the dashboard.
		while(IsOperatorControl() && IsEnabled()) {
			camera->GetImage(frame);
			imaqDrawShapeOnImage(frame, frame, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);
			CameraServer::GetInstance()->SetImage(frame);
			Wait(0.05);
		}
	}
};

START_ROBOT_CLASS(AxisCameraSample);

