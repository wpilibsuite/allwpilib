package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.CTREPCMJNI;

public class PneumaticsControlModule implements PneumaticsBase {
    private int m_handle;
    private final int m_module;

    public PneumaticsControlModule(final int module) {
        m_handle = CTREPCMJNI.initialize(module);
        m_module = module;
    }

    @Override
    public void close() {
        CTREPCMJNI.free(m_handle);
        m_handle = 0;
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
}
