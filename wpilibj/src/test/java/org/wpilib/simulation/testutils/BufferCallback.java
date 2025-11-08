// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.testutils;

import edu.wpi.first.hal.simulation.ConstBufferCallback;
import java.util.Arrays;

public class BufferCallback implements ConstBufferCallback {
  private boolean m_wasTriggered;
  private byte[] m_setValue;

  @Override
  public void callback(String name, byte[] buffer, int count) {
    m_wasTriggered = true;
    m_setValue = Arrays.copyOf(buffer, buffer.length);
  }

  public boolean wasTriggered() {
    return m_wasTriggered;
  }

  public byte[] getSetValue() {
    return Arrays.copyOf(m_setValue, m_setValue.length);
  }
}
