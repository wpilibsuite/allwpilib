/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Base class for all sensors. Stores most recent status information as well as containing utility
 * functions for checking channels and error processing.
 * @deprecated Use Sendable and SendableRegistry
 */
@Deprecated(since = "2020", forRemoval = true)
public abstract class SendableBase implements Sendable, AutoCloseable {
  /**
   * Creates an instance of the sensor base.
   */
  public SendableBase() {
    this(true);
  }

  /**
   * Creates an instance of the sensor base.
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  public SendableBase(boolean addLiveWindow) {
    if (addLiveWindow) {
      SendableRegistry.addLW(this, "");
    } else {
      SendableRegistry.add(this, "");
    }
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }
}
