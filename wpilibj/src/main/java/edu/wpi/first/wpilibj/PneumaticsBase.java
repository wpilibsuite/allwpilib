// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

public interface PneumaticsBase extends AutoCloseable {
  /**
   * For internal use to get a module for a specific type.
   *
   * @param module module number
   * @param type module type
   * @return module
   */
  static PneumaticsBase getForType(int module, PneumaticsModuleType type) {
    if (type == PneumaticsModuleType.CTREPCM) {
      return new PneumaticsControlModule(module);
    } else if (type == PneumaticsModuleType.REVPH) {
      return new PneumaticHub(module);
    }
    throw new IllegalArgumentException("Unknown module type");
  }

  /**
   * For internal use to get the default for a specific type.
   *
   * @param type module type
   * @return module default
   */
  static int getDefaultForType(PneumaticsModuleType type) {
    if (type == PneumaticsModuleType.CTREPCM) {
      return SensorUtil.getDefaultCTREPCMModule();
    } else if (type == PneumaticsModuleType.REVPH) {
      return SensorUtil.getDefaultREVPHModule();
    }
    throw new IllegalArgumentException("Unknown module type");
  }

  /**
   * Sets solenoids on a pneumatics module.
   *
   * @param mask mask
   * @param values values
   */
  void setSolenoids(int mask, int values);

  /**
   * Gets solenoid values.
   *
   * @return values
   */
  int getSolenoids();

  /**
   * Get module number for this module.
   *
   * @return module number
   */
  int getModuleNumber();

  /**
   * Get the disabled solenoids.
   *
   * @return disabled list
   */
  int getSolenoidDisabledList();

  /**
   * Fire a single solenoid shot.
   *
   * @param index solenoid index
   */
  void fireOneShot(int index);

  /**
   * Set the duration for a single solenoid shot.
   *
   * @param index solenoid index
   * @param durMs shot duration
   */
  void setOneShotDuration(int index, int durMs);

  boolean getCompressor();

  boolean getPressureSwitch();

  double getCompressorCurrent();

  void disableCompressor();

  void enableCompressorDigital();

  void enableCompressorAnalog(double minPressure, double maxPressure);

  void enableCompressorHybrid(double minPressure, double maxPressure);

  double getAnalogVoltage(int channel);

  double getPressure(int channel);

  CompressorConfigType getCompressorConfigType();

  /**
   * Check if a solenoid channel is valid.
   *
   * @param channel Channel to check
   * @return True if channel exists
   */
  boolean checkSolenoidChannel(int channel);

  /**
   * Check to see if the masked solenoids can be reserved, and if not reserve them.
   *
   * @param mask The solenoid mask to reserve
   * @return 0 if successful, mask of solenoids that couldn't be allocated otherwise
   */
  int checkAndReserveSolenoids(int mask);

  /**
   * Unreserve the masked solenoids.
   *
   * @param mask The solenoid mask to unreserve
   */
  void unreserveSolenoids(int mask);

  boolean reserveCompressor();

  void unreserveCompressor();

  @Override
  void close();

  Solenoid makeSolenoid(int channel);

  DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel);

  Compressor makeCompressor();
}
