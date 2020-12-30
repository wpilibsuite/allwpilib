// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

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

  @SuppressWarnings("MissingJavadocMethod")
  public void setStatus(double percentBusUtilization, int busOffCount, int txFullCount,
                        int receiveErrorCount, int transmitErrorCount) {
    this.percentBusUtilization = percentBusUtilization;
    this.busOffCount = busOffCount;
    this.txFullCount = txFullCount;
    this.receiveErrorCount = receiveErrorCount;
    this.transmitErrorCount = transmitErrorCount;
  }
}
