/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.PDPJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import tec.uom.se.quantity.Quantities;
import tec.uom.se.unit.Units;

import javax.measure.Unit;
import javax.measure.quantity.ElectricCurrent;
import javax.measure.quantity.ElectricPotential;
import javax.measure.quantity.Energy;
import javax.measure.quantity.Power;
import javax.measure.quantity.Temperature;

/**
 * Class for getting voltage, current, temperature, power and energy from the Power Distribution
 * Panel over CAN.
 */
public class PowerDistributionPanel extends SensorBase implements LiveWindowSendable {

  private final int m_module;

  /**
   * Constructor.
   *
   * @param module The CAN ID of the PDP
   */
  public PowerDistributionPanel(int module) {
    m_module = module;
    checkPDPModule(module);
    PDPJNI.initializePDP(module);
  }

  /**
   * Constructor.  Uses the default CAN ID (0).
   */
  public PowerDistributionPanel() {
    this(0);
  }

  /**
   * Query the input voltage of the PDP.
   *
   * @return The voltage of the PDP in volts
   * @deprecated Will be replaced with a method that returns a Quantity of ElectricPotential in 2019
   */
  @Deprecated
  public double getVoltage() {
    return PDPJNI.getPDPVoltage(m_module);
  }

  /*
  public Quantity<ElectricPotential> getVoltage() {
    return Quantities.getQuantity(PDPJNI.getPDPVoltage(m_module), SI.VOLT);
  }
  */

  /**
   * Query the input voltage of the PDP.
   *
   * @param unit The unit to return as
   * @return The electric potential of the PDP
   */
  public double getVoltage(Unit<ElectricPotential> unit) {
    return Quantities.getQuantity(getVoltage(), Units.VOLT).to(unit).getValue().doubleValue();
  }

  /**
   * Query the temperature of the PDP.
   *
   * @return The temperature of the PDP in degrees Celsius
   * @deprecated Will be replaced with a method that returns a Quantity of Temperature in 2019
   */
  @Deprecated
  public double getTemperature() {
    return PDPJNI.getPDPTemperature(m_module);
  }

  /*
  public Quantity<Temperature> getTemperature() {
    return Quantities.getQuantity(PDPJNI.getPDPTemperature(m_module), SI.CELSIUS);
  }
  */

  /**
   * Query the temperature of the PDP.
   *
   * @param unit The unit to return as
   * @return The temperature of the PDP
   */
  public double getTemperature(Unit<Temperature> unit) {
    return Quantities.getQuantity(getTemperature(), Units.CELSIUS)
        .to(unit).getValue().doubleValue();
  }

  /**
   * Query the current of a single channel of the PDP.
   *
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   * @deprecated Will be replaced with a method that returns a Quantity of ElectricCurrent in 2019
   */
  @Deprecated
  public double getCurrent(int channel) {
    double current = PDPJNI.getPDPChannelCurrent((byte) channel, m_module);

    checkPDPChannel(channel);

    return current;
  }

  /*
  public Quantity<ElectricCurrent> getCurrent(int channel) {
    checkPDPChannel(channel);

    return Quantities.getQuantity(PDPJNI.getPDPChannelCurrent((byte) channel, m_module), SI.AMPERE);
  }
  */

  /**
   * Query the current of a single channel of the PDP.
   *
   * @param unit The unit to return as
   * @return The current of one of the PDP channels (channels 0-15)
   */
  public double getCurrent(int channel, Unit<ElectricCurrent> unit) {
    return Quantities.getQuantity(getCurrent(channel), Units.AMPERE)
        .to(unit).getValue().doubleValue();
  }

  /**
   * Query the current of all monitored PDP channels (0-15).
   *
   * @return The current of all the channels in Amperes
   * @deprecated Will be replaced with a method that returns a Quantity of ElectricCurrent in 2019
   */
  @Deprecated
  public double getTotalCurrent() {
    return PDPJNI.getPDPTotalCurrent(m_module);
  }

  /*
  public Quantity<ElectricCurrent> getTotalCurrent() {
    return Quantities.getQuantity(PDPJNI.getPDPTotalCurrent(m_module), SI.AMPERE);
  }
  */

  /**
   * Query the current of all monitored PDP channels (0-15).
   *
   * @param unit The unit to return as
   * @return The current of all the channels
   */
  public double getTotalCurrent(Unit<ElectricCurrent> unit) {
    return Quantities.getQuantity(getTotalCurrent(), Units.AMPERE)
        .to(unit).getValue().doubleValue();
  }

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @return the total power in Watts
   * @deprecated Will be replaced with a method that returns a Quantity of Power in 2019
   */
  @Deprecated
  public double getTotalPower() {
    return PDPJNI.getPDPTotalPower(m_module);
  }

  /*
  public Quantity<Power> getTotalPower() {
    return Quantities.getQuantity(PDPJNI.getPDPTotalPower(m_module), SI.WATT);
  }
  */

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @param unit The unit to return as
   * @return the total power in Watts
   */
  public double getTotalPower(Unit<Power> unit) {
    return Quantities.getQuantity(getTotalPower(), Units.WATT).to(unit).getValue().doubleValue();
  }

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @return the total energy in Joules
   * @deprecated Will be replaced with a method that returns a Quantity of Energy in 2019
   */
  @Deprecated
  public double getTotalEnergy() {
    return PDPJNI.getPDPTotalEnergy(m_module);
  }

  /*
  public Quantity<Energy> getTotalEnergy() {
    return Quantities.getQuantity(PDPJNI.getPDPTotalEnergy(m_module), SI.JOULE);
  }
  */

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @param unit The unit to return as
   * @return the total energy in Joules
   */
  public double getTotalEnergy(Unit<Energy> unit) {
    return Quantities.getQuantity(PDPJNI.getPDPTotalEnergy(m_module), Units.JOULE)
        .to(unit).getValue().doubleValue();
  }

  /**
   * Reset the total energy to 0.
   */
  public void resetTotalEnergy() {
    PDPJNI.resetPDPTotalEnergy(m_module);
  }

  /**
   * Clear all PDP sticky faults.
   */
  public void clearStickyFaults() {
    PDPJNI.clearPDPStickyFaults(m_module);
  }

  @Override
  public String getSmartDashboardType() {
    return "PowerDistributionPanel";
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
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
      m_table.putNumber("Chan0", getCurrent(0, Units.AMPERE));
      m_table.putNumber("Chan1", getCurrent(1, Units.AMPERE));
      m_table.putNumber("Chan2", getCurrent(2, Units.AMPERE));
      m_table.putNumber("Chan3", getCurrent(3, Units.AMPERE));
      m_table.putNumber("Chan4", getCurrent(4, Units.AMPERE));
      m_table.putNumber("Chan5", getCurrent(5, Units.AMPERE));
      m_table.putNumber("Chan6", getCurrent(6, Units.AMPERE));
      m_table.putNumber("Chan7", getCurrent(7, Units.AMPERE));
      m_table.putNumber("Chan8", getCurrent(8, Units.AMPERE));
      m_table.putNumber("Chan9", getCurrent(9, Units.AMPERE));
      m_table.putNumber("Chan10", getCurrent(10, Units.AMPERE));
      m_table.putNumber("Chan11", getCurrent(11, Units.AMPERE));
      m_table.putNumber("Chan12", getCurrent(12, Units.AMPERE));
      m_table.putNumber("Chan13", getCurrent(13, Units.AMPERE));
      m_table.putNumber("Chan14", getCurrent(14, Units.AMPERE));
      m_table.putNumber("Chan15", getCurrent(15, Units.AMPERE));
      m_table.putNumber("Voltage", getVoltage(Units.VOLT));
      m_table.putNumber("TotalCurrent", getTotalCurrent(Units.AMPERE));
    }
  }

  /**
   * PDP doesn't have to do anything special when entering the LiveWindow. {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {
  }

  /**
   * PDP doesn't have to do anything special when exiting the LiveWindow. {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {
  }

}
