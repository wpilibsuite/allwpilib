package frc.sim.wpiClasses;

import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.util.Units;

class MotorGearboxWheelSim {
    
    DCMotor motor;
    double gearRatio;
    double wheelRadius_m;
    double curGroundForce_N;
    double wheelRotations_rad;
    double gearboxFricCoef_NmPerRadPerSec;
    double prevWheelRotationalSpeed_radPerSec;

    
    double wheelSpeed_RPM;
    double motorSpeed_RPM;

    /**
     * 1D simulation of  a motor, gearbox, and wheel.
     * Provides an output torque, given an input speed and voltage.
     * @param motor_in Motor set used to provide power input.
     * @param gearRatio_in Gearbox Ratio. Gearboxes which slow down motors (ie all of them) should have this greater than 1.0.
     * @param wheelRadius_m_in Radius of the wheel in meters
     * @param gearboxFricCoef_NmPerRadPerSec_in Kinetic Friction Losses in the gearbox (expressed in units of Nm of "fighting" force per radian per second of motor speed). Set to 0 if you're awesome with white lithium grease, make it positive if your freshman maybe forget the grease sometimes.
     */
    public MotorGearboxWheelSim(DCMotor motor_in, double gearRatio_in, double wheelRadius_m_in, double gearboxFricCoef_NmPerRadPerSec_in){
        motor = motor_in;
        gearRatio = gearRatio_in;
        wheelRadius_m = wheelRadius_m_in;
        gearboxFricCoef_NmPerRadPerSec = gearboxFricCoef_NmPerRadPerSec_in;
    }

    public void update(double groundVelocity_mps, double motorVoltage, double dtSeconds){

        double wheelRotationalSpeed_radPerSec = groundVelocity_mps / wheelRadius_m;
        double motorRotationalSpeed_radPerSec = wheelRotationalSpeed_radPerSec * gearRatio;

        double motorTorque_Nm = motor.KtNMPerAmp * motor.getCurrent(motorRotationalSpeed_radPerSec, motorVoltage);

        double gearboxFrictionalTorque_Nm = motorRotationalSpeed_radPerSec * gearboxFricCoef_NmPerRadPerSec;
        double curWheelTorque_Nm = motorTorque_Nm * gearRatio  - gearboxFrictionalTorque_Nm; //div by 1/torque ratio 
        
        curGroundForce_N = curWheelTorque_Nm / wheelRadius_m / 2;

        wheelRotations_rad += (wheelRotationalSpeed_radPerSec + prevWheelRotationalSpeed_radPerSec)/2 * dtSeconds; //Trapezoidal integration

        prevWheelRotationalSpeed_radPerSec = wheelRotationalSpeed_radPerSec;

        wheelSpeed_RPM = Units.radiansPerSecondToRotationsPerMinute(wheelRotationalSpeed_radPerSec);
        motorSpeed_RPM = Units.radiansPerSecondToRotationsPerMinute(motorRotationalSpeed_radPerSec);
    }

    public double getPosition_Rev(){
        return wheelRotations_rad / 2 / Math.PI;
    }

    public double getGroundForce_N(){
        return curGroundForce_N;
    }

}