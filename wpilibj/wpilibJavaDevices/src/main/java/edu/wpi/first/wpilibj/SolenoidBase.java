/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.SolenoidJNI;

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
public abstract class SolenoidBase extends SensorBase {

    private ByteBuffer[] m_ports;
    protected int m_moduleNumber; ///< The number of the solenoid module being used.
    protected Resource m_allocated = new Resource(63* SensorBase.kSolenoidChannels);

    /**
     * Constructor.
     *
     * @param moduleNumber The PCM CAN ID
     */
    public SolenoidBase(final int moduleNumber) {
        m_moduleNumber = moduleNumber;
        m_ports = new ByteBuffer[SensorBase.kSolenoidChannels];
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            ByteBuffer port = SolenoidJNI.getPortWithModule((byte) moduleNumber, (byte) i);
            IntBuffer status = IntBuffer.allocate(1);
            m_ports[i] = SolenoidJNI.initializeSolenoidPort(port, status);
            HALUtil.checkStatus(status);
        }
    }

    /**
     * Set the value of a solenoid.
     *
     * @param value The value you want to set on the module.
     * @param mask The channels you want to be affected.
     */
    protected synchronized void set(int value, int mask) {
        IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            int local_mask = 1 << i;
            if ((mask & local_mask) != 0)
                SolenoidJNI.setSolenoid(m_ports[i], (byte) (value & local_mask), status);
        }
        HALUtil.checkStatus(status);
    }

    /**
     * Read all 8 solenoids from the module used by this solenoid as a single byte
     *
     * @return The current value of all 8 solenoids on this module.
     */
    public byte getAll() {
        byte value = 0;
        IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            value |= SolenoidJNI.getSolenoid(m_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }
	/**
	 * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
	 * 
	 *		If a solenoid is shorted, it is added to the blacklist and
	 *		disabled until power cycle, or until faults are cleared.
	 *		@see #clearAllPCMStickyFaults()
	 * 
	 * @return The solenoid blacklist of all 8 solenoids on the module.
	 */
	public byte getPCMSolenoidBlackList() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();

		byte retval = SolenoidJNI.getPCMSolenoidBlackList(m_ports[0], status);
		HALUtil.checkStatus(status);

		return retval;
	}
	/**
	 * @return true if PCM sticky fault is set : The common 
	 *			highside solenoid voltage rail is too low,
	 *	 		most likely a solenoid channel is shorted.
	 */
	public boolean getPCMSolenoidVoltageStickyFault() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();

		boolean retval = SolenoidJNI.getPCMSolenoidVoltageStickyFault(m_ports[0], status);
		HALUtil.checkStatus(status);

		return retval;
	}
	/**
	 * @return true if PCM is in fault state : The common 
	 *			highside solenoid voltage rail is too low,
	 *	 		most likely a solenoid channel is shorted.
	 */
	public boolean getPCMSolenoidVoltageFault() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();

		boolean retval = SolenoidJNI.getPCMSolenoidVoltageFault(m_ports[0], status);
		HALUtil.checkStatus(status);

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
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();

		SolenoidJNI.clearAllPCMStickyFaults(m_ports[0], status);
		HALUtil.checkStatus(status);
	}
}
