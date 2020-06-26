/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

public interface I2CDevice {
  boolean initialize();
  int transaction(byte[] dataToSend, int sendSize, byte[] dataReceived, int receiveSize);
  int write(byte[] dataToSend, int sendSize);
  int read(byte[] buffer, int count);
}
