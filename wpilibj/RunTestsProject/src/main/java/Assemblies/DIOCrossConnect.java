/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Assemblies;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

/**
 *
 * @author brad
 */
public class DIOCrossConnect {
    
     /**
     * Cross connected Digital I/O lines.
     * The A1, B1, C1, and D1 are digital module 1 and
     * A2, B2, C2, and D2 are digital module 2
     */
    public static final int DIOCrossConnectA1 = 8;
    public static final int DIOCrossConnectA2 = 11;
    public static final int DIOCrossConnectB1 = 9;
    public static final int DIOCrossConnectB2 = 10;
    public static final int DIOCrossConnectC1 = 10;
    public static final int DIOCrossConnectC2 = 9;
    public static final int DIOCrossConnectD1 = 11;
    public static final int DIOCrossConnectD2 = 8;

    public static DigitalOutput getOutputA() {
        return new DigitalOutput(DIOCrossConnectA1);
    }
    
    public static DigitalInput getInputA() {
        return new DigitalInput(2, DIOCrossConnectA2);
    }
    
    public static DigitalOutput getOutputB() {
        return new DigitalOutput(DIOCrossConnectB1);
    }
    
    public static DigitalInput getInputB() {
        return new DigitalInput(2, DIOCrossConnectB2);
    }
    
    public static DigitalOutput getOutputC() {
        return new DigitalOutput(DIOCrossConnectC1);
    }
    
    public static DigitalInput getInputC() {
        return new DigitalInput(2, DIOCrossConnectC2);
    }
    
    public static DigitalOutput getOutputD() {
        return new DigitalOutput(DIOCrossConnectD1);
    }
    
    public static DigitalInput getInputD() {
        return new DigitalInput(2, DIOCrossConnectD2);
    }
}
