/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import Assemblies.MagneticRotaryAssembly;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Timer;

/**
 *
 * @author brad
 */
public class EncoderTest extends TestClass {
    
    private Encoder encoder;
    private Jaguar jag;
    private DigitalOutput fwdLimit;
    private DigitalOutput revLimit;
    private Counter gts;

    public String getName() {
        return "Encoder test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.DIGITAL,
                              RunTests.Tags.Type.PWM,
        };
    }
    
    public void setup() {
        encoder = MagneticRotaryAssembly.getEncoder();
        jag = MagneticRotaryAssembly.getJaguar();
        fwdLimit = MagneticRotaryAssembly.getLimitSWFwd();
        revLimit = MagneticRotaryAssembly.getLimitSWRev();
        gts = MagneticRotaryAssembly.getGTS();
        fwdLimit.set(false);
        revLimit.set(false);
    }

    public void teardown() {
        encoder.free();
        jag.free();
        fwdLimit.free();
        revLimit.free();
        gts.free();
    }
    
    {
        new Test("Compare Encoder with Geartooth sensor") {

           /**
             * Drive forward for a fixed time
             */
            private int driveForward(double time) {
                encoder.reset();
                encoder.start();
                jag.set(1);
                Timer.delay(time);
                jag.set(0);
                return encoder.get();
            }

            public void run() {
                gts.reset();
                gts.start();
                driveForward(2);
                Timer.delay(2);
                System.out.println("Gear tooth sensor: " + gts.get());
                System.out.println("Encoder: " + encoder.get());
            }
            
        };
    }

    {
        new Test("Drive forward then backward") {
            
            /**
             * Drive forward for a fixed time
             */
            private int driveForward(double time) {
                encoder.reset();
                encoder.start();
                jag.set(1);
                Timer.delay(time);
                jag.set(0);
                return encoder.get();
            }

            /**
             * Drive forward then let the motor stop, then drive forward again. Compare the results.
             * Then drive backwards, and see that the results have gone back near zero.
             * 
             * This is pretty cheesy, and needs to be replaced with a test that compares the gear tooth
             * sensor with the encoder.
             */
            public void run() {
                
                int pass1 = driveForward(1);
                assertEquals(1064, pass1, 50);
                
                Timer.delay(2);                 // wait for motor to stop
                
                int pass2 = driveForward(1);
                assertEquals(0, pass1-pass2, (int)(pass1*0.05));
                
                jag.set(-1);
                Timer.delay(1);
                jag.set(0);
                assertEquals(0, encoder.get(), (int)(pass1*0.10));
            }
        };
    }
}
