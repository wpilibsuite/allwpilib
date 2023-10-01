// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.communication;

/** Structure for holding the result of a network usage request. */
@SuppressWarnings("MemberName")
public class NetworkStatus {
  /** Amount of bytes received over the network interface. */
  public int rxBytes;

  /** Amount of bytes transmitted over the network interface. */
  public int txBytes;

  /**
   * Set network interface status.
   *
   * @param rxBytes Amount of bytes received over the network interface.
   * @param txBytes Amount of bytes transmitted over the network interface.
   */
  public void setStatus(int rxBytes, int txBytes) {
    this.rxBytes = rxBytes;
    this.txBytes = txBytes;
  }
}
