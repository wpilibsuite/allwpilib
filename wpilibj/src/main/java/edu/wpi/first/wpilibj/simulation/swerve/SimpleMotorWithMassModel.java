package frc.sim.wpiClasses;

import edu.wpi.first.wpilibj.simulation.FlywheelSim;
import edu.wpi.first.wpilibj.system.plant.DCMotor;

class SimpleMotorWithMassModel {
    
    double curDisplacement_Rev;

    FlywheelSim fwSim;

    /**
     * So far - this is just a wrapper around FlywheelSim to get position as an output
     * @param motor
     * @param gearing
     * @param moi
     */
    SimpleMotorWithMassModel(DCMotor motor, double gearing, double moi){
        fwSim = new FlywheelSim(motor, gearing, moi);
    }

    void update(double motorVoltage, double dtSeconds){
        fwSim.setInputVoltage(motorVoltage);
        fwSim.update(dtSeconds);
        curDisplacement_Rev += fwSim.getAngularVelocityRPM() / 60 * dtSeconds; //Add additional state of displacement in a hacky-ish calculation
    }

    /**
     * 
     * @return The present speed of the rotating mass
     */
    double getMechanismSpeed_RPM(){
        return fwSim.getAngularVelocityRPM();
    }

    /**
     * 
     * @return The present current draw of the mechanism
     */
    double getCurrent_A(){
        return fwSim.getCurrentDrawAmps();
    }

    /**
     * 
     * @return The present displacement in Revolutions
     */
    double getMechanismPosition_Rev(){
        return curDisplacement_Rev;
    }


}