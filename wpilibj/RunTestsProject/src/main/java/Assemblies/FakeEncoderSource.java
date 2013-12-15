/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package Assemblies;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Timer;

/**
 * @file FakeEncoderSource.java
 * Emulates a quadrature encoder
 * @author Ryan O'Meara
 */
public class FakeEncoderSource
{

    private Thread m_task;
    private int m_count;
    private int m_mSec;
    private boolean m_forward;
    private DigitalOutput m_outputA, m_outputB;
    private boolean allocatedOutputs;

    /**
     * Thread object that allows emulation of a quadrature encoder
     */
    private class QuadEncoderThread extends Thread
    {

        FakeEncoderSource m_encoder;

        QuadEncoderThread(FakeEncoderSource encode)
        {
            m_encoder = encode;
        }

        public void run()
        {

            DigitalOutput lead, lag;

            m_encoder.m_outputA.set(false);
            m_encoder.m_outputB.set(false);

            if (m_encoder.isForward())
            {
                lead = m_encoder.m_outputA;
                lag = m_encoder.m_outputB;
            } else
            {
                lead = m_encoder.m_outputB;
                lag = m_encoder.m_outputA;
            }

            try
            {
                for (int i = 0; i < m_encoder.m_count; i++)
                {
                    lead.set(true);
                    Thread.sleep(m_encoder.m_mSec);
                    lag.set(true);
                    Thread.sleep(m_encoder.m_mSec);
                    lead.set(false);
                    Thread.sleep(m_encoder.m_mSec);
                    lag.set(false);
                    Thread.sleep(m_encoder.m_mSec);
                }
            } catch (InterruptedException e)
            {
            }
        }
    }

    public FakeEncoderSource(int slotA, int portA, int slotB, int portB)
    {
        m_outputA = new DigitalOutput(slotA, portA);
        m_outputB = new DigitalOutput(slotB, portB);
        allocatedOutputs = true;
        initQuadEncoder();
    }

    public FakeEncoderSource(int portA, int portB)
    {
        m_outputA = new DigitalOutput(portA);
        m_outputB = new DigitalOutput(portB);
        allocatedOutputs = true;
        initQuadEncoder();
    }

    public FakeEncoderSource(DigitalOutput iA, DigitalOutput iB)
    {
        m_outputA = iA;
        m_outputB = iB;
        allocatedOutputs = false;
        initQuadEncoder();
    }

    public void free()
    {
        m_task = null;
        if (allocatedOutputs) {
            m_outputA.free();
            m_outputB.free();
        }
    }

    /**
     * Common initialization code
     */
    private void initQuadEncoder()
    {
        m_mSec = 1;
        m_forward = true;
        m_task = new QuadEncoderThread(this);
        m_outputA.set(false);
        m_outputB.set(false);
    }

    /**
     * Starts the thread
     */
    public void start()
    {
        m_task.start();
    }

    /**
     * Waits for thread to end
     */
    public void complete()
    {
        try
        {
            m_task.join();
        } catch (InterruptedException e)
        {
        }
        m_task = new QuadEncoderThread(this);
        Timer.delay(.5);
    }

    /**
     * Runs and waits for thread to end before returning
     */
    public void execute()
    {
        start();
        complete();
    }

    /**
     * Rate of pulses to send
     * @param mSec Pulse Rate
     */
    public void setRate(int mSec)
    {
        m_mSec = mSec;
    }

    /**
     * Set the number of pulses to simulate
     * @param count Pulse count
     */
    public void setCount(int count)
    {
        m_count = count;
    }

    /**
     * Set which direction the encoder simulates motion in
     * @param isForward Whether to simulate forward motion
     */
    public void setForward(boolean isForward)
    {
        m_forward = isForward;
    }

    /**
     * Accesses whether the encoder is simulating forward motion
     * @return Whether the simulated motion is in the forward direction
     */
    public boolean isForward()
    {
        return m_forward;
    }
}
