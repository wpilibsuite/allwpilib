/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tModuleType;

/**
 * Base class for AnalogModule and DigitalModule.
 *
 * @author dtjones
 */
public class Module extends SensorBase {

    /**
     * An array holding the object representing each module
     */
    protected static Module[] m_modules = new Module[tModuleType.kModuleType_Solenoid * FRCNetworkCommunicationsLibrary.kMaxModuleNumber + (FRCNetworkCommunicationsLibrary.kMaxModuleNumber - 1)];
    /**
     * The module number of the module
     */
    protected int m_moduleNumber;
    protected int m_moduleType;

    /**
     * Constructor.
     *
     * @param moduleNumber The number of this module (1 or 2).
     */
    protected Module(int moduleType, final int moduleNumber) {
        m_modules[toIndex(moduleType, moduleNumber)] = this;
        m_moduleNumber = moduleNumber;
    }

    /**
     * Gets the module number associated with a module.
     *
     * @return The module's number.
     */
    public int getModuleNumber() {
        return m_moduleNumber;
    }

    /**
     * Gets the module type associated with a module.
     *
     * @return The module's type.
     */
    public int getModuleType() {
        return m_moduleType;
    }

    /**
     * Static module singleton factory.
     *
     * @param moduleType The type of the module represented.
     * @param moduleNumber The module index within the module type.
     * @return the module
     */
    public static Module getModule(int moduleType, int moduleNumber) {
        if(m_modules[toIndex(moduleType, moduleNumber)] == null) {
            if(moduleType == tModuleType.kModuleType_Analog) {
                new AnalogModule(moduleNumber);
            } else if (moduleType == tModuleType.kModuleType_Digital) {
                new DigitalModule(moduleNumber);
                /*        } else if (moduleType.equals(ModulePresence.ModuleType.kSolenoid)) {
                              new Sol
                 */
            } else {
                throw new RuntimeException("A module of type "+moduleType+" with module index "+moduleNumber);
            }
        }
        return m_modules[toIndex(moduleType, moduleNumber)];
    }

    /**
     * Create an index into m_modules based on type and number
     *
     * @param moduleType The type of the module represented.
     * @param moduleNumber The module index within the module type.
     * @return The index into m_modules.
     */
    private static int toIndex(int moduleType, int moduleNumber) {
        if(moduleNumber == 0 || moduleNumber > FRCNetworkCommunicationsLibrary.kMaxModuleNumber)
            return 0;
        return moduleType * FRCNetworkCommunicationsLibrary.kMaxModuleNumber + (moduleNumber - 1);
    }
}
