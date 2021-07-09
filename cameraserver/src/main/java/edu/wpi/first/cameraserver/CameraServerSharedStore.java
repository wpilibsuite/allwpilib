// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraserver;

public final class CameraServerSharedStore {
  private static CameraServerShared cameraServerShared;

  private CameraServerSharedStore() {}

  /**
   * Get the CameraServerShared object.
   *
   * @return The CameraServerSharedObject
   */
  public static synchronized CameraServerShared getCameraServerShared() {
    if (cameraServerShared == null) {
      cameraServerShared =
          new CameraServerShared() {
            @Override
            public void reportVideoServer(int id) {}

            @Override
            public void reportUsbCamera(int id) {}

            @Override
            public void reportDriverStationError(String error) {}

            @Override
            public void reportAxisCamera(int id) {}

            @Override
            public Long getRobotMainThreadId() {
              return null;
            }
          };
    }
    return cameraServerShared;
  }

  /**
   * Set the CameraServerShared object.
   *
   * @param shared The CameraServerShared object.
   */
  public static synchronized void setCameraServerShared(CameraServerShared shared) {
    cameraServerShared = shared;
  }
}
