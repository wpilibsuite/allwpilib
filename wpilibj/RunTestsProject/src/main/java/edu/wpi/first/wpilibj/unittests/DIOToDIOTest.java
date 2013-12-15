package edu.wpi.first.wpilibj.unittests;

import Assemblies.DIOCrossConnect;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

/**
 * Tests the crossed DIO lines between the two digital sidecars on the
 * version 2.1 test stand by Chris Jennings.
 * 
 * @author Paul
 */
public class DIOToDIOTest extends TestClass {
    private DigitalOutput m_AOut;
    private DigitalInput m_AIn;

    public String getName() {
        return "DIOToDIOTest";
    }

    public String[] getTags() {
        return new String[] {RunTests.Tags.Lifecycle.INPRODUCTION,
                             RunTests.Tags.Type.DIGITAL};
    }

    public void setup() {
        m_AOut = DIOCrossConnect.getOutputA();
        m_AIn = DIOCrossConnect.getInputA();
    }

    {
        new Test("SignalALowDetected") {

            public void run() {
                m_AOut.set(false);
                assertFalse(m_AIn.get());
            }
        };

        new Test("SignalAHighDetected") {

            public void run() {
                m_AOut.set(true);
                assertTrue(m_AIn.get());
            }
        };
    }

    public void teardown() {
        m_AIn.free();
        m_AOut.free();
    }
}
