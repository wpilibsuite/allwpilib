// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.CTREPCMJNI;

public class PneumaticsControlModule implements PneumaticsBase {
    private final int m_handle;
    private final int m_module;

    public PneumaticsControlModule() {
        this(SensorUtil.getDefaultCTREPCMModule());
    }

    public PneumaticsControlModule(int module) {
        m_handle = CTREPCMJNI.initialize(module);
        m_module = module;
    }

    @Override
    public void close() throws Exception {
        CTREPCMJNI.free(m_handle);
    }

    public boolean getCompressor() {
        return CTREPCMJNI.getCompressor(m_handle);
    }

    public void setClosedLoopControl(boolean enabled) {
        CTREPCMJNI.setClosedLoopControl(m_handle, enabled);
    }

    public boolean getClosedLoopControl() {
        return CTREPCMJNI.getClosedLoopControl(m_handle);
    }

    public boolean getPressureSwitch() {
        return CTREPCMJNI.getPressureSwitch(m_handle);
    }

    public double getCompressorCurrent() {
        return CTREPCMJNI.getCompressorCurrent(m_handle);
    }

    public boolean getCompressorCurrentTooHighFault() {
        return CTREPCMJNI.getCompressorCurrentTooHighFault(m_handle);
    }

    public boolean getCompressorCurrentTooHighStickyFault() {
        return CTREPCMJNI.getCompressorCurrentTooHighStickyFault(m_handle);
    }

    public boolean getCompressorShortedFault() {
        return CTREPCMJNI.getCompressorShortedFault(m_handle);
    }

    public boolean getCompressorShortedStickyFault() {
        return CTREPCMJNI.getCompressorShortedStickyFault(m_handle);
    }

    public boolean getCompressorNotConnectedFault() {
        return CTREPCMJNI.getCompressorNotConnectedFault(m_handle);
    }

    public boolean getCompressorNotConnectedStickyFault() {
        return CTREPCMJNI.getCompressorNotConnectedStickyFault(m_handle);
    }

    @Override
    public void setSolenoids(int mask, int values) {
        CTREPCMJNI.setSolenoids(m_handle, mask, values);
    }

    @Override
    public int getSolenoids() {
        return CTREPCMJNI.getSolenoids(m_handle);
    }

    @Override
    public int getModuleNumber() {
        return m_module;
    }

    @Override
    public int getSolenoidDisabledList() {
        return CTREPCMJNI.getSolenoidDisabledList(m_handle);
    }

    public boolean getSolenoidVoltageFault() {
        return CTREPCMJNI.getSolenoidVoltageFault(m_handle);
    }

    public boolean getSolenoidVoltageStickyFault() {
        return CTREPCMJNI.getSolenoidVoltageStickyFault(m_handle);
    }

    public void clearAllStickyFaults() {
        CTREPCMJNI.clearAllStickyFaults(m_handle);
    }

    @Override
    public void fireOneShot(int index) {
         CTREPCMJNI.fireOneShot(m_handle, index);
    }

    @Override
    public void setOneShotDuration(int index, int durMs) {
        CTREPCMJNI.setOneShotDuration(m_handle, index, durMs);
    }

    @Override
    public boolean checkSolenoidChannel(int channel) {
        // TODO Auto-generated method stub
        return false;
    }
}
