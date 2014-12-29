package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.SensorBase;
import edu.wpi.first.wpilibj.hal.CompressorJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class for operating the PCM (Pneumatics compressor module)
 * The PCM automatically will run in close-loop mode by default whenever a Solenoid object is 
 * created. For most cases the Compressor object does not need to be
 * instantiated or used in a robot program.
 * 
 * This class is only required in cases where more detailed status or to enable/disable
 * closed loop control. Note: you cannot operate the compressor directly from this class as
 * doing so would circumvent the safety provided in using the pressure switch and closed
 * loop control. You can only turn off closed loop control, thereby stopping the compressor
 * from operating.
 */
public class Compressor extends SensorBase implements LiveWindowSendable {
	private ByteBuffer m_pcm;

	/**
	 * Create an instance of the Compressor
	 * @param pcmId
	 * 		The PCM CAN device ID. Most robots that use PCM will have a single module. Use this
	 * 		for supporting a second module other than the default.
	 */
	public Compressor(int pcmId) {
		initCompressor(pcmId);
	}

	/**
	 * Create an instance of the Compressor
	 * Makes a new instance of the compressor using the default PCM ID (0). Additional modules can be
	 * supported by making a new instance and specifying the CAN ID
	 */
	public Compressor() {
		initCompressor(getDefaultSolenoidModule());
	}

	private void initCompressor(int module) {
		m_table = null;

		m_pcm = CompressorJNI.initializeCompressor((byte)module);
	}

	/**
	 * Start the compressor running in closed loop control mode
	 * Use the method in cases where you would like to manually stop and start the compressor
	 * for applications such as conserving battery or making sure that the compressor motor
	 * doesn't start during critical operations.
	 */
	public void start() {
		setClosedLoopControl(true);
	}

	/**
	 * Stop the compressor from running in closed loop control mode.
	 * Use the method in cases where you would like to manually stop and start the compressor
	 * for applications such as conserving battery or making sure that the compressor motor
	 * doesn't start during critical operations.
	 */
	public void stop() {
		setClosedLoopControl(false);
	}

	/**
	 * Get the enabled status of the compressor
	 * @return true if the compressor is on
	 */
	public boolean enabled() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean on = CompressorJNI.getCompressor(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return on;
	}

	/**
	 * Get the current pressure switch value
	 * @return true if the pressure is low by reading the pressure switch that is plugged into the PCM
	 */
	public boolean getPressureSwitchValue() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean on = CompressorJNI.getPressureSwitch(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return on;
	}

	/**
	 * Get the current being used by the compressor
	 * @return current consumed in amps for the compressor motor
	 */
	public float getCompressorCurrent() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		float current = CompressorJNI.getCompressorCurrent(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return current;
	}

	/**
	 * Set the PCM in closed loop control mode
	 * @param on
	 * 			If true sets the compressor to be in closed loop control mode otherwise
	 * 			normal operation of the compressor is disabled.
	 */
	public void setClosedLoopControl(boolean on) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		CompressorJNI.setClosedLoopControl(m_pcm, on, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Gets the current operating mode of the PCM
	 * @return true if compressor is operating on closed-loop mode, otherwise return false.
	 */
	public boolean getClosedLoopControl() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean on = CompressorJNI.getClosedLoopControl(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return on;
	}

	/**
	 * @return true if PCM is in fault state : Compressor Drive is 
	 *			disabled due to compressor current being too high.
	 */
	public boolean getCompressorCurrentTooHighFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorCurrentTooHighFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * @return true if PCM sticky fault is set : Compressor Drive is 
	 *			disabled due to compressor current being too high.
	 */
	public boolean getCompressorCurrentTooHighStickyFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorCurrentTooHighStickyFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * @return true if PCM sticky fault is set : Compressor output 
	 *			appears to be shorted.
	 */
	public boolean getCompressorShortedStickyFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorShortedStickyFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * @return true if PCM is in fault state : Compressor output 
	 *			appears to be shorted.
	 */
	public boolean getCompressorShortedFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorShortedFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * @return true if PCM sticky fault is set : Compressor does not 
	 *			appear to be wired, i.e. compressor is
	 * 			not drawing enough current.
	 */
	public boolean getCompressorNotConnectedStickyFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorNotConnectedStickyFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * @return true if PCM is in fault state : Compressor does not 
	 *			appear to be wired, i.e. compressor is
	 * 			not drawing enough current.
	 */
	public boolean getCompressorNotConnectedFault() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		boolean retval = CompressorJNI.getCompressorNotConnectedFault(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

		return retval;
	}
	/**
	 * Clear ALL sticky faults inside PCM that Compressor is wired to.
	 *
	 * If a sticky fault is set, then it will be persistently cleared.  Compressor drive
	 *		maybe momentarily disable while flags are being cleared. Care should be 
	 *		taken to not call this too frequently, otherwise normal compressor 
	 *		functionality may be prevented.
	 *
	 * If no sticky faults are set then this call will have no effect.
	 */
	public void clearAllPCMStickyFaults() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		CompressorJNI.clearAllPCMStickyFaults(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
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
