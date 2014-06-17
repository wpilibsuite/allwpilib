/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimFloatInput;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Analog channel class.
 */
public class AnalogChannel extends SensorBase implements PIDSource,
		LiveWindowSendable {

	private SimFloatInput m_impl;
	private int m_moduleNumber, m_channel;

	/**
	 * Construct an analog channel on the default module.
	 *
	 * @param channel
	 *            The channel number to represent.
	 */
	public AnalogChannel(final int channel) {
		this(1, channel);
	}

	/**
	 * Construct an analog channel on a specified module.
	 *
	 * @param moduleNumber
	 *            The digital module to use (1 or 2).
	 * @param channel
	 *            The channel number to represent.
	 */
	public AnalogChannel(final int moduleNumber, final int channel) {
		m_channel = channel;
		m_moduleNumber = moduleNumber;
		m_impl = new SimFloatInput("simulator/analog/"+moduleNumber+"/"+channel);

		LiveWindow.addSensor("Analog", moduleNumber, channel, this);
	}

	/**
	 * Channel destructor.
	 */
	public void free() {
		m_channel = 0;
		m_moduleNumber = 0;
	}

	/**
	 * Get a scaled sample straight from this channel on the module. The value
	 * is scaled to units of Volts using the calibrated scaling data from
	 * getLSBWeight() and getOffset().
	 *
	 * @return A scaled sample straight from this channel on the module.
	 */
	public double getVoltage() {
		return m_impl.get();
	}

	/**
	 * Get a scaled sample from the output of the oversample and average engine
	 * for this channel. The value is scaled to units of Volts using the
	 * calibrated scaling data from getLSBWeight() and getOffset(). Using
	 * oversampling will cause this value to be higher resolution, but it will
	 * update more slowly. Using averaging will cause this value to be more
	 * stable, but it will update more slowly.
	 *
	 * @return A scaled sample from the output of the oversample and average
	 *         engine for this channel.
	 */
	public double getAverageVoltage() {
		return getVoltage();
	}

	/**
	 * Get the channel number.
	 *
	 * @return The channel number.
	 */
	public int getChannel() {
		return m_channel;
	}

	/**
	 * Gets the number of the analog module this channel is on.
	 *
	 * @return The module number of the analog module this channel is on.
	 */
	public int getModuleNumber() {
		return m_moduleNumber;
	}

    /**
	 * Get the average value for use with PIDController
	 *
	 * @return the average value
	 */
	public double pidGet() {
		return getAverageVoltage();
	}

    /**
	 * Live Window code, only does anything if live window is activated.
	 */
	public String getSmartDashboardType() {
		return "Analog Input";
	}

	private ITable m_table;

	/**
	 * {@inheritDoc}
	 */
	public void initTable(ITable subtable) {
		m_table = subtable;
		updateTable();
	}

	/**
	 * {@inheritDoc}
	 */
	public void updateTable() {
		if (m_table != null) {
			m_table.putNumber("Value", getAverageVoltage());
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public ITable getTable() {
		return m_table;
	}

	/**
	 * Analog Channels don't have to do anything special when entering the
	 * LiveWindow. {@inheritDoc}
	 */
	public void startLiveWindowMode() {
	}

	/**
	 * Analog Channels don't have to do anything special when exiting the
	 * LiveWindow. {@inheritDoc}
	 */
	public void stopLiveWindowMode() {
	}
}
