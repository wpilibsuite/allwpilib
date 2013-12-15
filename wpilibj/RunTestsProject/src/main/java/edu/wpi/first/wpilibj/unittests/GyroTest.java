/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import Assemblies.RCAssembly;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Timer;

/**
 *
 * @author mwills
 */
public class GyroTest extends TestClass{
    private Gyro gyro;
    private Servo servo;
    public String getName() {
        return "Gyro Test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INDEVEL,
                              RunTests.Tags.Type.ANALOG };
    }
    
    public void setup() {
        gyro = RCAssembly.getGyro();
        servo = RCAssembly.getServo();
    }

    public void teardown() {
        gyro.free();
        servo.free();
    }
    
    {
        new TestUnderDevelopment("Gyro connected to servo panning one way") {
            public void run() {
                servo.set(0);
                Timer.delay(3);
                gyro.reset();
                servo.set(1);
                Timer.delay(3);
                assertEquals(138, gyro.getAngle(), 1);
            }
            
        };
    }
    
    {
        new TestUnderDevelopment("Gyro connected to servo panning back and forth once") {
            public void run() {
                servo.set(0);
                Timer.delay(3);
                gyro.reset();
                servo.set(1);
                Timer.delay(3);
                servo.set(0);
                Timer.delay(3);
                
                assertEquals(0, gyro.getAngle(), .3);
            }
            
        };
    }
    
    {
        new TestUnderDevelopment("Gyro connected to servo panning back and forth 5 times") {
            public void run() {
                servo.set(0);
                Timer.delay(3);
                gyro.reset();
                for(int i = 0; i<5; ++i){
                    servo.set(1);
                    Timer.delay(3);
                    servo.set(0);
                    Timer.delay(3);
                }
                
                assertEquals(0, gyro.getAngle(), 1);
            }
            
        };
    }
    
    {
        new TestUnderDevelopment("Gyro connected to servo panning back and forth to many positions") {
            public void run() {
                servo.set(0);
                Timer.delay(3);
                gyro.reset();
                
                servo.set(0.5);
                Timer.delay(1);
                servo.set(0.75);
                Timer.delay(1);
                servo.set(0.25);
                Timer.delay(1);
                servo.set(1);
                Timer.delay(1);
                servo.set(0);
                Timer.delay(3);
                
                assertEquals(0, gyro.getAngle(), 1.2);
            }
            
        };
    }
    
}
