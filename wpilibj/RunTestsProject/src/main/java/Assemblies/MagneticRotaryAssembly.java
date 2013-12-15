package Assemblies;

import edu.wpi.first.wpilibj.AnalogChannel;
import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Jaguar;

/**
 *
 * @author brad
 */
public class MagneticRotaryAssembly {
    
    public static Jaguar getJaguar() {
        return new Jaguar(1, 6);
    }
    
    public static DigitalOutput getLimitSWFwd() {
        return new DigitalOutput(2, 13);
    }
    
    public static DigitalOutput getLimitSWRev() {
        return new DigitalOutput(2, 14);
    }
    
    public static Counter getGTS() {
        return new Counter(1, 5);
    }
    
    public static Encoder getEncoder() {
        return new Encoder(1, 6, 1, 7);
    }
    
    public static Counter getHallEffect() {
        return new Counter(1, 1);
    }
    
    public AnalogChannel getMagneticEncoder() {
        return new AnalogChannel(2, 6);
    }
}
