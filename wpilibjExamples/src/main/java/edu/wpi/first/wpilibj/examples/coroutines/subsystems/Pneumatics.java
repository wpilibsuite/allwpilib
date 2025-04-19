// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.coroutines.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.commandsv3.Command;
import edu.wpi.first.wpilibj.commandsv3.RequireableResource;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;

/** Subsystem for managing the compressor, pressure sensor, etc. */
public class Pneumatics extends RequireableResource {
  // External analog pressure sensor
  // product-specific voltage->pressure conversion, see product manual
  // in this case, 250(V/5)-25
  // the scale parameter in the AnalogPotentiometer constructor is scaled from 1 instead of 5,
  // so if r is the raw AnalogPotentiometer output, the pressure is 250r-25
  static final double kScale = 250;
  static final double kOffset = -25;
  private final AnalogPotentiometer m_pressureTransducer =
      new AnalogPotentiometer(/* the AnalogIn port*/ 2, kScale, kOffset);

  // Compressor connected to a PCM with a default CAN ID (0)
  // Note: the compressor is enabled by default
  private final Compressor m_compressor = new Compressor(PneumaticsModuleType.CTREPCM);

  public Pneumatics() {
    super("Pneumatics");
    var tab = Shuffleboard.getTab("Pneumatics");
    tab.addDouble("External Pressure [PSI]", this::getPressure);
  }

  /**
   * Query the analog pressure sensor.
   *
   * @return the measured pressure, in PSI
   */
  private double getPressure() {
    // Get the pressure (in PSI) from an analog pressure sensor connected to the RIO.
    return m_pressureTransducer.get();
  }

  /**
   * Disable the compressor closed-loop control. The compressor can be re-enabled with
   * {@link #enableCompressor()}.
   *
   * @return command
   */
  public Command disableCompressor() {
    return run((coroutine) -> {
      m_compressor.disable();
      coroutine.park();
    }).named("Disable Compressor");
  }

  /**
   * Enable the compressor closed-loop control. The compressor can be disabled again with
   * {@link #disableCompressor()}.
   *
   * @return command
   */
  public Command enableCompressor() {
    return run((coroutine) -> {
      m_compressor.enableDigital();
      coroutine.park();
    }).named("Enable Compressor");
  }
}
