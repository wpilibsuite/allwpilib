package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AccumulatorResult;
import edu.wpi.first.hal.DMAJNISample;

public class DMASample {
  private DMAJNISample dmaSample = new DMAJNISample();

  public void update(DMA dma, int timeoutMs) {
    dmaSample.update(dma.dmaHandle, timeoutMs);
  }

  public long getTime() {
    return dmaSample.getTime();
  }

  public double getTimeStamp() {
    return (double)getTime() * 0.0000001;
  }

  public int getEncoder(Encoder encoder) {
    return dmaSample.getEncoder(encoder.getHandle());
  }

  public int getCounter(Counter counter) {
    return dmaSample.getCounter(counter.getHandle());
  }

  public int getEncoderRate(Encoder encoder) {
    return dmaSample.getEncoderRate(encoder.getHandle());
  }

  public int getCounterRate(Counter counter) {
    return dmaSample.getCounterRate(counter.getHandle());
  }

  public boolean getDigitalSource(DigitalSource source) {
    return dmaSample.getDigitalSource(source.getHandle());
  }

  public int getAnalogInput(AnalogInput analog) {
    return dmaSample.getAnalogInput(analog.getHandle());
  }

  public int getAnalogInputAveraged(AnalogInput analog) {
    return dmaSample.getAnalogInputAveraged(analog.getHandle());
  }

  public void getAnalogAccumulator(AnalogInput analog, AccumulatorResult result) {
    dmaSample.getAnalogAccumulator(analog.getHandle(), result);
  }
}
