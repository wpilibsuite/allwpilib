/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                            */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

import java.util.HashMap;
import java.util.Map;

public class DMASample {
  private static final int kEnable_AI0_Low = 0;
  private static final int kEnable_AI0_High = 1;
  private static final int kEnable_AIAveraged0_Low = 2;
  private static final int kEnable_AIAveraged0_High = 3;
  private static final int kEnable_AI1_Low = 4;
  private static final int kEnable_AI1_High = 5;
  private static final int kEnable_AIAveraged1_Low = 6;
  private static final int kEnable_AIAveraged1_High = 7;
  private static final int kEnable_Accumulator0 = 8;
  private static final int kEnable_Accumulator1 = 9;
  private static final int kEnable_DI = 10;
  private static final int kEnable_AnalogTriggers = 11;
  private static final int kEnable_Counters_Low = 12;
  private static final int kEnable_Counters_High = 13;
  private static final int kEnable_CounterTimers_Low = 14;
  private static final int kEnable_CounterTimers_High = 15;
  private static final int kEnable_Encoders_Low = 16;
  private static final int kEnable_Encoders_High = 17;
  private static final int kEnable_EncoderTimers_Low = 18;
  private static final int kEnable_EncoderTimers_High = 19;

  private static class BaseStore {
    public double scaleData;

    public int valueType;
    public int index;
  }

  private final int[] dataBuffer = new int[100];
  private long timeStamp;
  private Map<Integer, BaseStore> propertyMap = new HashMap<>();

  public void update(int dmaHandle) {
    timeStamp = DMAJNI.readDMA(dmaHandle, dataBuffer);
  }

  public long getTime() {
    return timeStamp;
  }

  private BaseStore addEncoderInternal(int encoderHandle) {
    return null;
  }

  private BaseStore addCounterInternal(int encoderHandle) {
    return null;
  }

  private BaseStore addDigitalSourceInternal(int encoderHandle) {
    return null;
  }

  private BaseStore addAnalogInputInternal(int encoderHandle) {
    return null;
  }

  // 0-63: readBuffer
  // 64-83 channelOffsets
  // 84: capture size
  // 85: triggerChannels (bitflags)

  private int readValue(int valueType, int index) {
    int offset = dataBuffer[64 + valueType];
    if (offset == -1) {
      throw new RuntimeException("Resource not found in DMA capture");
    }
    return dataBuffer[offset + index];
  }

  public int getEncoder(int encoderHandle) {
    BaseStore data = propertyMap.get(encoderHandle);
    if (data == null) {
      data = addEncoderInternal(encoderHandle);
    }
    return readValue(data.valueType, data.index);
  }


  public int getEncoderRate(int encoderHandle) {
    BaseStore data = propertyMap.get(encoderHandle);
    if (data == null) {
      data = addEncoderInternal(encoderHandle);
    }
      // + 2 Hack, but needed to not have to call into JNI
    return readValue(data.valueType + 2, data.index);
  }


  public int getCounterEncoder(int encoderHandle) {
    BaseStore data = propertyMap.get(encoderHandle);
    if (data == null) {
      data = addCounterInternal(encoderHandle);
    }
    return readValue(data.valueType, data.index);
  }


  public int getCounterRate(int encoderHandle) {
    BaseStore data = propertyMap.get(encoderHandle);
    if (data == null) {
      data = addCounterInternal(encoderHandle);
    }
    // Hack, but needed to not have to call into JNI
    return readValue(data.valueType + 2, data.index);
  }

  public boolean getDigitalSource(int digitalSourceHandle) {
    BaseStore data = propertyMap.get(digitalSourceHandle);
    if (data == null) {
      data = addDigitalSourceInternal(digitalSourceHandle);
    }

    int value = readValue(data.valueType, 0);

    return ((value >> data.index) & 0x1) != 0;
  }

  public int getAnalogInput(int analogInputHandle) {
    BaseStore data = propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addAnalogInputInternal(analogInputHandle);
    }

    int value = readValue(data.valueType, data.index / 2);
    if ((data.index % 2) != 0) {
      return (value >>> 16) & 0xFFFF;
    } else {
      return value & 0xFFFF;
    }
  }

  public int getAnalogInputAveraged(int analogInputHandle) {
    BaseStore data = propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addAnalogInputInternal(analogInputHandle);
    }

    // + 2 Hack, but needed to not have to call into JNI
    int value = readValue(data.valueType + 2, data.index);
    return value;
  }

  public int getAnalogAccumulator(int analogInputHandle) {
    BaseStore data = propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addAnalogInputInternal(analogInputHandle);
    }

    if (data.index == 0) {
      return readValue(kEnable_Accumulator0, 0);
    } else if (data.index == 1) {
      return readValue(kEnable_Accumulator1, 0);
    } else {
      throw new RuntimeException("Resource not found in DMA capture");
    }
  }
}
