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
public class CurveOptions extends Structure {

    int m_extractionMode;
    int m_threshold;
    int m_filterSize;
    int m_minLength;
    int m_rowStepSize;
    int m_columnStepSize;
    int m_maxEndPointGap;
    int m_onlyClosed;
    int m_subPixelAccuracy;

    public CurveOptions(int m_extractionMode, int m_threshold, int m_filterSize, int m_minLength, int m_rowStepSize, int m_columnStepSize, int m_maxEndPointGap, int m_onlyClosed, int m_subPixelAccuracy) {
        this.m_extractionMode = m_extractionMode;
        this.m_threshold = m_threshold;
        this.m_filterSize = m_filterSize;
        this.m_minLength = m_minLength;
        this.m_rowStepSize = m_rowStepSize;
        this.m_columnStepSize = m_columnStepSize;
        this.m_maxEndPointGap = m_maxEndPointGap;
        this.m_onlyClosed = m_onlyClosed;
        this.m_subPixelAccuracy = m_subPixelAccuracy;
        allocateMemory();
        write();
    }



    public void read() {
    }

    public void write() {
        backingNativeMemory.setInt(0, m_extractionMode);
        backingNativeMemory.setInt(4, m_threshold);
        backingNativeMemory.setInt(8, m_filterSize);
        backingNativeMemory.setInt(12, m_minLength);
        backingNativeMemory.setInt(16, m_rowStepSize);
        backingNativeMemory.setInt(20, m_columnStepSize);
        backingNativeMemory.setInt(24, m_maxEndPointGap);
        backingNativeMemory.setInt(28, m_onlyClosed);
        backingNativeMemory.setInt(32, m_subPixelAccuracy);
    }

    public int size() {
        return 36;
    }

    public void free() {
        release();
    }

}
