package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DutyCycleJNI;

public class DutyCycle implements AutoClosable {
  private final int m_handle;

  public DutyCycle(DigitalSource digitalSource) {
    m_handle = DutyCycleJNI.initialize(digitalSource.getPortHandleForRouting(),
                            digitalSource.getAnalogTriggerTypeForRouting());
  }

  @Override
  public void close() {
    DutyCycleJNI.free(m_handle);
  }

  public int getFrequency() {
    return DutyCycleJNI.getFrequency(m_handle);
  }

  public int getOutputRaw() {
    return DutyCycleJNI.getOutputRaw(m_handle);
  }

  public int getOutputScaled() {
    return DutyCycleJNI.getOutputScaled(m_handle);
  }
}
