#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

"""
This is a demo program showing the use of OpenCV to do vision processing. The image is acquired
from an HTTP camera, then a rectangle is put on the image and sent to the dashboard. OpenCV has
many methods for different types of processing.
"""

import numpy
import cscore
from cscore import CameraServer
import cv2

#
# This code will work both on a RoboRIO and on other platforms. The exact mechanism
# to run it differs depending on whether you’re on a RoboRIO or a coprocessor
#
# https://robotpy.readthedocs.io/en/stable/vision/code.html


def main():
    # Create an HTTP camera. The address will need to be modified to have the
    # correct team number. The exact path will depend on the source.
    camera = cscore.HttpCamera("HTTP Camera", "http://10.x.y.11/video/stream.mjpg")
    # Start capturing images
    CameraServer.start_automatic_capture(camera)
    # Set the resolution
    camera.set_resolution(640, 480)

    # Get a CvSink. This will capture Mats from the camera
    cv_sink = CameraServer.get_video()

    # Setup a CvSource. This will send images back to the Dashboard
    output_stream = CameraServer.put_video("Rectangle", 640, 480)

    # Mats are very memory expensive. Lets reuse this Mat.
    mat = numpy.zeros((480, 640, 3), dtype="uint8")

    # Declare the color of the rectangle
    rect_color = (255, 255, 255)

    # The camera code will be killed when the robot.py program exits. If you wish to perform cleanup,
    # you should register an atexit handler. The child process will NOT be launched when running the robot code in
    # simulation or unit testing mode

    while True:
        # Tell the CvSink to grab a frame from the camera and put it in the source mat.  If there is an error notify the
        # output.

        if cv_sink.grab_frame(mat) == 0:
            # Send the output the error.
            output_stream.notify_error(cv_sink.get_error())

            # skip the rest of the current iteration
            continue

        # Put a rectangle on the image
        mat = cv2.rectangle(
            img=mat,
            pt1=(100, 100),
            pt2=(400, 400),
            color=rect_color,
            lineType=5,
        )

        # Give the output stream a new image to display
        output_stream.put_frame(mat)
