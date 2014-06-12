package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimDigitalInput;
import edu.wpi.first.wpilibj.tables.ITable;

public class DigitalInput implements LiveWindowSendable {
	private SimDigitalInput impl;
	private int m_moduleNumber, m_channel;

	/**
	 * Create an instance of a Digital Input class. Creates a digital input
	 * given a channel and uses the default module.
	 * 
	 * @param channel
	 *            the port for the digital input
	 */
	public DigitalInput(int channel) {
		this(1, channel);
	}

	/**
	 * Create an instance of a Digital Input class. Creates a digital input
	 * given an channel and module.
	 * 
	 * @param moduleNumber
	 *            The number of the digital module to use for this input
	 * @param channel
	 *            the port for the digital input
	 */
	public DigitalInput(int moduleNumber, int channel) {
		impl = new SimDigitalInput("simulator/dio/"+moduleNumber+"/"+channel);
		m_moduleNumber = moduleNumber;
		m_channel = channel;
	}

	/**
	 * Get the value from a digital input channel. Retrieve the value of a
	 * single digital input channel from the FPGA.
	 * 
	 * @return the stats of the digital input
	 */
	public boolean get() {
		return impl.get();
	}

	/**
	 * Get the channel of the digital input
	 * 
	 * @return The GPIO channel number that this object represents.
	 */
	public int getChannel() {
		return m_channel;
	}

	public boolean getAnalogTriggerForRouting() {
		return false;
	}

	/*
	 * Live Window code, only does anything if live window is activated.
	 */
	public String getSmartDashboardType() {
		return "Digital Input";
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
			m_table.putBoolean("Value", get());
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public ITable getTable() {
		return m_table;
	}

	/**
	 * {@inheritDoc}
	 */
	public void startLiveWindowMode() {
	}

	/**
	 * {@inheritDoc}
	 */
	public void stopLiveWindowMode() {
	}
}
