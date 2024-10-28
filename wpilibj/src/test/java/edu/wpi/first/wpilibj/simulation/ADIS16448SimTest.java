// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.ADIS16448_IMU;
import edu.wpi.first.wpilibj.SPI;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class ADIS16448SimTest {
  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testYawAxis(ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      assertEquals(yawAxis, imu.getYawAxis());
    }
  }

  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testGyroAngles(
      ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      ADIS16448_IMUSim sim = new ADIS16448_IMUSim(imu);
      sim.setGyroAngleX(123.45);
      sim.setGyroAngleY(-67.89);
      sim.setGyroAngleZ(4.13);

      assertEquals(123.45, imu.getGyroAngleX());
      assertEquals(-67.89, imu.getGyroAngleY());
      assertEquals(4.13, imu.getGyroAngleZ());
    }
  }

  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testGyroRates(ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      ADIS16448_IMUSim sim = new ADIS16448_IMUSim(imu);
      sim.setGyroRateX(15.92);
      sim.setGyroRateY(-65.35);
      sim.setGyroRateZ(2.71);

      assertEquals(15.92, imu.getGyroRateX());
      assertEquals(-65.35, imu.getGyroRateY());
      assertEquals(2.71, imu.getGyroRateZ());
    }
  }

  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testAccelValues(
      ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      ADIS16448_IMUSim sim = new ADIS16448_IMUSim(imu);
      sim.setAccelX(6.85);
      sim.setAccelY(-1.82);
      sim.setAccelZ(8.69);

      assertEquals(6.85, imu.getAccelX());
      assertEquals(-1.82, imu.getAccelY());
      assertEquals(8.69, imu.getAccelZ());
    }
  }

  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testAngleBasedOnYawAxis(
      ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      ADIS16448_IMUSim sim = new ADIS16448_IMUSim(imu);
      sim.setGyroAngleX(123.45);
      sim.setGyroAngleY(-67.89);
      sim.setGyroAngleZ(4.13);

      switch (yawAxis) {
        case kX -> assertEquals(imu.getAngle(), imu.getGyroAngleX());
        case kY -> assertEquals(imu.getAngle(), imu.getGyroAngleY());
        case kZ -> assertEquals(imu.getAngle(), imu.getGyroAngleZ());
        default -> fail("invalid YawAxis!");
      }
    }
  }

  @ParameterizedTest
  @MethodSource("provideEnumCombinations")
  void testRateBasedOnYawAxis(
      ADIS16448_IMU.IMUAxis yawAxis, ADIS16448_IMU.CalibrationTime calibrationTime) {
    HAL.initialize(500, 0);

    try (ADIS16448_IMU imu = new ADIS16448_IMU(yawAxis, SPI.Port.kMXP, calibrationTime)) {
      ADIS16448_IMUSim sim = new ADIS16448_IMUSim(imu);
      sim.setGyroRateX(1.92);
      sim.setGyroRateY(-6.35);
      sim.setGyroRateZ(20.71);

      switch (yawAxis) {
        case kX -> assertEquals(imu.getRate(), imu.getGyroRateX());
        case kY -> assertEquals(imu.getRate(), imu.getGyroRateY());
        case kZ -> assertEquals(imu.getRate(), imu.getGyroRateZ());
        default -> fail("invalid YawAxis!");
      }
    }
  }

  static Stream<Arguments> provideEnumCombinations() {
    return Stream.of(ADIS16448_IMU.IMUAxis.values())
        .flatMap(
            imuAxis ->
                Stream.of(ADIS16448_IMU.CalibrationTime.values())
                    .map(calibrationTime -> arguments(imuAxis, calibrationTime)));
  }
}
