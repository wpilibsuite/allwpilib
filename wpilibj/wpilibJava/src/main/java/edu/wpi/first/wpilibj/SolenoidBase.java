/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;

import com.sun.jna.Pointer;

import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.parsing.IDeviceController;

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
public abstract class SolenoidBase extends SensorBase implements IDeviceController {

    private Pointer[] m_ports;
    protected int m_moduleNumber; ///< The number of the solenoid module being used.
    // XXX: Move this to be both HAL calls
    protected Resource m_allocated = new Resource(HALLibrary.solenoid_kNumDO7_0Elements.get() * SensorBase.kSolenoidChannels);

    /**
     * Constructor.
     *
     * @param moduleNumber The number of the solenoid module to use.
     */
    public SolenoidBase(final int moduleNumber) {
        m_moduleNumber = moduleNumber;
        m_ports = new Pointer[SensorBase.kSolenoidChannels];
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            Pointer port = HALLibrary.getPortWithModule((byte) moduleNumber, (byte) (i+1));
            IntBuffer status = IntBuffer.allocate(1);
            m_ports[i] = HALLibrary.initializeSolenoidPort(port, status);
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
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            int local_mask = 1 << i;
            if ((mask & local_mask) != 0)
                HALLibrary.setSolenoid(m_ports[i], (byte) (value & local_mask), status);
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
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kSolenoidChannels; i++) {
            value |= HALLibrary.getSolenoid(m_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Read all 8 solenoids in the default solenoid module as a single byte
     *
     * @return The current value of all 8 solenoids on the default module.
     */
    public static byte getAllFromDefaultModule() {
        return getAllFromModule(getDefaultSolenoidModule());
    }

    /**
     * Read all 8 solenoids in the specified solenoid module as a single byte
     *
     * @return The current value of all 8 solenoids on the specified module.
     */
    public static byte getAllFromModule(int moduleNumber) {
        checkSolenoidModule(moduleNumber);
        throw new RuntimeException("Not supported right now.");
    }
}
