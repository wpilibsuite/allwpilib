/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.sun.jna.Structure;

/**
 *
 * @author dtjones
 */
public class EllipseDescriptor extends Structure {

    double m_minMajorRadius;
    double m_maxMajorRadius;
    double m_minMinorRadius;
    double m_maxMinorRadius;

    public EllipseDescriptor(double m_minMajorRadius, double m_maxMajorRadius, double m_minMinorRadius, double m_maxMinorRadius) {
        this.m_minMajorRadius = m_minMajorRadius;
        this.m_maxMajorRadius = m_maxMajorRadius;
        this.m_minMinorRadius = m_minMinorRadius;
        this.m_maxMinorRadius = m_maxMinorRadius;
        allocateMemory();
        write();
    }

    /**
     * Free the c memory associated with this object.
     */
    public void free() {
        release();
    }

    public void read() {
        m_minMajorRadius = backingNativeMemory.getDouble(0);
        m_maxMajorRadius = backingNativeMemory.getDouble(8);
        m_minMinorRadius = backingNativeMemory.getDouble(16);
        m_maxMinorRadius = backingNativeMemory.getDouble(24);
    }

    public void write() {
        backingNativeMemory.setDouble(0, m_minMajorRadius);
        backingNativeMemory.setDouble(8, m_maxMajorRadius);
        backingNativeMemory.setDouble(16, m_minMinorRadius);
        backingNativeMemory.setDouble(24, m_maxMinorRadius);

    }

    public int size() {
        return 32;
    }
}
