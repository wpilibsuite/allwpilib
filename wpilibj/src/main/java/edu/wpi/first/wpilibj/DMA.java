package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DMAJNI;

public class DMA implements AutoCloseable {
  int dmaHandle; // Package private on purpose

  public DMA() {
    dmaHandle = DMAJNI.initialize();
  }

  @Override
  public void close() {
    DMAJNI.free(dmaHandle);
  }

  public void setPause(boolean pause) {
    DMAJNI.setPause(dmaHandle, pause);
  }

  public void setRate(int cycles) {
    DMAJNI.setRate(dmaHandle, cycles);
  }

  public void addEncoder(Encoder encoder) {
    DMAJNI.addEncoder(dmaHandle, encoder.getHandle());
  }

  public void addEncoderRate(Encoder encoder) {
    DMAJNI.addEncoderRate(dmaHandle, encoder.getHandle());
  }

  public void addCounter(Counter counter) {
    DMAJNI.addCounter(dmaHandle, counter.getHandle());
  }

  public void addCounterRate(Counter counter) {
    DMAJNI.addCounterRate(dmaHandle, counter.getHandle());
  }

  public void addDigitalSource(DigitalSource source) {
    DMAJNI.addDigitalSource(dmaHandle, source.getHandle());
  }

  public void addAnalogInput(AnalogInput analog) {
    DMAJNI.addAnalogInput(dmaHandle, analog.getHandle());
  }

  public void addAveragedAnalogInput(AnalogInput analog) {
    DMAJNI.addAveragedAnalogInput(dmaHandle, analog.getHandle());
  }

  public void addAnalogAccumulator(AnalogInput analog) {
    DMAJNI.addAnalogAccumulator(dmaHandle, analog.getHandle());
  }

  public void setExternalTrigger(DigitalSource source, boolean rising, boolean falling) {
    DMAJNI.setExternalTrigger(dmaHandle, source.getPortHandleForRouting(), source.getAnalogTriggerTypeForRouting(), rising, falling);
  }

  public void startDMA(int queueDepth) {
    DMAJNI.startDMA(dmaHandle, queueDepth);
  }

  public void stopDMA() {
    DMAJNI.stopDMA(dmaHandle);
  }
}
