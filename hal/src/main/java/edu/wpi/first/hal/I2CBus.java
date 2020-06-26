/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

public interface I2CBus {
  boolean openBus();
  void closeBus();

  boolean initialize(int deviceAddress);
  int transaction(int deviceAddress, byte[] dataToSend, int sendSize, byte[] dataReceived,
                  int receiveSize);
  int write(int deviceAddress, byte[] dataToSend, int sendSize);
  int read(int deviceAddress, byte[] buffer, int count);
}
