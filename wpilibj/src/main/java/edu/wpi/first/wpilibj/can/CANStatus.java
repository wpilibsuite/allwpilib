/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

/**
 * Structure for holding the result of a CAN Status request.
 */
public class CANStatus {
  /**
   * The utilization of the CAN Bus.
   */
  @SuppressWarnings("MemberName")
  public double percentBusUtilization;

  /**
   * The CAN Bus off count.
   */
  @SuppressWarnings("MemberName")
  public int busOffCount;

  /**
   * The CAN Bus TX full count.
   */
  @SuppressWarnings("MemberName")
  public int txFullCount;

  /**
   * The CAN Bus receive error count.
   */
  @SuppressWarnings("MemberName")
  public int receiveErrorCount;

  /**
   * The CAN Bus transmit error count.
   */
  @SuppressWarnings("MemberName")
  public int transmitErrorCount;

  @SuppressWarnings("JavadocMethod")
  public void setStatus(double percentBusUtilization, int busOffCount, int txFullCount,
                        int receiveErrorCount, int transmitErrorCount) {
    this.percentBusUtilization = percentBusUtilization;
    this.busOffCount = busOffCount;
    this.txFullCount = txFullCount;
    this.receiveErrorCount = receiveErrorCount;
    this.transmitErrorCount = transmitErrorCount;
  }
}
