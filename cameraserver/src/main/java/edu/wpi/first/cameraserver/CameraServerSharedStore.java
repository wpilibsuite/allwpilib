/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.cameraserver;

public final class CameraServerSharedStore {
  private static CameraServerShared cameraServerShared;

  private CameraServerSharedStore() {
  }

  /**
   * get the CameraServerShared object.
   */
  public static synchronized CameraServerShared getCameraServerShared() {
    if (cameraServerShared == null) {
      cameraServerShared = new CameraServerShared() {

        @Override
        public void reportVideoServer(int id) {

        }

        @Override
        public void reportUsbCamera(int id) {

        }

        @Override
        public void reportDriverStationError(String error) {

        }

        @Override
        public void reportAxisCamera(int id) {

        }

        @Override
        public Long getRobotMainThreadId() {
          return null;
        }
      };
    }
    return cameraServerShared;
  }

  /**
   * set the CameraServerShared object.
   */
  public static synchronized void setCameraServerShared(CameraServerShared shared) {
    cameraServerShared = shared;
  }
}
