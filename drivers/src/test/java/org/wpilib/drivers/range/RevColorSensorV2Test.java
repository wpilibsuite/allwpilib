// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.range;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.Milliseconds;
import static org.wpilib.units.Units.Seconds;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.drivers.range.RevColorSensorV2.DeviceStatus;
import org.wpilib.drivers.range.RevColorSensorV2.FailureReason;
import org.wpilib.drivers.range.RevColorSensorV2.Gain;
import org.wpilib.drivers.range.RevColorSensorV2.LedDrive;
import org.wpilib.drivers.range.RevColorSensorV2.Register;
import org.wpilib.hardware.bus.I2C;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.CallbackStore;
import org.wpilib.simulation.I2CSim;

@ResourceLock("I2C")
class RevColorSensorV2Test {
  private static final double DELTA = 1e-9;

  /**
   * Command bit plus the auto-increment command type. The sensor advances through consecutive
   * registers during a multi-byte read only when the command type is auto-increment.
   */
  private static final int COMMAND_AUTO_INCREMENT = 0x80 | (0x01 << 5);

  private static final byte TMD37821_DEVICE_ID = 0x60;

  private static final int STATUS_COLOR_VALID = 0x01;
  private static final int STATUS_PROXIMITY_VALID = 0x02;
  private static final int STATUS_ALL_VALID = STATUS_COLOR_VALID | STATUS_PROXIMITY_VALID;

  /** Default integration time of 24 ms, expressed as ten 2.4 ms cycles. */
  private static final int DEFAULT_ATIME = 246;

  private static final int DEFAULT_MAXIMUM_RAW_COLOR_VALUE = 10240;

  private final I2CSim m_i2cSim = new I2CSim(I2C.Port.PORT_0.value);
  private final Map<Integer, byte[]> m_registerData = new HashMap<>();
  private final List<byte[]> m_writes = new ArrayList<>();
  private final List<Integer> m_readRegisters = new ArrayList<>();
  private final List<Integer> m_readCounts = new ArrayList<>();

  private CallbackStore m_readCallback;
  private CallbackStore m_writeCallback;
  private int m_selectedRegister;

  @BeforeEach
  void setUp() {
    HAL.initialize();
    m_i2cSim.resetData();
    m_readCallback =
        m_i2cSim.registerReadCallback(
            (name, buffer, count) -> {
              m_readRegisters.add(m_selectedRegister);
              m_readCounts.add(count);
              byte[] data = m_registerData.get(m_selectedRegister);
              if (data != null) {
                System.arraycopy(data, 0, buffer, 0, Math.min(count, data.length));
              }
            });
    m_writeCallback =
        m_i2cSim.registerWriteCallback(
            (name, buffer, count) -> {
              if (count == 0) {
                return;
              }
              m_selectedRegister = Byte.toUnsignedInt(buffer[0]);
              if (count > 1) {
                m_writes.add(Arrays.copyOf(buffer, count));
              }
            });
    setRegister(Register.DEVICE_ID, new byte[] {TMD37821_DEVICE_ID});
  }

  @AfterEach
  void tearDown() {
    m_readCallback.close();
    m_writeCallback.close();
    m_i2cSim.resetData();
  }

  @Test
  void usesDefaultAddressAndCloses() {
    var sensor = new RevColorSensorV2(I2C.Port.PORT_0);

    assertEquals(I2C.Port.PORT_0, sensor.getPort());
    assertEquals(RevColorSensorV2.DEFAULT_ADDRESS, sensor.getDeviceAddress());

    sensor.close();
    assertThrows(IllegalStateException.class, sensor::getPort);
    assertThrows(IllegalStateException.class, sensor::update);
    assertThrows(IllegalStateException.class, sensor::getDeviceStatus);
    assertThrows(IllegalStateException.class, sensor::getColor);
    assertThrows(IllegalStateException.class, sensor::getRawProximity);
    assertThrows(IllegalStateException.class, sensor::getDistanceMeters);
    assertThrows(IllegalStateException.class, () -> sensor.setGain(Gain.GAIN_16));

    sensor.close();
  }

  @Test
  void rejectsInvalidI2cAddresses() {
    assertThrows(IllegalArgumentException.class, () -> new RevColorSensorV2(I2C.Port.PORT_0, -1));
    assertThrows(IllegalArgumentException.class, () -> new RevColorSensorV2(I2C.Port.PORT_0, 0x80));
  }

  @Test
  void configuresSensorOnConstruction() {
    setRegister(Register.CONTROL, new byte[] {0x00});

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertEquals(TMD37821_DEVICE_ID, sensor.getDeviceId());
      assertEquals(0, sensor.getFailureCount());
      assertNull(sensor.getLastFailureReason());

      // The integrator is turned off, the gain and timing are written, and the color and
      // proximity channels are enabled after the power on warm-up.
      assertEquals(6, m_writes.size());
      assertWrite(m_writes.get(0), Register.ENABLE, 0x00);
      assertWrite(m_writes.get(1), Register.ATIME, DEFAULT_ATIME);
      assertWrite(m_writes.get(2), Register.CONTROL, 0x61);
      assertWrite(m_writes.get(3), Register.PPULSE, 8);
      assertWrite(m_writes.get(4), Register.ENABLE, 0x01);
      assertWrite(m_writes.get(5), Register.ENABLE, 0x07);
    }
  }

  @Test
  void preservesUnrelatedControlBits() {
    setRegister(Register.CONTROL, new byte[] {0x0C});

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.setGain(Gain.GAIN_64);
      sensor.setLedDrive(LedDrive.PERCENT_12_5);

      // Bits 3:2 are untouched, bit 5 selects the IR photodiode, bits 7:6 are the LED drive,
      // and bits 1:0 are the gain.
      assertWrite(lastWriteTo(Register.CONTROL), Register.CONTROL, 0xEF);
    }
  }

  @Test
  void rejectsUnexpectedDeviceId() {
    setRegister(Register.DEVICE_ID, new byte[] {0x44});

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      assertEquals(DeviceStatus.FAULT_UNEXPECTED_DEVICE_ID, sensor.getDeviceStatus());
      assertEquals(FailureReason.UNEXPECTED_DEVICE_ID, sensor.getLastFailureReason());
      assertEquals(0x44, sensor.getDeviceId());
      assertEquals(0, m_writes.size());
    }
  }

  @Test
  void retriesConfigurationOnUpdate() {
    setRegister(Register.DEVICE_ID, new byte[] {0x00});

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      assertEquals(DeviceStatus.FAULT_UNEXPECTED_DEVICE_ID, sensor.getDeviceStatus());

      setRegister(Register.DEVICE_ID, new byte[] {TMD37821_DEVICE_ID});
      setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 1, 2, 3, 4, 5));
      sensor.update();

      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertEquals(1, sensor.getRawClear());
      assertEquals(5, sensor.getRawProximity());
    }
  }

  @Test
  void decodesColorAndProximity() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 10240, 5120, 2560, 1024, 512));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();

      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertEquals(DEFAULT_MAXIMUM_RAW_COLOR_VALUE, sensor.getMaximumRawColorValue());

      assertEquals(10240, sensor.getRawClear());
      assertEquals(5120, sensor.getRawRed());
      assertEquals(2560, sensor.getRawGreen());
      assertEquals(1024, sensor.getRawBlue());
      assertEquals(512, sensor.getRawProximity());

      assertEquals(1.0, sensor.getClear(), DELTA);
      assertEquals(0.5, sensor.getRed(), DELTA);
      assertEquals(0.25, sensor.getGreen(), DELTA);
      assertEquals(0.1, sensor.getBlue(), DELTA);

      // Color rounds to 12 bits of precision.
      assertEquals(0.5, sensor.getColor().red, 1e-3);
      assertEquals(0.25, sensor.getColor().green, 1e-3);
      assertEquals(0.1, sensor.getColor().blue, 1e-3);

      assertEquals(512.0 / 1023.0, sensor.getProximity(), DELTA);
    }
  }

  @Test
  void addressesTheBulkReadWithTheAutoIncrementCommandType() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 1, 2, 3, 4, 5));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      m_readRegisters.clear();
      m_readCounts.clear();
      sensor.update();

      // Without the auto-increment command type the sensor would return the STATUS register
      // once per byte instead of advancing through the color and proximity registers.
      assertEquals(1, m_readRegisters.size());
      assertEquals(0xB3, m_readRegisters.get(0));
      assertEquals(Register.STATUS.getAddress() | 0x80 | 0x20, m_readRegisters.get(0));
      assertEquals(11, m_readCounts.get(0));
    }
  }

  @Test
  void clampsNormalizedChannelsWithSoftwareGain() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 0, 4096, 1024, 0, 2000));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.setSoftwareGain(4.0);
      sensor.update();

      assertEquals(4.0, sensor.getSoftwareGain(), DELTA);
      assertEquals(1.0, sensor.getRed(), DELTA);
      assertEquals(0.4, sensor.getGreen(), DELTA);
      assertEquals(4096, sensor.getRawRed());

      // The proximity channel saturates at its raw maximum.
      assertEquals(1.0, sensor.getProximity(), DELTA);
    }
  }

  @Test
  void convertsProximityToDistance() {
    // A raw reading of a + b * (cm + c)^-2 must convert back to cm.
    int rawOptical = (int) Math.round(186.347 + 30403.5 * Math.pow(5.0 + 0.576649, -2.0));
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, rawOptical));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();

      assertEquals(0.05, sensor.getDistanceMeters(), 1e-3);
      assertEquals(0.05, sensor.getDistance().in(Meters), 1e-3);
    }
  }

  @Test
  void reportsOutOfRangeDistanceAsNaN() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 100));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();

      assertTrue(Double.isNaN(sensor.getDistanceMeters()));
      assertTrue(Double.isNaN(sensor.getDistance().magnitude()));
    }
  }

  @Test
  void appliesCustomDistanceCalibration() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 300));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();
      double defaultDistance = sensor.getDistanceMeters();

      sensor.setDistanceCalibration(186.347, 2 * 30403.5, 0.576649);

      // Doubling the b parameter means the same reading is produced by a more distant target.
      assertTrue(sensor.getDistanceMeters() > defaultDistance);
    }
  }

  @Test
  void preservesMeasurementsWhenDataIsNotValid() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 100, 200, 300, 400, 500));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();

      setRegister(Register.STATUS, statusBlock(0x00, 1, 2, 3, 4, 5));
      sensor.update();

      assertEquals(DeviceStatus.FAULT_BAD_READ, sensor.getDeviceStatus());
      assertEquals(FailureReason.PROXIMITY_DATA_NOT_VALID, sensor.getLastFailureReason());
      assertEquals(2, sensor.getFailureCount());
      assertEquals(100, sensor.getRawClear());
      assertEquals(500, sensor.getRawProximity());

      setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 7, 8, 9, 10, 11));
      sensor.update();

      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertEquals(7, sensor.getRawClear());
      assertEquals(11, sensor.getRawProximity());
    }
  }

  @Test
  void updatesProximityWhenOnlyColorIsNotValid() {
    setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 100, 200, 300, 400, 500));

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.update();

      setRegister(Register.STATUS, statusBlock(STATUS_PROXIMITY_VALID, 1, 2, 3, 4, 900));
      sensor.update();

      assertEquals(FailureReason.COLOR_DATA_NOT_VALID, sensor.getLastFailureReason());
      assertEquals(1, sensor.getFailureCount());
      assertEquals(100, sensor.getRawClear());
      assertEquals(900, sensor.getRawProximity());
    }
  }

  @Test
  void reportsAbortedTransactions() {
    var i2c = new FailingI2C();
    i2c.m_failReads = true;

    try (var sensor = new RevColorSensorV2(i2c)) {
      assertEquals(DeviceStatus.FAULT_BAD_READ, sensor.getDeviceStatus());
      assertEquals(FailureReason.I2C_READ_ABORTED, sensor.getLastFailureReason());
      assertEquals(1, sensor.getFailureCount());
      assertEquals(0, m_writes.size());

      i2c.m_failReads = false;
      i2c.m_failWrites = true;
      sensor.update();

      assertEquals(FailureReason.I2C_WRITE_ABORTED, sensor.getLastFailureReason());
      assertEquals(2, sensor.getFailureCount());

      i2c.m_failWrites = false;
      setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 1, 2, 3, 4, 5));
      sensor.update();

      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertEquals(2, sensor.getRawRed());
    }
  }

  @Test
  void roundsIntegrationTimeUpToWholeCycles() {
    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.setIntegrationTime(Milliseconds.of(100));
      assertEquals(100.8, sensor.getIntegrationTime().in(Milliseconds), DELTA);
      assertEquals(42 * 1024, sensor.getMaximumRawColorValue());
      assertWrite(lastWriteTo(Register.ATIME), Register.ATIME, 256 - 42);

      // The color channels saturate at 16 bits well before the longest integration time.
      sensor.setIntegrationTime(Seconds.of(1));
      assertEquals(614.4, sensor.getIntegrationTime().in(Milliseconds), DELTA);
      assertEquals(65535, sensor.getMaximumRawColorValue());
      assertWrite(lastWriteTo(Register.ATIME), Register.ATIME, 0);

      sensor.setIntegrationTime(Milliseconds.of(1));
      assertEquals(2.4, sensor.getIntegrationTime().in(Milliseconds), DELTA);
      assertEquals(1024, sensor.getMaximumRawColorValue());
      assertWrite(lastWriteTo(Register.ATIME), Register.ATIME, 255);
    }
  }

  @Test
  void rejectsInvalidConfigurationValues() {
    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      assertThrows(NullPointerException.class, () -> sensor.setGain(null));
      assertThrows(NullPointerException.class, () -> sensor.setLedDrive(null));
      assertThrows(NullPointerException.class, () -> sensor.setIntegrationTime(null));
      assertThrows(
          IllegalArgumentException.class, () -> sensor.setIntegrationTime(Milliseconds.of(0)));
      assertThrows(
          IllegalArgumentException.class, () -> sensor.setIntegrationTime(Milliseconds.of(-1)));
      assertThrows(
          IllegalArgumentException.class,
          () -> sensor.setIntegrationTime(Milliseconds.of(Double.NaN)));
      assertThrows(IllegalArgumentException.class, () -> sensor.setProximityPulseCount(0));
      assertThrows(IllegalArgumentException.class, () -> sensor.setProximityPulseCount(256));
      assertThrows(IllegalArgumentException.class, () -> sensor.setSoftwareGain(0.0));
      assertThrows(IllegalArgumentException.class, () -> sensor.setSoftwareGain(Double.NaN));
      assertThrows(
          IllegalArgumentException.class,
          () -> sensor.setDistanceCalibration(Double.POSITIVE_INFINITY, 1.0, 1.0));

      // The sensor keeps its defaults after every rejected value.
      assertEquals(Gain.GAIN_4, sensor.getGain());
      assertEquals(LedDrive.PERCENT_50, sensor.getLedDrive());
      assertEquals(24.0, sensor.getIntegrationTime().in(Milliseconds), DELTA);
      assertEquals(8, sensor.getProximityPulseCount());
      assertEquals(1.0, sensor.getSoftwareGain(), DELTA);
    }
  }

  @Test
  void storesConfigurationWrittenBeforeInitializationSucceeds() {
    setRegister(Register.DEVICE_ID, new byte[] {0x00});

    try (var sensor = new RevColorSensorV2(I2C.Port.PORT_0)) {
      sensor.setGain(Gain.GAIN_16);
      sensor.setProximityPulseCount(32);
      assertEquals(0, m_writes.size());

      setRegister(Register.DEVICE_ID, new byte[] {TMD37821_DEVICE_ID});
      setRegister(Register.STATUS, statusBlock(STATUS_ALL_VALID, 0, 0, 0, 0, 0));
      sensor.update();

      assertEquals(DeviceStatus.READY, sensor.getDeviceStatus());
      assertWrite(lastWriteTo(Register.CONTROL), Register.CONTROL, 0x62);
      assertWrite(lastWriteTo(Register.PPULSE), Register.PPULSE, 32);
    }
  }

  private void setRegister(Register register, byte[] data) {
    m_registerData.put(register.getAddress() | COMMAND_AUTO_INCREMENT, data);
  }

  private byte[] lastWriteTo(Register register) {
    for (int i = m_writes.size() - 1; i >= 0; i--) {
      if (Byte.toUnsignedInt(m_writes.get(i)[0])
          == (register.getAddress() | COMMAND_AUTO_INCREMENT)) {
        return m_writes.get(i);
      }
    }
    throw new AssertionError("No write to " + register);
  }

  private static byte[] statusBlock(
      int status, int clear, int red, int green, int blue, int proximity) {
    byte[] data = new byte[11];
    data[0] = (byte) status;
    putUnsignedShort(data, 1, clear);
    putUnsignedShort(data, 3, red);
    putUnsignedShort(data, 5, green);
    putUnsignedShort(data, 7, blue);
    putUnsignedShort(data, 9, proximity);
    return data;
  }

  private static void putUnsignedShort(byte[] data, int offset, int value) {
    data[offset] = (byte) value;
    data[offset + 1] = (byte) (value >> 8);
  }

  private static void assertWrite(byte[] write, Register register, int expected) {
    assertEquals(register.getAddress() | COMMAND_AUTO_INCREMENT, Byte.toUnsignedInt(write[0]));
    assertEquals(expected, Byte.toUnsignedInt(write[1]));
  }

  private static final class FailingI2C extends I2C {
    private boolean m_failReads;
    private boolean m_failWrites;

    FailingI2C() {
      super(I2C.Port.PORT_0, RevColorSensorV2.DEFAULT_ADDRESS);
    }

    @Override
    public boolean read(int registerAddress, int count, byte[] buffer) {
      return m_failReads || super.read(registerAddress, count, buffer);
    }

    @Override
    public synchronized boolean write(int registerAddress, int data) {
      return m_failWrites || super.write(registerAddress, data);
    }
  }
}
