// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.util.HashMap;
import java.util.Map;

public class DMAJNISample {
  private static final int kEnable_Accumulator0 = 8;
  private static final int kEnable_Accumulator1 = 9;

  static class BaseStore {
    public final int m_valueType;
    public final int m_index;

    BaseStore(int valueType, int index) {
      this.m_valueType = valueType;
      this.m_index = index;
    }
  }

  private final int[] m_dataBuffer = new int[100];
  private final int[] m_storage = new int[100];
  private long m_timeStamp;
  private Map<Integer, BaseStore> m_propertyMap = new HashMap<>();

  public int update(int dmaHandle, double timeoutSeconds) {
    m_timeStamp = DMAJNI.readDMA(dmaHandle, timeoutSeconds, m_dataBuffer, m_storage);
    return m_storage[25];
  }

  public int getCaptureSize() {
    return m_storage[22];
  }

  public int getTriggerChannels() {
    return m_storage[23];
  }

  public int getRemaining() {
    return m_storage[24];
  }

  public long getTime() {
    return m_timeStamp;
  }

  private BaseStore addSensorInternal(int handle) {
    BaseStore sensorData = DMAJNI.getSensorReadData(handle);
    m_propertyMap.put(handle, sensorData);
    return sensorData;
  }

  public void addSensor(int handle) {
    addSensorInternal(handle);
  }

  private int readValue(int valueType, int index) {
    int offset = m_storage[valueType];
    if (offset == -1) {
      throw new RuntimeException("Resource not found in DMA capture");
    }
    return m_dataBuffer[offset + index];
  }

  public int getEncoder(int encoderHandle) {
    BaseStore data = m_propertyMap.get(encoderHandle);
    if (data == null) {
      data = addSensorInternal(encoderHandle);
    }
    return readValue(data.m_valueType, data.m_index);
  }

  public int getEncoderPeriod(int encoderHandle) {
    BaseStore data = m_propertyMap.get(encoderHandle);
    if (data == null) {
      data = addSensorInternal(encoderHandle);
    }
    // + 2 Hack, but needed to not have to call into JNI
    return readValue(data.m_valueType + 2, data.m_index);
  }

  public int getCounter(int counterHandle) {
    BaseStore data = m_propertyMap.get(counterHandle);
    if (data == null) {
      data = addSensorInternal(counterHandle);
    }
    return readValue(data.m_valueType, data.m_index);
  }

  public int getCounterPeriod(int counterHandle) {
    BaseStore data = m_propertyMap.get(counterHandle);
    if (data == null) {
      data = addSensorInternal(counterHandle);
    }
    // Hack, but needed to not have to call into JNI
    return readValue(data.m_valueType + 2, data.m_index);
  }

  public boolean getDigitalSource(int digitalSourceHandle) {
    BaseStore data = m_propertyMap.get(digitalSourceHandle);
    if (data == null) {
      data = addSensorInternal(digitalSourceHandle);
    }

    int value = readValue(data.m_valueType, 0);

    return ((value >> data.m_index) & 0x1) != 0;
  }

  public int getAnalogInput(int analogInputHandle) {
    BaseStore data = m_propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addSensorInternal(analogInputHandle);
    }

    int value = readValue(data.m_valueType, data.m_index / 2);
    if ((data.m_index % 2) != 0) {
      return (value >>> 16) & 0xFFFF;
    } else {
      return value & 0xFFFF;
    }
  }

  public int getAnalogInputAveraged(int analogInputHandle) {
    BaseStore data = m_propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addSensorInternal(analogInputHandle);
    }

    // + 2 Hack, but needed to not have to call into JNI
    int value = readValue(data.m_valueType + 2, data.m_index);
    return value;
  }

  public void getAnalogAccumulator(int analogInputHandle, AccumulatorResult result) {
    BaseStore data = m_propertyMap.get(analogInputHandle);
    if (data == null) {
      data = addSensorInternal(analogInputHandle);
    }

    if (data.m_index == 0) {
      int val0 = readValue(kEnable_Accumulator0, 0);
      int val1 = readValue(kEnable_Accumulator0, 1);
      int val2 = readValue(kEnable_Accumulator0, 2);
      result.count = val2;
      result.value = ((long) val1 << 32) | val0;
    } else if (data.m_index == 1) {
      int val0 = readValue(kEnable_Accumulator1, 0);
      int val1 = readValue(kEnable_Accumulator1, 1);
      int val2 = readValue(kEnable_Accumulator1, 2);
      result.count = val2;
      result.value = ((long) val1 << 32) | val0;
    } else {
      throw new RuntimeException("Resource not found in DMA capture");
    }
  }

  public int getDutyCycleOutput(int dutyCycleHandle) {
    BaseStore data = m_propertyMap.get(dutyCycleHandle);
    if (data == null) {
      data = addSensorInternal(dutyCycleHandle);
    }
    return readValue(data.m_valueType, data.m_index);
  }
}
