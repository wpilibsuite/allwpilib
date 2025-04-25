// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraserver;

/** Storage for CameraServerShared instance. */
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
            public void reportUsage(String resource, String data) {}

            @Override
            public void reportDriverStationError(String error) {}

            @Override
            public Long getRobotMainThreadId() {
              return null;
            }
          };
    }
    return cameraServerShared;
  }

  /**
   * Report usage.
   *
   * @param resource the resource name
   * @param data arbitrary string data
   */
  public static void reportUsage(String resource, String data) {
    getCameraServerShared().reportUsage(resource, data);
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
