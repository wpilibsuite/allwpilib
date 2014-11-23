#include "WPILib.h"

/**
 * Uses IMAQdx to manually acquire a new image each frame, and annotate the image by drawing
 * a circle on it, and show it on the FRC Dashboard.
 */
class IntermediateVisionRobot : public SampleRobot
{
	IMAQdxSession session;
	Image *frame;

public:
	void RobotInit() override {
	    // create an image
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		// open the camera
		IMAQdxOpenCamera("cam1", IMAQdxCameraControlModeController, &session);
		IMAQdxConfigureGrab(session);
	}

	void OperatorControl() override {
	    // acquire images
		IMAQdxStartAcquisition(session);

        // grab an image, draw the circle, and provide it for the camera server which will
        // in turn send it to the dashboard.
		while(IsOperatorControl() && IsEnabled()) {
			IMAQdxGrab(session, frame, true, NULL);

			imaqDrawShapeOnImage(frame, frame, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);

			CameraServer::GetInstance()->SetImage(frame);
		}
        // stop image acquisition
		IMAQdxStopAcquisition(session);
	}
};

START_ROBOT_CLASS(IntermediateVisionRobot);

