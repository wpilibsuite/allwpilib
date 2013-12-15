/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.unittests;

import Assemblies.FakeCounterSource;
import edu.wpi.first.testing.Connections;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.Counter;

/**
 * @file FakeEncoderTest.java
 * Test the fake encoder classes to verify they are valid for future test use
 * @author Ryan O'Meara
 */
public class FakeEncoderTest extends TestClass implements Connections
{
    private Counter counter;
    private FakeCounterSource fakeEncoder;

    public String getName()
    {
        return "FakeEncoder Test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.DIGITAL };
    }

    public void setup()
    {
        counter = new Counter(DIOCrossConnectD1);
        fakeEncoder = new FakeCounterSource(2, DIOCrossConnectD2);
    }

    public void teardown()
    {
        counter.free();
        fakeEncoder.free();
    }


    {
        new Test("Counting accuracy")
        {

            public void run()
            {
                final int MAXCOUNT = 500;
                counter.reset();
                counter.start();

                fakeEncoder.setCount(MAXCOUNT);
                fakeEncoder.setRate(1);
                fakeEncoder.execute();
                assertEquals(counter.get(), MAXCOUNT);
                counter.stop();

            }
        };
    }
}
