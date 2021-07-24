// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

/** Structure for holding the result of a CAN Status request. */
@SuppressWarnings("MemberName")
public class CANStatus {
  /** The utilization of the CAN Bus. */
  public double percentBusUtilization;

  /** The CAN Bus off count. */
  public int busOffCount;

  /** The CAN Bus TX full count. */
  public int txFullCount;

  /** The CAN Bus receive error count. */
  public int receiveErrorCount;

  /** The CAN Bus transmit error count. */
  public int transmitErrorCount;

  /**
   * Set CAN bus status.
   *
   * @param percentBusUtilization CAN bus utilization as a percent.
   * @param busOffCount Bus off event count.
   * @param txFullCount TX buffer full event count.
   * @param receiveErrorCount Receive error event count.
   * @param transmitErrorCount Transmit error event count.
   */
  public void setStatus(
      double percentBusUtilization,
      int busOffCount,
      int txFullCount,
      int receiveErrorCount,
      int transmitErrorCount) {
    this.percentBusUtilization = percentBusUtilization;
    this.busOffCount = busOffCount;
    this.txFullCount = txFullCount;
    this.receiveErrorCount = receiveErrorCount;
    this.transmitErrorCount = transmitErrorCount;
  }
}
