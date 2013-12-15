/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Assemblies;

import edu.wpi.first.testing.SlowServo;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Servo;

/**
 *
 * @author mwills
 */
public class RCAssembly {
    
    public static Servo getServo(){
        return new SlowServo(1, 1);
    }
    
    public static Gyro getGyro(){
        return new Gyro(1, 1);
    }
    
    /*public static SPIAccelerometer getAccelerometer(){
        return new SPIAccelerometer(2, 1, 2, 3, 4, 5, 6);
    }*/
}
