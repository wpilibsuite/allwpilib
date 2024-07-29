// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/** Interface for pneumatics devices. */
public interface PneumaticsBase extends AutoCloseable {
  /**
   * For internal use to get a module for a specific type.
   *
   * @param module module number
   * @param type module type
   * @return module
   */
  static PneumaticsBase getForType(int module, PneumaticsModuleType type) {
    return switch (type) {
      case CTREPCM -> new PneumaticsControlModule(module);
      case REVPH -> new PneumaticHub(module);
    };
  }

  /**
   * For internal use to get the default for a specific type.
   *
   * @param type module type
   * @return module default
   */
  static int getDefaultForType(PneumaticsModuleType type) {
    return switch (type) {
      case CTREPCM -> SensorUtil.getDefaultCTREPCMModule();
      case REVPH -> SensorUtil.getDefaultREVPHModule();
    };
  }

  /**
   * Sets solenoids on a pneumatics module.
   *
   * @param mask Bitmask indicating which solenoids to set. The LSB represents solenoid 0.
   * @param values Bitmask indicating the desired states of the solenoids. The LSB represents
   *     solenoid 0.
   */
  void setSolenoids(int mask, int values);

  /**
   * Gets a bitmask of solenoid values.
   *
   * @return Bitmask containing the state of the solenoids. The LSB represents solenoid 0.
   */
  int getSolenoids();

  /**
   * Get module number for this module.
   *
   * @return module number
   */
  int getModuleNumber();

  /**
   * Get a bitmask of disabled solenoids.
   *
   * @return Bitmask indicating disabled solenoids. The LSB represents solenoid 0.
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

  /**
   * Returns whether the compressor is active or not.
   *
   * @return True if the compressor is on - otherwise false.
   */
  boolean getCompressor();

  /**
   * Returns the state of the pressure switch.
   *
   * @return True if pressure switch indicates that the system is not full, otherwise false.
   */
  boolean getPressureSwitch();

  /**
   * Returns the current drawn by the compressor in amps.
   *
   * @return The current drawn by the compressor.
   */
  double getCompressorCurrent();

  /** Disables the compressor. */
  void disableCompressor();

  /**
   * Enables the compressor in digital mode using the digital pressure switch. The compressor will
   * turn on when the pressure switch indicates that the system is not full, and will turn off when
   * the pressure switch indicates that the system is full.
   */
  void enableCompressorDigital();

  /**
   * If supported by the device, enables the compressor in analog mode. This mode uses an analog
   * pressure sensor connected to analog channel 0 to cycle the compressor. The compressor will turn
   * on when the pressure drops below {@code minPressure} and will turn off when the pressure
   * reaches {@code maxPressure}. This mode is only supported by the REV PH with the REV Analog
   * Pressure Sensor connected to analog channel 0.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value.
   */
  void enableCompressorAnalog(double minPressure, double maxPressure);

  /**
   * If supported by the device, enables the compressor in hybrid mode. This mode uses both a
   * digital pressure switch and an analog pressure sensor connected to analog channel 0 to cycle
   * the compressor. This mode is only supported by the REV PH with the REV Analog Pressure Sensor
   * connected to analog channel 0.
   *
   * <p>The compressor will turn on when <i>both</i>:
   *
   * <ul>
   *   <li>The digital pressure switch indicates the system is not full AND
   *   <li>The analog pressure sensor indicates that the pressure in the system is below the
   *       specified minimum pressure.
   * </ul>
   *
   * <p>The compressor will turn off when <i>either</i>:
   *
   * <ul>
   *   <li>The digital pressure switch is disconnected or indicates that the system is full OR
   *   <li>The pressure detected by the analog sensor is greater than the specified maximum
   *       pressure.
   * </ul>
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value and the pressure switch indicates that the system is not full.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value or the pressure switch is disconnected or indicates that the system is
   *     full.
   */
  void enableCompressorHybrid(double minPressure, double maxPressure);

  /**
   * If supported by the device, returns the raw voltage of the specified analog input channel.
   *
   * <p>This function is only supported by the REV PH. On CTRE PCM, this will return 0.
   *
   * @param channel The analog input channel to read voltage from.
   * @return The voltage of the specified analog input channel.
   */
  double getAnalogVoltage(int channel);

  /**
   * If supported by the device, returns the pressure (in PSI) read by an analog pressure sensor on
   * the specified analog input channel.
   *
   * <p>This function is only supported by the REV PH. On CTRE PCM, this will return 0.
   *
   * @param channel The analog input channel to read pressure from.
   * @return The pressure (in PSI) read by an analog pressure sensor on the specified analog input
   *     channel.
   */
  double getPressure(int channel);

  /**
   * Returns the active compressor configuration.
   *
   * @return The active compressor configuration.
   */
  CompressorConfigType getCompressorConfigType();

  /**
   * Check if a solenoid channel is valid.
   *
   * @param channel Channel to check
   * @return True if channel exists
   */
  boolean checkSolenoidChannel(int channel);

  /**
   * Check to see if the solenoids marked in the bitmask can be reserved, and if so, reserve them.
   *
   * @param mask The bitmask of solenoids to reserve. The LSB represents solenoid 0.
   * @return 0 if successful; mask of solenoids that couldn't be allocated otherwise
   */
  int checkAndReserveSolenoids(int mask);

  /**
   * Unreserve the solenoids marked in the bitmask.
   *
   * @param mask The bitmask of solenoids to unreserve. The LSB represents solenoid 0.
   */
  void unreserveSolenoids(int mask);

  /**
   * Reserve the compressor.
   *
   * @return true if successful; false if compressor already reserved
   */
  boolean reserveCompressor();

  /** Unreserve the compressor. */
  void unreserveCompressor();

  @Override
  void close();

  /**
   * Create a solenoid object for the specified channel.
   *
   * @param channel solenoid channel
   * @return Solenoid object
   */
  Solenoid makeSolenoid(int channel);

  /**
   * Create a double solenoid object for the specified channels.
   *
   * @param forwardChannel solenoid channel for forward
   * @param reverseChannel solenoid channel for reverse
   * @return DoubleSolenoid object
   */
  DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel);

  /**
   * Create a compressor object.
   *
   * @return Compressor object
   */
  Compressor makeCompressor();
}
