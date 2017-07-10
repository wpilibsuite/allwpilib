package edu.wpi.first.wpilibj.examples.vision.quickvision;

import edu.wpi.first.wpilibj.CameraServer;
import edu.wpi.first.wpilibj.IterativeRobot;

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam
 * and send it to the FRC dashboard without doing any vision processing. This
 * is the easiest way to get camera images to the dashboard. Just add this to the
 * robotInit() method in your program.
 */
public class Robot extends IterativeRobot {
	@Override
	public void robotInit() {
		CameraServer.getInstance().startAutomaticCapture();
	}

}
