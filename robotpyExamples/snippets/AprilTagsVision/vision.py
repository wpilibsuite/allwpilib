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
    detector.add_family("tag36h11", 1)

    # Set up Pose Estimator - parameters are for a Microsoft Lifecam HD-3000
    # (https://www.chiefdelphi.com/t/wpilib-apriltagdetector-sample-code/421411/21)
    pose_est_config = robotpy_apriltag.AprilTagPoseEstimator.Config(
        0.1651,
        699.3778103158814,
        677.7161226393544,
        345.6059345433618,
        207.12741326228522,
    )
    estimator = robotpy_apriltag.AprilTagPoseEstimator(pose_est_config)

    # Get the UsbCamera from CameraServer
    camera = CameraServer.start_automatic_capture()

    # Set the resolution
    camera.set_resolution(640, 480)

    # Get a CvSink. This will capture Mats from the camera
    cv_sink = CameraServer.get_video()

    # Set up a CvSource. This will send images back to the Dashboard
    output_stream = CameraServer.put_video("Detected", 640, 480)

    # Mats are very memory expensive. Let's reuse these.
    mat = np.zeros((480, 640, 3), dtype=np.uint8)
    gray_mat = np.zeros(shape=(480, 640), dtype=np.uint8)

    # Instantiate once
    tags = []
    outline_color = (0, 255, 0)
    cross_color = (0, 0, 255)

    # Output the list to Network Tables
    tags_table = ntcore.NetworkTableInstance.get_default().get_table("apriltags")
    pub_tags = tags_table.get_integer_array_topic("tags").publish()

    try:
        while True:
            # Tell the CvSink to grab a frame from the camera and put it
            # in the source mat.  If there is an error notify the output.
            if cv_sink.grab_frame(mat) == 0:
                # Send the output frame the error
                output_stream.notify_error(cv_sink.get_error())

                # Skip the rest of the current iteration
                continue

            cv2.cvtColor(mat, cv2.COLOR_RGB2GRAY, dst=gray_mat)

            detections = detector.detect(gray_mat)

            # have not seen any tags yet
            tags.clear()

            for detection in detections:
                # remember we saw this tag
                tags.append(detection.get_id())

                # draw lines around the tag
                for i in range(4):
                    j = (i + 1) % 4
                    point1 = (
                        int(detection.get_corner(i).x),
                        int(detection.get_corner(i).y),
                    )
                    point2 = (
                        int(detection.get_corner(j).x),
                        int(detection.get_corner(j).y),
                    )
                    mat = cv2.line(mat, point1, point2, outline_color, 2)

                # mark the center of the tag
                cx = int(detection.get_center().x)
                cy = int(detection.get_center().y)
                ll = 10
                mat = cv2.line(
                    mat,
                    (cx - ll, cy),
                    (cx + ll, cy),
                    cross_color,
                    2,
                )
                mat = cv2.line(
                    mat,
                    (cx, cy - ll),
                    (cx, cy + ll),
                    cross_color,
                    2,
                )

                # identify the tag
                mat = cv2.putText(
                    mat,
                    str(detection.get_id()),
                    (cx + ll, cy),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    cross_color,
                    3,
                )

                # determine pose
                pose = estimator.estimate(detection)

                # put pose into dashboard
                rot = pose.rotation()
                tags_table.get_entry(f"pose_{detection.get_id()}").set_double_array(
                    [pose.x(), pose.y(), pose.z(), rot.x(), rot.y(), rot.z()]
                )

            # put list of tags onto dashboard
            pub_tags.set(tags)

            # Give output stream a new image to display
            output_stream.put_frame(mat)
    finally:
        pub_tags.close()
        detector.close()

    # The camera code will be killed when the robot.py program exits. If you wish to perform cleanup,
    # you should register an atexit handler. The child process will NOT be launched when running the robot code in
    # simulation or unit testing mode
