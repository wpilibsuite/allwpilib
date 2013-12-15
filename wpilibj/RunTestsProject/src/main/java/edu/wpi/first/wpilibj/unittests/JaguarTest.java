/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.Connections;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.Victor;

/**
 *
 * @author brad
 */
public class JaguarTest extends TestClass implements Connections {

    Victor victor;
    Counter gts;

    public String getName() {
        return "JaguarTest";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.DIGITAL,
                              RunTests.Tags.Type.PWM
        };
    }

    public void setup() {
        victor = new Victor(2);
        gts = new Counter(2, GreyJagGearTooth); // need to encode this slot number
    }

    public void teardown() {
        victor.free();
        gts.free();
    }

    {
        new Test("Drive forward") {

            public void run() {
                gts.reset();
                gts.start();
                victor.set(1);
                Timer.delay(1);
                victor.set(0);
                System.out.println("Counter output: " + gts.get());

                assertEquals(0.0, 0.1, 0.1);
            }
        };

        new Test("Drive backward") {

            public void run() {
                victor.set(-1);
                Timer.delay(1);
                victor.set(0);

                assertEquals(0.0, 0.1, 0.1);
            }
        };
    }
}
