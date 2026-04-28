#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#


import ntcore
import robotpy_apriltag
from cscore import CameraServer

import cv2
import numpy as np

#
# This code will work both on a RoboRIO and on other platforms. The exact mechanism
# to run it differs depending on whether you’re on a RoboRIO or a coprocessor
#
# https://robotpy.readthedocs.io/en/stable/vision/code.html


def main():
    detector = robotpy_apriltag.AprilTagDetector()

    # look for tag36h11, correct 1 error bit (hamming distance 1)
    # hamming 1 allocates 781KB, 2 allocates 27.4 MB, 3 allocates 932 MB
    # max of 1 recommended for RoboRIO 1, while hamming 2 is feasible on the RoboRIO 2
    detector.addFamily("tag36h11", 1)

    # Set up Pose Estimator - parameters are for a Microsoft Lifecam HD-3000
    # (https://www.chiefdelphi.com/t/wpilib-apriltagdetector-sample-code/421411/21)
    poseEstConfig = robotpy_apriltag.AprilTagPoseEstimator.Config(
        0.1651,
        699.3778103158814,
        677.7161226393544,
        345.6059345433618,
        207.12741326228522,
    )
    estimator = robotpy_apriltag.AprilTagPoseEstimator(poseEstConfig)

    # Get the UsbCamera from CameraServer
    camera = CameraServer.startAutomaticCapture()

    # Set the resolution
    camera.setResolution(640, 480)

    # Get a CvSink. This will capture Mats from the camera
    cvSink = CameraServer.getVideo()

    # Set up a CvSource. This will send images back to the Dashboard
    outputStream = CameraServer.putVideo("Detected", 640, 480)

    # Mats are very memory expensive. Let's reuse these.
    mat = np.zeros((480, 640, 3), dtype=np.uint8)
    grayMat = np.zeros(shape=(480, 640), dtype=np.uint8)

    # Instantiate once
    tags = []
    outlineColor = (0, 255, 0)
    crossColor = (0, 0, 255)

    # Output the list to Network Tables
    tagsTable = ntcore.NetworkTableInstance.getDefault().getTable("apriltags")
    pubTags = tagsTable.getIntegerArrayTopic("tags").publish()

    try:
        while True:
            # Tell the CvSink to grab a frame from the camera and put it
            # in the source mat.  If there is an error notify the output.
            if cvSink.grabFrame(mat) == 0:
                # Send the output frame the error
                outputStream.notifyError(cvSink.getError())

                # Skip the rest of the current iteration
                continue

            cv2.cvtColor(mat, cv2.COLOR_RGB2GRAY, dst=grayMat)

            detections = detector.detect(grayMat)

            # have not seen any tags yet
            tags.clear()

            for detection in detections:
                # remember we saw this tag
                tags.append(detection.getId())

                # draw lines around the tag
                for i in range(4):
                    j = (i + 1) % 4
                    point1 = (
                        int(detection.getCorner(i).x),
                        int(detection.getCorner(i).y),
                    )
                    point2 = (
                        int(detection.getCorner(j).x),
                        int(detection.getCorner(j).y),
                    )
                    mat = cv2.line(mat, point1, point2, outlineColor, 2)

                # mark the center of the tag
                cx = int(detection.getCenter().x)
                cy = int(detection.getCenter().y)
                ll = 10
                mat = cv2.line(
                    mat,
                    (cx - ll, cy),
                    (cx + ll, cy),
                    crossColor,
                    2,
                )
                mat = cv2.line(
                    mat,
                    (cx, cy - ll),
                    (cx, cy + ll),
                    crossColor,
                    2,
                )

                # identify the tag
                mat = cv2.putText(
                    mat,
                    str(detection.getId()),
                    (cx + ll, cy),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    crossColor,
                    3,
                )

                # determine pose
                pose = estimator.estimate(detection)

                # put pose into dashboard
                rot = pose.rotation()
                tagsTable.getEntry(f"pose_{detection.getId()}").setDoubleArray(
                    [pose.X(), pose.Y(), pose.Z(), rot.X(), rot.Y(), rot.Z()]
                )

            # put list of tags onto dashboard
            pubTags.set(tags)

            # Give output stream a new image to display
            outputStream.putFrame(mat)
    finally:
        pubTags.close()
        detector.close()

    # The camera code will be killed when the robot.py program exits. If you wish to perform cleanup,
    # you should register an atexit handler. The child process will NOT be launched when running the robot code in
    # simulation or unit testing mode
