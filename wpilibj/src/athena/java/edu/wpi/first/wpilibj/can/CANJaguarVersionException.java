/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

/**
 * Exception indicating that the CAN driver layer has not been initialized. This
 * happens when an entry-point is called before a CAN driver plugin has been
 * installed.
 */
public class CANJaguarVersionException extends RuntimeException {

  public static final int kMinLegalFIRSTFirmwareVersion = 101;
  // 3330 was the first shipping RDK firmware version for the Jaguar
  public static final int kMinRDKFirmwareVersion = 3330;

  public CANJaguarVersionException(int deviceNumber, int fwVersion) {
    super(getString(deviceNumber, fwVersion));
    System.out.println("fwVersion[" + deviceNumber + "]: " + fwVersion);
  }

  static String getString(int deviceNumber, int fwVersion) {
    String msg;
    if (fwVersion < kMinRDKFirmwareVersion) {
      msg =
          "Jaguar " + deviceNumber
              + " firmware is too old.  It must be updated to at least version "
              + Integer.toString(kMinLegalFIRSTFirmwareVersion)
              + " of the FIRST approved firmware!";
    } else {
      msg =
          "Jaguar " + deviceNumber
              + " firmware is not FIRST approved.  It must be updated to at least version "
              + Integer.toString(kMinLegalFIRSTFirmwareVersion)
              + " of the FIRST approved firmware!";
    }
    return msg;
  }
}
