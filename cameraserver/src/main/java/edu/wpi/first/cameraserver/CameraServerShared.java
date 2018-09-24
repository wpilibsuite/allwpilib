/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.cameraserver;


public interface CameraServerShared {
  /**
   * get the main thread id func.
   *
   * @return the robotMainThreadId
   */
  Long getRobotMainThreadId();

  /**
   * Report an error to the driver station.
   *
   * @param error the error to set
   */
  void reportDriverStationError(String error);

  /**
   * Report an video server usage.
   *
   * @param id the usage id
   */
  void reportVideoServer(int id);

  /**
   * Report a usb camera usage.
   *
   * @param id the usage id
   */
  void reportUsbCamera(int id);

  /**
   * Report an axis camera usage.
   *
   * @param id the usage id
   */
  void reportAxisCamera(int id);
}
