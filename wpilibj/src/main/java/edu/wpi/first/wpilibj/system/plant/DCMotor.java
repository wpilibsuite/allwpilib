/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system.plant;

public class DCMotor {
    public DCMotor(double nominalVoltageVolts,
                   double stallTorqueNewtonMeters,
                   double stallCurrentAmps,
                   double freeCurrentAmps,
                   double freeSpeedRadPerSec) {
        this.nominalVoltageVolts = nominalVoltageVolts;
        this.stallTorqueNewtonMeters = stallTorqueNewtonMeters;
        this.stallCurrentAmps = stallCurrentAmps;
        this.freeCurrentAmps = freeCurrentAmps;
        this.freeSpeedRadPerSec = freeSpeedRadPerSec;

        this.Rohms = nominalVoltageVolts / stallCurrentAmps;
        this.KvRadPerSecPerVolt = freeSpeedRadPerSec / (nominalVoltageVolts - Rohms * freeCurrentAmps);
        this.KtNMPerAmp = stallTorqueNewtonMeters / stallCurrentAmps;
    }

    public final double nominalVoltageVolts;
    public final double stallTorqueNewtonMeters;
    public final double stallCurrentAmps;
    public final double freeCurrentAmps;
    public final double freeSpeedRadPerSec;

    public final double Rohms;

    public final double KvRadPerSecPerVolt;

    public final double KtNMPerAmp;

    public static DCMotor getCIM(int numMotors) {
        return new DCMotor(12,
                2.42 * numMotors, 133,
                2.7, 5310 / 60.0 * 2.0 * Math.PI);
    }

    public static DCMotor getVex775Pro(int numMotors) {
        return new DCMotor(12,
                0.71 * numMotors, 134,
                0.7, 18730 / 60.0 * 2.0 * Math.PI);
    }



}
