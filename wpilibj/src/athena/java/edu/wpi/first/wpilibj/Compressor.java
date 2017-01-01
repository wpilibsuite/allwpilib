/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.CompressorJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class for operating a compressor connected to a PCM (Pneumatic Control Module). The PCM will
 * automatically run in closed loop mode by default whenever a {@link Solenoid} object is created.
 * For most cases, a Compressor object does not need to be instantiated or used in a robot program.
 * This class is only required in cases where the robot program needs a more detailed status of the
 * compressor or to enable/disable closed loop control.
 *
 * <p>Note: you cannot operate the compressor directly from this class as doing so would circumvent
 * the safety provided by using the pressure switch and closed loop control. You can only turn off
 * closed loop control, thereby stopping the compressor from operating.
 */
public class Compressor extends SensorBase implements LiveWindowSendable {
  private int m_compressorHandle;
  private byte m_module;

  /**
   * Makes a new instance of the compressor using the provided CAN device ID.  Use this constructor
   * when you have more than one PCM.
   *
   * @param module The PCM CAN device ID (0 - 62 inclusive)
   */
  public Compressor(int module) {
    m_table = null;
    m_module = (byte) module;

    m_compressorHandle = CompressorJNI.initializeCompressor((byte) module);
  }

  /**
   * Makes a new instance of the compressor using the default PCM ID of 0.
   *
   * <p>Additional modules can be supported by making a new instance and {@link #Compressor(int)
   * specifying the CAN ID.}
   */
  public Compressor() {
    this(getDefaultSolenoidModule());
  }

  /**
   * Start the compressor running in closed loop control mode.
   *
   * <p>Use the method in cases where you would like to manually stop and start the compressor for
   * applications such as conserving battery or making sure that the compressor motor doesn't start
   * during critical operations.
   */
  public void start() {
    setClosedLoopControl(true);
  }

  /**
   * Stop the compressor from running in closed loop control mode.
   *
   * <p>Use the method in cases where you would like to manually stop and start the compressor for
   * applications such as conserving battery or making sure that the compressor motor doesn't start
   * during critical operations.
   */
  public void stop() {
    setClosedLoopControl(false);
  }

  /**
   * Get the status of the compressor.
   *
   * @return true if the compressor is on
   */
  public boolean enabled() {
    return CompressorJNI.getCompressor(m_compressorHandle);
  }

  /**
   * Get the pressure switch value.
   *
   * @return true if the pressure is low
   */
  public boolean getPressureSwitchValue() {
    return CompressorJNI.getCompressorPressureSwitch(m_compressorHandle);
  }

  /**
   * Get the current being used by the compressor.
   *
   * @return current consumed by the compressor in amps
   */
  public double getCompressorCurrent() {
    return CompressorJNI.getCompressorCurrent(m_compressorHandle);
  }

  /**
   * Set the PCM in closed loop control mode.
   *
   * @param on if true sets the compressor to be in closed loop control mode (default)
   */
  public void setClosedLoopControl(boolean on) {
    CompressorJNI.setCompressorClosedLoopControl(m_compressorHandle, on);
  }

  /**
   * Gets the current operating mode of the PCM.
   *
   * @return true if compressor is operating on closed-loop mode
   */
  public boolean getClosedLoopControl() {
    return CompressorJNI.getCompressorClosedLoopControl(m_compressorHandle);
  }

  /**
   * If PCM is in fault state : Compressor Drive is disabled due to compressor current being too
   * high.
   *
   * @return true if PCM is in fault state.
   */
  public boolean getCompressorCurrentTooHighFault() {
    return CompressorJNI.getCompressorCurrentTooHighFault(m_compressorHandle);
  }

  /**
   * If PCM sticky fault is set : Compressor is disabled due to compressor current being too
   * high.
   *
   * @return true if PCM sticky fault is set.
   */
  public boolean getCompressorCurrentTooHighStickyFault() {
    return CompressorJNI.getCompressorCurrentTooHighStickyFault(m_compressorHandle);
  }

  /**
   * @return true if PCM sticky fault is set : Compressor output appears to be shorted.
   */
  public boolean getCompressorShortedStickyFault() {
    return CompressorJNI.getCompressorShortedStickyFault(m_compressorHandle);
  }

  /**
   * @return true if PCM is in fault state : Compressor output appears to be shorted.
   */
  public boolean getCompressorShortedFault() {
    return CompressorJNI.getCompressorShortedFault(m_compressorHandle);
  }

  /**
   * If PCM sticky fault is set : Compressor does not appear to be wired, i.e. compressor is not
   * drawing enough current.
   *
   * @return true if PCM sticky fault is set.
   */
  public boolean getCompressorNotConnectedStickyFault() {
    return CompressorJNI.getCompressorNotConnectedStickyFault(m_compressorHandle);
  }

  /**
   * If PCM is in fault state : Compressor does not appear to be wired, i.e. compressor is not
   * drawing enough current.
   *
   * @return true if PCM is in fault state.
   */
  public boolean getCompressorNotConnectedFault() {
    return CompressorJNI.getCompressorNotConnectedFault(m_compressorHandle);
  }

  /**
   * Clear ALL sticky faults inside PCM that Compressor is wired to.
   *
   * <p>If a sticky fault is set, then it will be persistently cleared. The compressor might
   * momentarily disable while the flags are being cleared. Doo not call this method too
   * frequently, otherwise normal compressor functionality may be prevented.
   *
   * <p>If no sticky faults are set then this call will have no effect.
   */
  public void clearAllPCMStickyFaults() {
    CompressorJNI.clearAllPCMStickyFaults(m_module);
  }

  @Override
  public void startLiveWindowMode() {
  }

  @Override
  public void stopLiveWindowMode() {
  }

  @Override
  public String getSmartDashboardType() {
    return "Compressor";
  }

  private ITable m_table;

  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  @Override
  public ITable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putBoolean("Enabled", enabled());
      m_table.putBoolean("Pressure Switch", getPressureSwitchValue());
    }
  }
}
