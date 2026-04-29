#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import cscore
from cscore import CameraServer
import cv2
import numpy


def main():
    # Create an HTTP camera. The address will need to be modified to have the
    # correct team number. The exact path will depend on the source.
    camera = cscore.HttpCamera("HTTP Camera", "http://10.x.y.11/video/stream.mjpg")
    # Start capturing images
    CameraServer.startAutomaticCapture(camera)
    # Set the resolution
    camera.setResolution(640, 480)

    # Get a CvSink. This will capture Mats from the camera
    cvSink = CameraServer.getVideo()

    # Setup a CvSource. This will send images back to the Dashboard
    outputStream = CameraServer.putVideo("Rectangle", 640, 480)

    # Mats are very memory expensive. Lets reuse this Mat.
    mat = numpy.zeros((480, 640, 3), dtype="uint8")

    while True:
        # Tell the CvSink to grab a frame from the camera and put it
        # in the source mat.  If there is an error notify the output.
        if cvSink.grabFrame(mat) == 0:
            # Send the output the error.
            outputStream.notifyError(cvSink.getError())
            # skip the rest of the current iteration
            continue

        # Put a rectangle on the image
        cv2.rectangle(mat, (100, 100), (400, 400), (255, 255, 255), 5)

        # Give the output stream a new image to display
        outputStream.putFrame(mat)
