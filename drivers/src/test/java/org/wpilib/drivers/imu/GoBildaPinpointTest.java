// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.imu;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.wpilib.units.Units.Inches;
import static org.wpilib.units.Units.Meters;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.drivers.imu.GoBildaPinpoint.DeviceStatus;
import org.wpilib.drivers.imu.GoBildaPinpoint.EncoderDirection;
import org.wpilib.drivers.imu.GoBildaPinpoint.ErrorDetectionType;
import org.wpilib.drivers.imu.GoBildaPinpoint.FailureReason;
import org.wpilib.drivers.imu.GoBildaPinpoint.OdometryPod;
import org.wpilib.drivers.imu.GoBildaPinpoint.Register;
import org.wpilib.hardware.bus.I2C;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.simulation.CallbackStore;
import org.wpilib.simulation.I2CSim;

@ResourceLock("I2C")
class GoBildaPinpointTest {
  private static final double DELTA = 1e-5;

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
  }

  @AfterEach
  void tearDown() {
    m_readCallback.close();
    m_writeCallback.close();
    m_i2cSim.resetData();
  }

  @Test
  void usesDefaultAddressAndCloses() {
    var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0);

    assertEquals(I2C.Port.PORT_0, pinpoint.getPort());
    assertEquals(GoBildaPinpoint.DEFAULT_ADDRESS, pinpoint.getDeviceAddress());

    pinpoint.close();
    assertThrows(IllegalStateException.class, pinpoint::getPort);
    assertThrows(IllegalStateException.class, pinpoint::getDeviceStatus);
    assertThrows(IllegalStateException.class, pinpoint::getPose);
    assertThrows(IllegalStateException.class, pinpoint::update);
    assertThrows(IllegalStateException.class, () -> pinpoint.setYawScalar(1.0));

    pinpoint.close();
  }

  @Test
  void cachedQuaternionGettersThrowAfterClose() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));
    setRegister(
        Register.BULK_READ,
        concat(encodeFloat(0.5f), encodeFloat(-0.25f), encodeFloat(0.125f), encodeFloat(0.75f)));

    var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0);
    pinpoint.setBulkReadScope(
        Register.QUATERNION_W, Register.QUATERNION_X, Register.QUATERNION_Y, Register.QUATERNION_Z);
    pinpoint.update();
    pinpoint.close();

    assertThrows(IllegalStateException.class, pinpoint::getQuaternion);
    assertThrows(IllegalStateException.class, pinpoint::getRotation3d);
  }

  @Test
  void rejectsInvalidI2cAddresses() {
    assertThrows(IllegalArgumentException.class, () -> new GoBildaPinpoint(I2C.Port.PORT_0, -1));
    assertThrows(IllegalArgumentException.class, () -> new GoBildaPinpoint(I2C.Port.PORT_0, 0x80));
  }

  @Test
  void rejectsInvalidConfigurationValues() {
    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      assertThrows(IllegalArgumentException.class, () -> pinpoint.setOffsets(Double.NaN, 0.0));
      assertThrows(
          IllegalArgumentException.class, () -> pinpoint.setOffsets(0.0, Double.POSITIVE_INFINITY));
      assertThrows(
          IllegalArgumentException.class,
          () -> pinpoint.setOffsets(Meters.of(1.0), Meters.of(Double.MAX_VALUE)));
      assertEquals(0, m_writes.size());
      assertThrows(IllegalArgumentException.class, () -> pinpoint.setEncoderResolution(0.0));
      assertThrows(
          IllegalArgumentException.class, () -> pinpoint.setEncoderResolution(Double.MIN_VALUE));
      assertThrows(IllegalArgumentException.class, () -> pinpoint.setYawScalar(Double.MAX_VALUE));
      assertThrows(IllegalArgumentException.class, () -> pinpoint.setHeadingRadians(Double.NaN));
    }
  }

  @Test
  void validatesEntirePoseBeforeWriting() {
    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      assertThrows(
          IllegalArgumentException.class,
          () -> pinpoint.setPose(new Pose2d(1.0, Double.MAX_VALUE, new Rotation2d())));
      assertEquals(0, m_writes.size());

      assertThrows(
          IllegalArgumentException.class,
          () -> pinpoint.setPose(new Pose2d(1.0, 2.0, new Rotation2d(Double.NaN))));
      assertEquals(0, m_writes.size());
    }
  }

  @Test
  void decodesFirmwareV2FixedBulkRead() {
    setRegister(Register.DEVICE_VERSION, encodeInt(2));
    setRegister(
        Register.BULK_READ,
        fixedBulkData(1, 800, 12345, -54321, 1234.5f, -678.25f, 7.25f, 2500, -3000, 4.5f));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.update();

      assertEquals(DeviceStatus.READY, pinpoint.getDeviceStatus());
      assertEquals(800, pinpoint.getLoopTimeMicroseconds());
      assertEquals(1250.0, pinpoint.getFrequencyHz(), DELTA);
      assertEquals(12345, pinpoint.getXEncoder());
      assertEquals(-54321, pinpoint.getYEncoder());
      assertEquals(1.2345, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(-0.67825, pinpoint.getYPositionMeters(), DELTA);
      assertEquals(7.25, pinpoint.getHeadingRadians(), DELTA);
      assertEquals(2.5, pinpoint.getXVelocityMetersPerSecond(), DELTA);
      assertEquals(-3.0, pinpoint.getYVelocityMetersPerSecond(), DELTA);
      assertEquals(4.5, pinpoint.getHeadingVelocityRadiansPerSecond(), DELTA);
      Pose2d pose = pinpoint.getPose();
      assertEquals(1.2345, pose.getX(), DELTA);
      assertEquals(-0.67825, pose.getY(), DELTA);
      assertEquals(new Rotation2d(7.25), pose.getRotation());
    }
  }

  @Test
  void decodesFirmwareV3FlexibleBulkRead() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));
    setRegister(
        Register.BULK_READ, fixedBulkData(1, 1000, 17, 29, -2400, 3600, -2.5f, -1250, 875, -1.25f));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.update();

      assertEquals(DeviceStatus.READY, pinpoint.getDeviceStatus());
      assertEquals(-2.4, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(3.6, pinpoint.getYPositionMeters(), DELTA);
      assertEquals(-2.5, pinpoint.getHeadingRadians(), DELTA);
      assertEquals(-1.25, pinpoint.getXVelocityMetersPerSecond(), DELTA);
      assertEquals(0.875, pinpoint.getYVelocityMetersPerSecond(), DELTA);
      assertEquals(-1.25, pinpoint.getHeadingVelocityRadiansPerSecond(), DELTA);
    }
  }

  @Test
  void synchronizesDefaultBulkReadScopeForANewV3Instance() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));
    setRegister(
        Register.BULK_READ, fixedBulkData(1, 1000, 17, 29, -2400, 3600, -2.5f, -1250, 875, -1.25f));

    try (var previous = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      previous.setBulkReadScope(Register.X_POSITION, Register.H_ORIENTATION);
    }
    m_writes.clear();

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.update();

      assertEquals(1, m_writes.size());
      assertArrayEquals(
          new byte[] {
            (byte) Register.SET_BULK_READ.getAddress(),
            (byte) Register.DEVICE_STATUS.getAddress(),
            (byte) Register.LOOP_TIME.getAddress(),
            (byte) Register.X_ENCODER_VALUE.getAddress(),
            (byte) Register.Y_ENCODER_VALUE.getAddress(),
            (byte) Register.X_POSITION.getAddress(),
            (byte) Register.Y_POSITION.getAddress(),
            (byte) Register.H_ORIENTATION.getAddress(),
            (byte) Register.X_VELOCITY.getAddress(),
            (byte) Register.Y_VELOCITY.getAddress(),
            (byte) Register.H_VELOCITY.getAddress()
          },
          m_writes.get(0));
      assertEquals(-2.4, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(3.6, pinpoint.getYPositionMeters(), DELTA);
    }
  }

  @Test
  void configuresFlexibleBulkReadAndRemovesDuplicates() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.X_POSITION, Register.H_ORIENTATION, Register.X_POSITION);

      assertArrayEquals(
          new byte[] {
            (byte) Register.SET_BULK_READ.getAddress(),
            (byte) Register.X_POSITION.getAddress(),
            (byte) Register.H_ORIENTATION.getAddress()
          },
          m_writes.get(0));

      setRegister(Register.BULK_READ, concat(encodeFloat(8500), encodeFloat(12.5f)));
      pinpoint.update();

      assertEquals(8.5, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(12.5, pinpoint.getHeadingRadians(), DELTA);
    }
  }

  @Test
  void flexibleBulkReadCannotOverwriteDetectedDeviceVersion() {
    for (ErrorDetectionType errorDetectionType :
        new ErrorDetectionType[] {ErrorDetectionType.NONE, ErrorDetectionType.LOCAL_TEST}) {
      setRegister(Register.DEVICE_VERSION, encodeInt(3));

      try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
        pinpoint.setErrorDetectionType(errorDetectionType);
        pinpoint.setBulkReadScope(Register.DEVICE_VERSION, Register.X_POSITION);
        final int readCount = m_readCounts.size();

        setRegister(Register.BULK_READ, concat(encodeInt(2), encodeFloat(1000)));
        pinpoint.update();

        assertEquals(3, pinpoint.getDeviceVersion());
        assertEquals(1.0, pinpoint.getXPositionMeters(), DELTA);

        setRegister(Register.BULK_READ, concat(encodeInt(1), encodeFloat(2000)));
        pinpoint.update();

        assertEquals(3, pinpoint.getDeviceVersion());
        assertEquals(2.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(readCount + 2, m_readCounts.size());
        assertEquals(8, m_readCounts.get(readCount));
        assertEquals(8, m_readCounts.get(readCount + 1));
        assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount));
        assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount + 1));
      }
    }
  }

  @Test
  void deviceVersionGetterCannotOverwriteDetectedProtocolVersion() {
    for (ErrorDetectionType errorDetectionType :
        new ErrorDetectionType[] {ErrorDetectionType.NONE, ErrorDetectionType.LOCAL_TEST}) {
      setRegister(Register.DEVICE_VERSION, encodeInt(3));
      setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 1000, 0, 0, 0, 0, 0));

      try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
        pinpoint.setErrorDetectionType(errorDetectionType);
        pinpoint.update();

        int readCount = m_readCounts.size();
        setRegister(Register.DEVICE_VERSION, encodeInt(2));
        assertEquals(3, pinpoint.getDeviceVersion());
        assertEquals(readCount, m_readCounts.size());

        setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 2000, 0, 0, 0, 0, 0));
        pinpoint.update();
        assertEquals(2.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(40, m_readCounts.get(readCount));

        readCount = m_readCounts.size();
        setRegister(Register.DEVICE_VERSION, encodeInt(-1));
        assertEquals(3, pinpoint.getDeviceVersion());
        assertEquals(readCount, m_readCounts.size());

        setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 3000, 0, 0, 0, 0, 0));
        pinpoint.update();
        assertEquals(3.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(40, m_readCounts.get(readCount));
      }
    }
  }

  @Test
  void readsPartialPoseScopeAsSingleSnapshot() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.X_POSITION, Register.H_ORIENTATION);
      setRegister(Register.BULK_READ, concat(encodeFloat(1000), encodeFloat(0.1f)));
      pinpoint.update();

      setRegister(
          Register.BULK_READ, concat(encodeFloat(2000), encodeFloat(3000), encodeFloat(0.2f)));
      final int readCount = m_readCounts.size();
      final int writeCount = m_writes.size();

      Pose2d pose = pinpoint.getPose();

      assertEquals(2.0, pose.getX(), DELTA);
      assertEquals(3.0, pose.getY(), DELTA);
      assertEquals(0.2, pose.getRotation().getRadians(), DELTA);
      assertEquals(readCount + 1, m_readCounts.size());
      assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount));
      assertEquals(12, m_readCounts.get(readCount));
      assertArrayEquals(
          new byte[] {
            (byte) Register.SET_BULK_READ.getAddress(),
            (byte) Register.X_POSITION.getAddress(),
            (byte) Register.Y_POSITION.getAddress(),
            (byte) Register.H_ORIENTATION.getAddress()
          },
          m_writes.get(writeCount));
      assertArrayEquals(
          new byte[] {
            (byte) Register.SET_BULK_READ.getAddress(),
            (byte) Register.X_POSITION.getAddress(),
            (byte) Register.H_ORIENTATION.getAddress()
          },
          m_writes.get(writeCount + 1));

      setRegister(
          Register.BULK_READ, concat(encodeFloat(2500), encodeFloat(Float.NaN), encodeFloat(0.3f)));
      pose = pinpoint.getPose();

      assertEquals(2.0, pose.getX(), DELTA);
      assertEquals(3.0, pose.getY(), DELTA);
      assertEquals(0.2, pose.getRotation().getRadians(), DELTA);
      assertEquals(Register.Y_POSITION, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.NONFINITE_VALUE, pinpoint.getLastFailureReason());
    }
  }

  @Test
  void readsOmittedPoseScopeAsSingleCrcProtectedSnapshot() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.DEVICE_STATUS);
      pinpoint.setErrorDetectionType(ErrorDetectionType.CRC);
      setRegister(Register.BULK_READ, appendCrc(encodeInt(1)));
      pinpoint.update();

      setRegister(
          Register.BULK_READ,
          appendCrc(concat(encodeFloat(4000), encodeFloat(-5000), encodeFloat(1.25f))));
      final int readCount = m_readCounts.size();

      Pose2d pose = pinpoint.getPose();

      assertEquals(4.0, pose.getX(), DELTA);
      assertEquals(-5.0, pose.getY(), DELTA);
      assertEquals(1.25, pose.getRotation().getRadians(), DELTA);
      assertEquals(readCount + 1, m_readCounts.size());
      assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount));
      assertEquals(13, m_readCounts.get(readCount));
    }
  }

  @Test
  void readsPartialQuaternionScopeAsSingleSnapshot() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.QUATERNION_W, Register.QUATERNION_Z);
      setRegister(Register.BULK_READ, concat(encodeFloat(0.1f), encodeFloat(0.4f)));
      pinpoint.update();

      setRegister(
          Register.BULK_READ,
          concat(encodeFloat(0.5f), encodeFloat(-0.25f), encodeFloat(0.125f), encodeFloat(0.75f)));
      final int readCount = m_readCounts.size();

      var quaternion = pinpoint.getQuaternion();

      assertEquals(0.5, quaternion.getW(), DELTA);
      assertEquals(-0.25, quaternion.getX(), DELTA);
      assertEquals(0.125, quaternion.getY(), DELTA);
      assertEquals(0.75, quaternion.getZ(), DELTA);
      assertEquals(readCount + 1, m_readCounts.size());
      assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount));
      assertEquals(16, m_readCounts.get(readCount));

      setRegister(
          Register.BULK_READ,
          concat(encodeFloat(0.6f), encodeFloat(Float.NaN), encodeFloat(0.2f), encodeFloat(0.7f)));
      quaternion = pinpoint.getQuaternion();

      assertEquals(0.5, quaternion.getW(), DELTA);
      assertEquals(-0.25, quaternion.getX(), DELTA);
      assertEquals(0.125, quaternion.getY(), DELTA);
      assertEquals(0.75, quaternion.getZ(), DELTA);
      assertEquals(Register.QUATERNION_X, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.NONFINITE_VALUE, pinpoint.getLastFailureReason());
    }
  }

  @Test
  void failedSnapshotScopeRestorationRetriesRequestedScope() {
    for (boolean quaternionSnapshot : new boolean[] {false, true}) {
      byte[] requestedScope = {
        (byte) Register.SET_BULK_READ.getAddress(),
        (byte) Register.DEVICE_STATUS.getAddress(),
        (byte) Register.X_POSITION.getAddress()
      };
      byte[] snapshotData =
          quaternionSnapshot
              ? concat(
                  encodeFloat(0.5f), encodeFloat(-0.25f), encodeFloat(0.125f), encodeFloat(0.75f))
              : concat(encodeFloat(1500), encodeFloat(2500), encodeFloat(0.5f));
      var faultingI2c =
          new RestoreFailingI2C(
              requestedScope,
              encodeInt(3),
              concat(encodeInt(1), encodeFloat(1000)),
              snapshotData,
              concat(encodeInt(2), encodeFloat(2000)));

      try (var pinpoint = new GoBildaPinpoint(faultingI2c)) {
        pinpoint.setBulkReadScope(Register.DEVICE_STATUS, Register.X_POSITION);
        pinpoint.update();
        faultingI2c.failNextRestoration();

        if (quaternionSnapshot) {
          pinpoint.getQuaternion();
        } else {
          pinpoint.getPose();
        }
        pinpoint.update();

        assertEquals(2.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(DeviceStatus.CALIBRATING, pinpoint.getDeviceStatus());
        assertEquals(4, faultingI2c.m_readCounts.size());
        assertEquals(4, faultingI2c.m_readCounts.get(0));
        assertEquals(8, faultingI2c.m_readCounts.get(1));
        assertEquals(quaternionSnapshot ? 16 : 12, faultingI2c.m_readCounts.get(2));
        assertEquals(8, faultingI2c.m_readCounts.get(3));
        assertEquals(8, faultingI2c.m_writes.size());
        assertArrayEquals(requestedScope, faultingI2c.m_writes.get(5));
        assertArrayEquals(requestedScope, faultingI2c.m_writes.get(6));
      }
    }
  }

  @Test
  void rejectsInvalidFlexibleBulkReadScopes() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      assertThrows(IllegalArgumentException.class, pinpoint::setBulkReadScope);
      assertThrows(
          IllegalArgumentException.class, () -> pinpoint.setBulkReadScope(Register.DEVICE_CONTROL));
      assertThrows(
          IllegalArgumentException.class, () -> pinpoint.setBulkReadScope(Register.BULK_READ));
    }
  }

  @Test
  void validatesCrcAndPreservesLastGoodBulkRead() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));
    byte[] goodData = fixedBulkData(1, 1000, 1, 2, 1500, -2500, 0.75f, 500, -750, 0.25f);
    setRegister(Register.BULK_READ, appendCrc(goodData));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setErrorDetectionType(ErrorDetectionType.CRC);
      pinpoint.update();

      assertEquals(1.5, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(DeviceStatus.READY, pinpoint.getDeviceStatus());

      byte[] corruptData = appendCrc(fixedBulkData(1, 1000, 1, 2, 4200, 0, 0, 0, 0, 0));
      corruptData[corruptData.length - 1] ^= 0x01;
      setRegister(Register.BULK_READ, corruptData);
      pinpoint.update();

      assertEquals(1.5, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals(Register.BULK_READ, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.CRC_MISMATCH, pinpoint.getLastFailureReason());
      assertEquals(1, pinpoint.getFailureCount());
      assertEquals(1, pinpoint.getFailureCount(Register.BULK_READ));
    }
  }

  @Test
  void localValidationPreservesEntirePoseForImplausiblePositionReading() {
    for (int version : new int[] {2, 3}) {
      setRegister(Register.DEVICE_VERSION, encodeInt(version));
      setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 1000, 0, 0, 0, 0, 0));

      try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
        pinpoint.update();
        assertEquals(1.0, pinpoint.getXPositionMeters(), DELTA);

        setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 7000, 2000, 1, 0, 0, 0));
        pinpoint.update();

        assertEquals(1.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(0.0, pinpoint.getYPositionMeters(), DELTA);
        assertEquals(0.0, pinpoint.getHeadingRadians(), DELTA);
        assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
        assertEquals(Register.X_POSITION, pinpoint.getLastFailedRegister());
        assertEquals(FailureReason.CHANGE_TOO_LARGE, pinpoint.getLastFailureReason());
        assertEquals(1, pinpoint.getFailureCount(Register.X_POSITION));
      }
    }
  }

  @Test
  void localValidationReestablishesPoseBaselineAfterOmittedBulkSamples() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.DEVICE_STATUS);
      setRegister(Register.BULK_READ, encodeInt(1));
      pinpoint.update();

      setRegister(
          Register.BULK_READ, concat(encodeFloat(1000), encodeFloat(2000), encodeFloat(0.5f)));
      Pose2d pose = pinpoint.getPose();
      assertEquals(1.0, pose.getX(), DELTA);
      assertEquals(2.0, pose.getY(), DELTA);

      setRegister(Register.BULK_READ, encodeInt(1));
      pinpoint.update();

      setRegister(
          Register.BULK_READ, concat(encodeFloat(7000), encodeFloat(8000), encodeFloat(1.0f)));
      pose = pinpoint.getPose();
      assertEquals(7.0, pose.getX(), DELTA);
      assertEquals(8.0, pose.getY(), DELTA);
      assertEquals(1.0, pose.getRotation().getRadians(), DELTA);
      assertEquals(0, pinpoint.getFailureCount());

      setRegister(
          Register.BULK_READ, concat(encodeFloat(13_000), encodeFloat(8000), encodeFloat(1.0f)));
      pose = pinpoint.getPose();
      assertEquals(7.0, pose.getX(), DELTA);
      assertEquals(8.0, pose.getY(), DELTA);
      assertEquals(1.0, pose.getRotation().getRadians(), DELTA);
      assertEquals(FailureReason.CHANGE_TOO_LARGE, pinpoint.getLastFailureReason());
    }
  }

  @Test
  void enteringLocalValidationEstablishesFreshPoseBaselines() {
    for (ErrorDetectionType initialMode :
        new ErrorDetectionType[] {ErrorDetectionType.NONE, ErrorDetectionType.CRC}) {
      setRegister(Register.DEVICE_VERSION, encodeInt(3));
      byte[] corruptedPose =
          fixedBulkData(1, 1000, 0, 0, Float.POSITIVE_INFINITY, 8000, 200, 0, 0, 0);
      if (initialMode == ErrorDetectionType.CRC) {
        corruptedPose = appendCrc(corruptedPose);
      }
      setRegister(Register.BULK_READ, corruptedPose);

      try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
        pinpoint.setErrorDetectionType(initialMode);
        pinpoint.update();

        assertEquals(Double.POSITIVE_INFINITY, pinpoint.getXPositionMeters());
        assertEquals(8.0, pinpoint.getYPositionMeters(), DELTA);
        assertEquals(200.0, pinpoint.getHeadingRadians(), DELTA);

        pinpoint.setErrorDetectionType(ErrorDetectionType.LOCAL_TEST);
        setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 1000, 2000, 0.5f, 0, 0, 0));
        pinpoint.update();

        assertEquals(1.0, pinpoint.getXPositionMeters(), DELTA);
        assertEquals(2.0, pinpoint.getYPositionMeters(), DELTA);
        assertEquals(0.5, pinpoint.getHeadingRadians(), DELTA);
        assertEquals(DeviceStatus.READY, pinpoint.getDeviceStatus());
        assertEquals(0, pinpoint.getFailureCount());
      }
    }
  }

  @Test
  void poseChangingWritesResetLocalValidationBaselines() {
    setRegister(Register.DEVICE_VERSION, encodeInt(2));
    setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 0, 0, 0, 0, 0, 0));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.update();

      pinpoint.setPose(new Pose2d(7.0, -7.0, new Rotation2d(1.0)));
      setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 7000, -7000, 1, 0, 0, 0));
      pinpoint.update();
      assertEquals(7.0, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(-7.0, pinpoint.getYPositionMeters(), DELTA);

      pinpoint.setXPositionMeters(-7.0);
      pinpoint.setYPositionMeters(7.0);
      pinpoint.setHeadingRadians(130.0);
      setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, -7000, 7000, 130, 0, 0, 0));
      pinpoint.update();
      assertEquals(-7.0, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(7.0, pinpoint.getYPositionMeters(), DELTA);
      assertEquals(130.0, pinpoint.getHeadingRadians(), DELTA);

      pinpoint.resetPositionAndImu();
      setRegister(Register.BULK_READ, fixedBulkData(1, 1000, 0, 0, 0, 0, 0, 0, 0, 0));
      pinpoint.update();
      assertEquals(0.0, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(0.0, pinpoint.getYPositionMeters(), DELTA);
      assertEquals(0.0, pinpoint.getHeadingRadians(), DELTA);
      assertEquals(DeviceStatus.READY, pinpoint.getDeviceStatus());
    }
  }

  @Test
  void flexibleBulkReadRejectsInvalidLoopTimeBeforeCachingFloats() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.X_POSITION, Register.LOOP_TIME);
      setRegister(Register.BULK_READ, concat(encodeFloat(1500), encodeInt(0)));

      pinpoint.update();

      assertEquals(0.0, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals(Register.LOOP_TIME, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.INVALID_LOOP_TIME, pinpoint.getLastFailureReason());
    }
  }

  @Test
  void localValidationRejectsNonfiniteValuesFromEveryFloatRegister() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));
    Register[] floatRegisters = {
      Register.X_POSITION,
      Register.Y_POSITION,
      Register.H_ORIENTATION,
      Register.X_VELOCITY,
      Register.Y_VELOCITY,
      Register.H_VELOCITY,
      Register.MM_PER_TICK,
      Register.X_POD_OFFSET,
      Register.Y_POD_OFFSET,
      Register.YAW_SCALAR,
      Register.QUATERNION_W,
      Register.QUATERNION_X,
      Register.QUATERNION_Y,
      Register.QUATERNION_Z,
      Register.PITCH,
      Register.ROLL
    };

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      for (Register register : floatRegisters) {
        pinpoint.setBulkReadScope(register);
        setRegister(Register.BULK_READ, encodeFloat(Float.NaN));

        pinpoint.update();

        assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
        assertEquals(register, pinpoint.getLastFailedRegister());
        assertEquals(FailureReason.NONFINITE_VALUE, pinpoint.getLastFailureReason());
        assertEquals(1, pinpoint.getFailureCount(register));
      }
    }
  }

  @Test
  void customBulkScopeCannotOverwriteLocalReadFault() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.X_POSITION, Register.DEVICE_STATUS);
      setRegister(Register.BULK_READ, concat(encodeFloat(1000), encodeInt(1)));
      pinpoint.update();

      setRegister(Register.BULK_READ, concat(encodeFloat(7000), encodeInt(1)));
      pinpoint.update();

      assertEquals(1.0, pinpoint.getXPositionMeters(), DELTA);
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
    }
  }

  @Test
  void goodNarrowReadClearsPreviousLocalReadFault() {
    setRegister(Register.H_ORIENTATION, encodeFloat(0.5f));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.updateHeading();

      setRegister(Register.H_ORIENTATION, encodeFloat(Float.NaN));
      pinpoint.updateHeading();
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals(0.5, pinpoint.getHeadingRadians(), DELTA);

      setRegister(Register.H_ORIENTATION, encodeFloat(0.75f));
      pinpoint.updateHeading();
      assertEquals(DeviceStatus.NOT_READY, pinpoint.getDeviceStatus());
      assertEquals(0.75, pinpoint.getHeadingRadians(), DELTA);
    }
  }

  @Test
  void transientReadFailuresPreserveCachedDeviceStatus() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.DEVICE_STATUS, Register.H_ORIENTATION);
      setRegister(Register.BULK_READ, concat(encodeInt(1 << 1), encodeFloat(0.25f)));
      pinpoint.update();
      assertEquals(DeviceStatus.CALIBRATING, pinpoint.getDeviceStatus());

      setRegister(Register.H_ORIENTATION, encodeFloat(Float.NaN));
      pinpoint.updateHeading();
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals((1 << 1) | (1 << 5), pinpoint.getDeviceStatusBits());

      setRegister(Register.H_ORIENTATION, encodeFloat(0.5f));
      pinpoint.updateHeading();
      assertEquals(DeviceStatus.CALIBRATING, pinpoint.getDeviceStatus());
      assertEquals(1 << 1, pinpoint.getDeviceStatusBits());

      setRegister(Register.BULK_READ, concat(encodeInt(1 << 2), encodeFloat(0.75f)));
      pinpoint.update();
      assertEquals(DeviceStatus.FAULT_X_POD_NOT_DETECTED, pinpoint.getDeviceStatus());

      pinpoint.setBulkReadScope(Register.H_ORIENTATION);
      setRegister(Register.BULK_READ, encodeFloat(Float.NaN));
      pinpoint.update();
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals((1 << 2) | (1 << 5), pinpoint.getDeviceStatusBits());

      setRegister(Register.BULK_READ, encodeFloat(1.0f));
      pinpoint.update();
      assertEquals(DeviceStatus.FAULT_X_POD_NOT_DETECTED, pinpoint.getDeviceStatus());
      assertEquals(1 << 2, pinpoint.getDeviceStatusBits());
    }
  }

  @Test
  void writesConfigurationInDeviceUnitsAndLittleEndian() {
    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setOffsets(Inches.of(2), Inches.of(-3));
      pinpoint.setEncoderResolution(OdometryPod.SWINGARM);
      pinpoint.setYawScalar(1.0125);
      pinpoint.setEncoderDirections(EncoderDirection.REVERSED, EncoderDirection.FORWARD);
      pinpoint.recalibrateImu();
      pinpoint.resetPositionAndImu();
      pinpoint.setPose(new Pose2d(1.25, -0.75, new Rotation2d(2.25)));
    }

    assertFloatWrite(m_writes.get(0), Register.X_POD_OFFSET, 50.8f);
    assertFloatWrite(m_writes.get(1), Register.Y_POD_OFFSET, -76.2f);
    assertFloatWrite(m_writes.get(2), Register.MM_PER_TICK, 13.26291192f);
    assertFloatWrite(m_writes.get(3), Register.YAW_SCALAR, 1.0125f);
    assertIntWrite(m_writes.get(4), Register.DEVICE_CONTROL, 1 << 4);
    assertIntWrite(m_writes.get(5), Register.DEVICE_CONTROL, 1 << 3);
    assertIntWrite(m_writes.get(6), Register.DEVICE_CONTROL, 1 << 0);
    assertIntWrite(m_writes.get(7), Register.DEVICE_CONTROL, 1 << 1);
    assertFloatWrite(m_writes.get(8), Register.X_POSITION, 1250.0f);
    assertFloatWrite(m_writes.get(9), Register.Y_POSITION, -750.0f);
    assertFloatWrite(m_writes.get(10), Register.H_ORIENTATION, 2.25f);
  }

  @Test
  void convertsEncoderResolutionBetweenTicksPerMeterAndDeviceUnits() {
    setRegister(Register.MM_PER_TICK, encodeFloat(12.345f));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setEncoderResolution(54321.0);
      assertFloatWrite(m_writes.get(0), Register.MM_PER_TICK, 54.321f);
      assertEquals(12.345f * 1000.0, pinpoint.getEncoderResolutionTicksPerMeter(), DELTA);
    }
  }

  @Test
  void readsV3OrientationRegisters() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(
          Register.QUATERNION_W,
          Register.QUATERNION_X,
          Register.QUATERNION_Y,
          Register.QUATERNION_Z,
          Register.PITCH,
          Register.ROLL);
      setRegister(
          Register.BULK_READ,
          concat(
              encodeFloat(0.5f),
              encodeFloat(-0.25f),
              encodeFloat(0.125f),
              encodeFloat(0.75f),
              encodeFloat(0.45f),
              encodeFloat(-0.65f)));
      pinpoint.update();
      final int readCount = m_readCounts.size();

      var quaternion = pinpoint.getQuaternion();

      assertEquals(0.5, quaternion.getW(), DELTA);
      assertEquals(-0.25, quaternion.getX(), DELTA);
      assertEquals(0.125, quaternion.getY(), DELTA);
      assertEquals(0.75, quaternion.getZ(), DELTA);
      assertEquals(0.45, pinpoint.getPitchRadians(), DELTA);
      assertEquals(-0.65, pinpoint.getRollRadians(), DELTA);
      assertEquals(readCount, m_readCounts.size());

      setRegister(
          Register.BULK_READ,
          concat(
              encodeFloat(0.6f),
              encodeFloat(Float.NaN),
              encodeFloat(0.2f),
              encodeFloat(0.7f),
              encodeFloat(0.55f),
              encodeFloat(-0.75f)));
      pinpoint.update();
      quaternion = pinpoint.getQuaternion();

      assertEquals(0.5, quaternion.getW(), DELTA);
      assertEquals(-0.25, quaternion.getX(), DELTA);
      assertEquals(0.125, quaternion.getY(), DELTA);
      assertEquals(0.75, quaternion.getZ(), DELTA);
      assertEquals(0.55, pinpoint.getPitchRadians(), DELTA);
      assertEquals(-0.75, pinpoint.getRollRadians(), DELTA);
      assertEquals(Register.QUATERNION_X, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.NONFINITE_VALUE, pinpoint.getLastFailureReason());
    }
  }

  @Test
  void localValidationPreservesQuaternionForDegenerateSamples() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(
          Register.QUATERNION_W,
          Register.QUATERNION_X,
          Register.QUATERNION_Y,
          Register.QUATERNION_Z);
      setRegister(
          Register.BULK_READ,
          concat(encodeFloat(0.5f), encodeFloat(-0.25f), encodeFloat(0.125f), encodeFloat(0.75f)));
      pinpoint.update();

      for (float component : new float[] {0.0f, 1e-8f}) {
        setRegister(
            Register.BULK_READ,
            concat(
                encodeFloat(component),
                encodeFloat(component),
                encodeFloat(component),
                encodeFloat(component)));
        pinpoint.update();

        var quaternion = pinpoint.getQuaternion();
        assertEquals(0.5, quaternion.getW(), DELTA);
        assertEquals(-0.25, quaternion.getX(), DELTA);
        assertEquals(0.125, quaternion.getY(), DELTA);
        assertEquals(0.75, quaternion.getZ(), DELTA);
        assertEquals(Register.QUATERNION_W, pinpoint.getLastFailedRegister());
        assertEquals(FailureReason.INVALID_QUATERNION, pinpoint.getLastFailureReason());
      }
      assertEquals(2, pinpoint.getFailureCount(Register.QUATERNION_W));
    }
  }

  @Test
  void quaternionSnapshotCrcFailurePreservesCachedValues() {
    setRegister(Register.DEVICE_VERSION, encodeInt(3));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      pinpoint.setBulkReadScope(Register.DEVICE_STATUS);
      pinpoint.setErrorDetectionType(ErrorDetectionType.CRC);
      setRegister(Register.BULK_READ, appendCrc(encodeInt(1)));
      pinpoint.update();

      byte[] data =
          appendCrc(
              concat(
                  encodeFloat(0.5f), encodeFloat(-0.25f), encodeFloat(0.125f), encodeFloat(0.75f)));
      data[data.length - 1] ^= 0x01;
      setRegister(Register.BULK_READ, data);
      final int readCount = m_readCounts.size();

      var quaternion = pinpoint.getQuaternion();

      assertEquals(0.0, quaternion.getW(), DELTA);
      assertEquals(0.0, quaternion.getX(), DELTA);
      assertEquals(0.0, quaternion.getY(), DELTA);
      assertEquals(0.0, quaternion.getZ(), DELTA);
      assertEquals(readCount + 1, m_readCounts.size());
      assertEquals(Register.BULK_READ.getAddress(), m_readRegisters.get(readCount));
      assertEquals(17, m_readCounts.get(readCount));
      assertEquals(DeviceStatus.FAULT_BAD_READ, pinpoint.getDeviceStatus());
      assertEquals(Register.BULK_READ, pinpoint.getLastFailedRegister());
      assertEquals(FailureReason.CRC_MISMATCH, pinpoint.getLastFailureReason());
      assertEquals(1, pinpoint.getFailureCount(Register.BULK_READ));
    }
  }

  @Test
  void rejectsV3FeaturesOnOlderFirmware() {
    setRegister(Register.DEVICE_VERSION, encodeInt(2));

    try (var pinpoint = new GoBildaPinpoint(I2C.Port.PORT_0)) {
      assertThrows(
          UnsupportedOperationException.class,
          () -> pinpoint.setErrorDetectionType(ErrorDetectionType.CRC));
      assertThrows(
          UnsupportedOperationException.class,
          () -> pinpoint.setBulkReadScope(Register.X_POSITION));
      assertThrows(UnsupportedOperationException.class, pinpoint::getQuaternion);
      assertThrows(UnsupportedOperationException.class, pinpoint::getPitchRadians);
      assertThrows(UnsupportedOperationException.class, pinpoint::getRollRadians);
    }
  }

  private void setRegister(Register register, byte[] data) {
    m_registerData.put(register.getAddress(), data);
  }

  private static class RestoreFailingI2C extends I2C {
    private final byte[] m_requestedScope;
    private final byte[][] m_reads;
    private final List<byte[]> m_writes = new ArrayList<>();
    private final List<Integer> m_readCounts = new ArrayList<>();
    private boolean m_failNextRestoration;
    private int m_readIndex;

    RestoreFailingI2C(byte[] requestedScope, byte[]... reads) {
      super(Port.PORT_0, GoBildaPinpoint.DEFAULT_ADDRESS);
      m_requestedScope = requestedScope.clone();
      m_reads = new byte[reads.length][];
      for (int i = 0; i < reads.length; i++) {
        m_reads[i] = reads[i].clone();
      }
    }

    synchronized void failNextRestoration() {
      m_failNextRestoration = true;
    }

    @Override
    public synchronized boolean writeBulk(byte[] data) {
      m_writes.add(data.clone());
      if (m_failNextRestoration && Arrays.equals(data, m_requestedScope)) {
        m_failNextRestoration = false;
        return true;
      }
      return false;
    }

    @Override
    public boolean readOnly(byte[] buffer, int count) {
      if (m_readIndex >= m_reads.length || m_reads[m_readIndex].length != count) {
        throw new IllegalStateException("Unexpected I2C read");
      }
      m_readCounts.add(count);
      System.arraycopy(m_reads[m_readIndex], 0, buffer, 0, count);
      m_readIndex++;
      return false;
    }
  }

  private static byte[] fixedBulkData(
      int status,
      int loopTime,
      int xEncoder,
      int yEncoder,
      float xPosition,
      float yPosition,
      float heading,
      float xVelocity,
      float yVelocity,
      float headingVelocity) {
    return concat(
        encodeInt(status),
        encodeInt(loopTime),
        encodeInt(xEncoder),
        encodeInt(yEncoder),
        encodeFloat(xPosition),
        encodeFloat(yPosition),
        encodeFloat(heading),
        encodeFloat(xVelocity),
        encodeFloat(yVelocity),
        encodeFloat(headingVelocity));
  }

  private static byte[] concat(byte[]... arrays) {
    int length = 0;
    for (byte[] array : arrays) {
      length += array.length;
    }
    byte[] result = new byte[length];
    int offset = 0;
    for (byte[] array : arrays) {
      System.arraycopy(array, 0, result, offset, array.length);
      offset += array.length;
    }
    return result;
  }

  private static byte[] encodeInt(int value) {
    return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(value).array();
  }

  private static byte[] encodeFloat(float value) {
    return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putFloat(value).array();
  }

  private static byte[] appendCrc(byte[] data) {
    byte[] result = Arrays.copyOf(data, data.length + 1);
    result[data.length] = computeCrc8(data);
    return result;
  }

  private static byte computeCrc8(byte[] data) {
    byte crc = (byte) 0x90;
    for (byte value : data) {
      crc ^= value;
      for (int bit = 0; bit < 8; bit++) {
        crc = (crc & 0x80) != 0 ? (byte) ((crc << 1) ^ (byte) 0x31) : (byte) (crc << 1);
      }
    }
    return crc;
  }

  private static void assertFloatWrite(byte[] write, Register register, float expected) {
    assertEquals(register.getAddress(), Byte.toUnsignedInt(write[0]));
    assertEquals(
        expected, ByteBuffer.wrap(write, 1, 4).order(ByteOrder.LITTLE_ENDIAN).getFloat(), DELTA);
  }

  private static void assertIntWrite(byte[] write, Register register, int expected) {
    assertEquals(register.getAddress(), Byte.toUnsignedInt(write[0]));
    assertEquals(expected, ByteBuffer.wrap(write, 1, 4).order(ByteOrder.LITTLE_ENDIAN).getInt());
  }
}
