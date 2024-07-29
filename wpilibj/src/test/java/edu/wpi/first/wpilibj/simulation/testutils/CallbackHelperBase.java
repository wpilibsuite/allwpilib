// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.testutils;

import edu.wpi.first.hal.simulation.NotifyCallback;

public abstract class CallbackHelperBase<T> implements NotifyCallback {
  protected boolean m_wasTriggered;
  protected T m_setValue;

  public final boolean wasTriggered() {
    return m_wasTriggered;
  }

  public final T getSetValue() {
    return m_setValue;
  }

  public final void reset() {
    m_wasTriggered = false;
    m_setValue = null;
  }
}
