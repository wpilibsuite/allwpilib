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
 * @file FakeCounterSource.java
 * Simulates an encoder for testing purposes
 * @author Ryan O'Meara
 */
public class FakeCounterSource
{

    private Thread m_task;
    private int m_count;
    private int m_mSec;
    private DigitalOutput m_output;

    /**
     * Thread object that allows emulation of an encoder
     */
    private class EncoderThread extends Thread
    {

        FakeCounterSource m_encoder;

        EncoderThread(FakeCounterSource encode)
        {
            m_encoder = encode;
        }

        public void run()
        {
            m_encoder.m_output.set(false);
            try
            {
                for (int i = 0; i < m_encoder.m_count; i++)
                {
                    Thread.sleep(m_encoder.m_mSec);
                    m_encoder.m_output.set(true);
                    Thread.sleep(m_encoder.m_mSec);
                    m_encoder.m_output.set(false);
                }
            } catch (InterruptedException e)
            {
            }
        }
    }

    /**
     * Create a fake encoder on a given port
     * @param port The port the encoder is supposed to be on
     */
    public FakeCounterSource(int port)
    {
        m_output = new DigitalOutput(port);
        initEncoder();
    }

    /**
     * Create a new fake encoder on the indicated slot and port
     * @param slot Slot to create on
     * @param port THe port that the encoder is supposably on
     */
    public FakeCounterSource(int slot, int port)
    {
        m_output = new DigitalOutput(slot, port);
        initEncoder();
    }

    /**
     * Destroy Object with minimum memory leak
     */
    public void free()
    {
        m_task = null;
        m_output.free();
    }

    /**
     * Common initailization code
     */
    private void initEncoder()
    {
        m_mSec = 1;
        m_task = new EncoderThread(this);
        m_output.set(false);
    }

    /**
     * Starts the thread execution task
     */
    public void start()
    {
        m_task.start();
    }

    /**
     * Waits for the thread to complete
     */
    public void complete()
    {
        try
        {
            m_task.join();
        } catch (InterruptedException e)
        {
        }
        m_task = new EncoderThread(this);
        Timer.delay(.5);
    }

    /**
     * Starts and completes a task set - does not return until thred has finished
     * its operations
     */
    public void execute()
    {
        start();
        complete();
    }

    /**
     * Sets the count to run encoder
     * @param count The count to emulate to the controller
     */
    public void setCount(int count)
    {
        m_count = count;
    }

    /**
     * Specify the rate to send pulses
     * @param mSec The rate to send out pulses at
     */
    public void setRate(int mSec)
    {
        m_mSec = mSec;
    }
}
