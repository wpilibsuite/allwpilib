package edu.wpi.first.wpilibj.system.plant;

/**
 * Holds the constants for a DC motor.
 */
public class DCMotor {

  public final double m_nominalVoltageVolts;
  public final double m_stallTorqueNewtonMeters;
  public final double m_stallCurrentAmps;
  public final double m_freeCurrentAmps;
  public final double m_freeSpeedRadPerSec;
  @SuppressWarnings("MemberName")
  public final double m_rOhms;
  @SuppressWarnings("MemberName")
  public final double m_KvRadPerSecPerVolt;
  @SuppressWarnings("MemberName")
  public final double m_KtNMPerAmp;


  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltageVolts Voltage at which the motor constants were measured.
   * @param stallTorqueNewtonMeters    Current draw when stalled.
   * @param stallCurrentAmps   Current draw when stalled.
   * @param freeCurrentAmps    Current draw under no load.
   * @param freeSpeedRadPerSec      Angular velocity under no load.
   */
  public DCMotor(double nominalVoltageVolts,
                 double stallTorqueNewtonMeters,
                 double stallCurrentAmps,
                 double freeCurrentAmps,
                 double freeSpeedRadPerSec) {
    this.m_nominalVoltageVolts = nominalVoltageVolts;
    this.m_stallTorqueNewtonMeters = stallTorqueNewtonMeters;
    this.m_stallCurrentAmps = stallCurrentAmps;
    this.m_freeCurrentAmps = freeCurrentAmps;
    this.m_freeSpeedRadPerSec = freeSpeedRadPerSec;

    this.m_rOhms = nominalVoltageVolts / stallCurrentAmps;
    this.m_KvRadPerSecPerVolt = freeSpeedRadPerSec / (nominalVoltageVolts - m_rOhms
            * freeCurrentAmps);
    this.m_KtNMPerAmp = stallTorqueNewtonMeters / stallCurrentAmps;
  }

  /**
   * Return a gearbox of CIM motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getCIM(int numMotors) {
    return new DCMotor(12,
            2.42 * numMotors, 133,
            2.7, 5310 / 60.0 * 2.0 * Math.PI);
  }

  /**
   * Return a gearbox of 775Pro motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getVex775Pro(int numMotors) {
    return new DCMotor(12,
            0.71 * numMotors, 134,
            0.7, 18730 / 60.0 * 2.0 * Math.PI);
  }

  /**
   * Return a gearbox of NEO motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getNEO(int numMotors) {
    return new DCMotor(12, 2.6 * numMotors,
            105, 1.8, 5676 / 60d * 2 * Math.PI);
  }
}
