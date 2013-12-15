
package edu.wpi.first.testing;

/**
 *
 * @author brad
 */
public interface Connections {
    
    /**
     * Grey Jaguar, Victor and associated motor and sensors
     */
    public static final int GreyJagPWM = 5;
    public static final int GreyJagBrakeCoast = 12;
    public static final int GreyJagLimitFWD = 13;
    public static final int GreyJagLimitRev = 14;
    public static final int VictorPWM = 2;
    public static final int VictorBrakeCoast = 2;
    public static final int GreyJagGearTooth = 7;  // DSC module 2
    public static final int ContinuousTurnPot = 3;
    
    /**
     * 
     */
    public static final int DIOCrossConnectA1 = 8;
    public static final int DIOCrossConnectA2 = 11;
    public static final int DIOCrossConnectB1 = 9;
    public static final int DIOCrossConnectB2 = 10;
    public static final int DIOCrossConnectC1 = 10;
    public static final int DIOCrossConnectC2 = 9;
    public static final int DIOCrossConnectD1 = 11;
    public static final int DIOCrossConnectD2 = 8;


}
