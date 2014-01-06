/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRC_NetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.parsing.IDevice;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Compressor object.
 * The Compressor object is designed to handle the operation of the compressor, pressure sensor and
 * relay for a FIRST robot pneumatics system. The Compressor object starts a task which runs in the
 * backround and periodically polls the pressure sensor and operates the relay that controls the
 * compressor.
 */
public class Compressor extends SensorBase implements IDevice, LiveWindowSendable {

    private DigitalInput m_pressureSwitch;
    private Relay m_relay;
    private boolean m_enabled;
    private Thread m_task;
    private boolean m_run = true;

    /**
     * Internal thread.
     *
     * Task which checks the compressor pressure switch and operates the relay as necessary
     * depending on the pressure.
     *
     * Do not call this function directly.
     */
    private class CompressorThread extends Thread {

        Compressor m_compressor;

        CompressorThread(Compressor comp) {
            m_compressor = comp;
        }

        public void run() {
            while (m_run) {
                if (m_compressor.enabled()) {
                    m_compressor.setRelayValue(!m_compressor.getPressureSwitchValue() ? Relay.Value.kOn : Relay.Value.kOff);
                } else {
                    m_compressor.setRelayValue(Relay.Value.kOff);
                }
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                }
            }
        }
    }

    /**
     * Initialize the Compressor object.
     * This method is the common initialization code for all the constructors for the Compressor
     * object. It takes the relay channel and pressure switch channel and spawns a task that polls the
     * compressor and sensor.
     *
     * You MUST start the compressor by calling the start() method.
     */
    private void initCompressor(final int pressureSwitchSlot,
                                final int pressureSwitchChannel,
                                final int compresssorRelaySlot,
                                final int compressorRelayChannel) {

        m_enabled = false;
        m_pressureSwitch = new DigitalInput(pressureSwitchSlot, pressureSwitchChannel);
        m_relay = new Relay(compresssorRelaySlot, compressorRelayChannel, Relay.Direction.kForward);

        UsageReporting.report(tResourceType.kResourceType_Compressor, 0);

        m_task = new CompressorThread(this);
        m_task.start();
    }

    /**
     * Compressor constructor.
     * Given a fully specified relay channel and pressure switch channel, initialize the Compressor object.
     *
     * You MUST start the compressor by calling the start() method.
     *
     * @param pressureSwitchSlot The module that the pressure switch is attached to.
     * @param pressureSwitchChannel The GPIO channel that the pressure switch is attached to.
     * @param compresssorRelaySlot The module that the compressor relay is attached to.
     * @param compressorRelayChannel The relay channel that the compressor relay is attached to.
     */
    public Compressor(final int pressureSwitchSlot,
                      final int pressureSwitchChannel,
                      final int compresssorRelaySlot,
                      final int compressorRelayChannel) {
        initCompressor(pressureSwitchSlot,
                       pressureSwitchChannel,
                       compresssorRelaySlot,
                       compressorRelayChannel);
    }

    /**
     * Compressor constructor.
     * Given a relay channel and pressure switch channel (both in the default digital module), initialize
     * the Compressor object.
     *
     * You MUST start the compressor by calling the start() method.
     *
     * @param pressureSwitchChannel The GPIO channel that the pressure switch is attached to.
     * @param compressorRelayChannel The relay channel that the compressor relay is attached to.
     */
    public Compressor(final int pressureSwitchChannel, final int compressorRelayChannel) {
        initCompressor(getDefaultDigitalModule(),
                       pressureSwitchChannel,
                       getDefaultDigitalModule(),
                       compressorRelayChannel);
    }

    /**
     * Delete the Compressor object.
     * Delete the allocated resources for the compressor and kill the compressor task that is polling
     * the pressure switch.
     */
    public void free() {
        m_run = false;
        try {
            m_task.join();
        } catch (InterruptedException e) {}
        m_pressureSwitch.free();
        m_relay.free();
        m_pressureSwitch = null;
        m_relay = null;
    }

    /**
     * Operate the relay for the compressor.
     * Change the value of the relay output that is connected to the compressor motor.
     * This is only intended to be called by the internal polling thread.
     * @param relayValue the value to set the relay to
     */
    public void setRelayValue(Relay.Value relayValue) {
        m_relay.set(relayValue);
    }

    /**
     * Get the pressure switch value.
     * Read the pressure switch digital input.
     *
     * @return The current state of the pressure switch.
     */
    public boolean getPressureSwitchValue() {
        return m_pressureSwitch.get();
    }

    /**
     * Start the compressor.
     * This method will allow the polling loop to actually operate the compressor. The
     * is stopped by default and won't operate until starting it.
     */
    public void start() {
        m_enabled = true;
    }

    /**
     * Stop the compressor.
     * This method will stop the compressor from turning on.
     */
    public void stop() {
        m_enabled = false;
    }

    /**
     * Get the state of the enabled flag.
     * Return the state of the enabled flag for the compressor and pressure switch
     * combination.
     *
     * @return The state of the compressor thread's enable flag.
     */
    public boolean enabled() {
        return m_enabled;
    }

    /*
     * Live Window code, only does anything if live window is activated.
     */
    public String getSmartDashboardType() {
        return "Compressor";
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
    public ITable getTable() {
        return m_table;
    }

    /**
     * {@inheritDoc}
     */
    public void updateTable() {
        if (m_table != null) {
            m_table.putBoolean("Enabled", m_enabled);
            m_table.putBoolean("Pressure Switch", getPressureSwitchValue());
        }
    }

    /**
     * {@inheritDoc}
     */
    public void startLiveWindowMode() {}

    /**
     * {@inheritDoc}
     */
    public void stopLiveWindowMode() {}
}
