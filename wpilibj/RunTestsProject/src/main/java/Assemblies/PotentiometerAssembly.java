/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Assemblies;

import edu.wpi.first.wpilibj.AnalogChannel;
import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Victor;

/**
 *
 * @author Fred
 */
public class PotentiometerAssembly {
    
    public static Jaguar getJaguar(){
        return new Jaguar(1,5);
    }
    
    public static DigitalOutput getLimSWFwd(){
        return new DigitalOutput(1, 13);
    }
    
    public static DigitalOutput getLimSWRev(){
        return new DigitalOutput(1, 14);
    }
    
    public static Victor getVictor(){
        return new Victor(1, 2);
    }
    
    public static AnalogChannel getPotentiometer(){
        return new AnalogChannel(1, 3);
    }
    
    public static Counter getHallEffect(){
        return new Counter(1, 4);
    }
    
    public static Counter getGearTooth(){
        return new Counter(2, 7);
    }
}
