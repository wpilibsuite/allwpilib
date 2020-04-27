/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpiutil.math.MathUtil;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.MatrixDimensionException;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

/**
 * This class simulates the motion of a standard swerve drive. To use this sim, users should
 * first call {@link #setModuleAzimuthVoltages(double...)} and {@link #setModuleDriveVoltages(double...)}
 * in simulationPeriodic to set the currently applied voltages to the azimuth (angle) and
 * drive motors. Next, {@link #update(double)} should be called to update the estimated system
 * state. Finally, encoders should be updated with the {@link #getEstimatedModuleStates()} and
 * {@link #getEstimatedChassisSpeed()}. For a full usage example, see the example project.
 */
public class SimSwerveDrive {
  private final SwerveDriveKinematics m_kinematics;
  private final double m_chassisMassKg;
  private final SimSwerveModule[] m_modules;
  private double[] m_moduleDriveVoltages;
  private double[] m_moduleAzimuthVoltages;
  private ChassisSpeeds m_estimatedSpeed = new ChassisSpeeds();
  private Rotation2d m_estimatedHeading = new Rotation2d();

  /**
   * Create a simulated swerve drive. The azimuthSystem can be created using
   * {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#createSingleJointedArmSystem(DCMotor, double, double)}
   * or {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyPositionSystem(double, double)}.
   *
   * <p>Note that with this constructor, all swerve module pods are assumed to use the same
   * azimuth and drive motors.
   *
   * @param chassisMassKg          The mass of the chassis.
   * @param azimuthSystem          A state-space system representing the
   * @param driveMotor             The {@link DCMotor} representing a single swerve module's
   *                               drive motor.
   * @param driveMotorGearing      The gearing between the drive motor and the wheel, written
   *                               as output over input. This should usually be greater than
   *                               1.
   * @param driveWheelRadiusMeters The radius of the drive wheel in meters.
   * @param modulePositions        A list of {@link Translation2d}s representing the
   *                               positions of the modules.
   */
  public SimSwerveDrive(double chassisMassKg, LinearSystem<N2, N1, N1> azimuthSystem,
                        DCMotor driveMotor, double driveMotorGearing,
                        double driveWheelRadiusMeters,
                        Translation2d... modulePositions) {
    this(chassisMassKg, Arrays.stream(modulePositions)
        .map(it -> new SimSwerveModule(driveMotor, driveMotorGearing, driveWheelRadiusMeters, it, azimuthSystem))
        .collect(Collectors.toList()).toArray(new SimSwerveModule[modulePositions.length]));
  }

  public SimSwerveDrive(double chassisMassKg, SimSwerveModule... modules) {
    this.m_chassisMassKg = chassisMassKg;
    this.m_modules = modules;

    this.m_kinematics = new SwerveDriveKinematics(
        Arrays.stream(modules).map(SimSwerveModule::getModulePosition).toArray(Translation2d[]::new));

    m_moduleDriveVoltages = new double[m_modules.length];
    m_moduleAzimuthVoltages = new double[m_modules.length];
  }

  public SwerveDriveKinematics getKinematics() {
    return m_kinematics;
  }


  public ChassisSpeeds getEstimatedChassisSpeed() {
    return m_estimatedSpeed;
  }

  public SwerveModuleState[] getEstimatedModuleStates() {
    return m_kinematics.toSwerveModuleStates(getEstimatedChassisSpeed());
  }

  public SimSwerveModule[] getModules() {
    return m_modules;
  }

  public List<Rotation2d> getEstimatedModuleAngles() {
    return Arrays.stream(m_modules).map(SimSwerveModule::getAzimuthAngle).collect(Collectors.toList());
  }

  public void setModuleDriveVoltages(double... voltages) {
    if (voltages.length != m_modules.length) {
      throw new MatrixDimensionException(String.format("Got the wrong number of inputs. Got %s voltage inputs, but I have %s wheels!", voltages.length, m_modules.length));
    }

    m_moduleDriveVoltages = voltages;
  }

  public void setModuleAzimuthVoltages(double... voltages) {
    if (voltages.length != m_modules.length) {
      throw new MatrixDimensionException(String.format("Got the wrong number of azimuth voltages. Got %s angles, but I have %s wheels!", voltages.length, m_modules.length));
    }

    m_moduleAzimuthVoltages = voltages;
  }

  public void update(double dtSeconds) {
    // First, we determine the wheel speeds necessary to be at the current chassis speeds
    // Then, we use this speed to calculate the force each module exerts on the chassis,
    // Which we use to figure out the acceleration-ish of each wheel. We then integrate this
    // acceleration forward with RK4, and use IK to go back to chassis speeds from our new
    // wheel speeds.

    var wheelSpeeds = m_kinematics.toSwerveModuleStates(m_estimatedSpeed);
    SwerveModuleState[] newState = new SwerveModuleState[m_modules.length];

    for (int i = 0; i < m_modules.length; i++) {
      int finalI = i;
      var newSpeedI = RungeKutta.rungeKutta(
          // F = ma  <==> a = F / m
          (Double x, Double u) -> m_modules[finalI]
              .estimateModuleForceNewtons(wheelSpeeds[finalI].speedMetersPerSecond, m_moduleDriveVoltages[finalI]) / m_chassisMassKg * m_modules.length,
          wheelSpeeds[i].speedMetersPerSecond,
          m_moduleDriveVoltages[i],
          dtSeconds);

      var angle = m_modules[i].update(m_moduleAzimuthVoltages[i], dtSeconds);
      newState[i] = new SwerveModuleState(newSpeedI, angle);
    }

    this.m_estimatedSpeed = m_kinematics.toChassisSpeeds(newState);

    // Integrate heading forward with RK4
    m_estimatedHeading = new Rotation2d(RungeKutta.rungeKutta(
        (double heading) -> this.m_estimatedSpeed.omegaRadiansPerSecond,
        m_estimatedHeading.getRadians(), dtSeconds));
  }

  public void resetChassisSpeeds(ChassisSpeeds chassisSpeeds) {
    this.m_estimatedSpeed = chassisSpeeds;
  }

  public Rotation2d getEstimatedHeading() {
    return m_estimatedHeading;
  }

  public static class SimSwerveModule {
    private final DCMotor m_driveMotor;
    private final double m_driveMotorGearing;
    private final double m_driveWheelRadiusMeters;
    private final Translation2d m_position;

    private final LinearSystem<N2, N1, N1> m_azimuthSystem;
    private Matrix<N2, N1> m_azimuthState = VecBuilder.fill(0, 0);

    /**
     * Construct a SimSwerveModule. The base unit of this model must be radians. This model
     * can be created with {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#createSingleJointedArmSystem(DCMotor, double, double)}
     * or {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyPositionSystem(double, double)}.
     *
     * @param driveMotor             The drive motor as a DCMotor.
     * @param driveMotorGearing      The gearing between the drive motor and wheel. Usually greater than one.
     * @param driveWheelRadiusMeters The radius of the wheel in meters.
     * @param positionMeters         The locations of this wheel relative to the physical center of the
     *                               robot.
     */
    public SimSwerveModule(DCMotor driveMotor, double driveMotorGearing,
                           double driveWheelRadiusMeters, Translation2d positionMeters,
                           LinearSystem<N2, N1, N1> azimuthSystem) {
      this.m_driveMotor = driveMotor;
      this.m_driveMotorGearing = driveMotorGearing;
      this.m_driveWheelRadiusMeters = driveWheelRadiusMeters;
      this.m_position = positionMeters;
      this.m_azimuthSystem = azimuthSystem;
    }

    /**
     * Update the estimated angle of the swerve module.
     *
     * @param azimuthVoltageVolts The voltage applied to the azimuth.
     * @param dtSeconds           Time since last update. Usually 0.020s.
     */
    protected Rotation2d update(double azimuthVoltageVolts, double dtSeconds) {
      m_azimuthState = m_azimuthSystem.calculateX(m_azimuthState, VecBuilder.fill(azimuthVoltageVolts), dtSeconds);
      return new Rotation2d(m_azimuthState.get(0, 0));
    }

    /**
     * Get the currently estimated angle of the swerve module.
     */
    public Rotation2d getAzimuthAngle() {
      return new Rotation2d(MathUtil.normalizeAngle(m_azimuthState.get(0, 0)));
    }

    @SuppressWarnings("LocalVariableName")
    public double estimateModuleForceNewtons(double wheelVelocityMps, double wheelVoltage) {
      // By the elevator equations of motion presented in Controls Engineering in FRC,
      // F_m = (G Kt)/(R r) Voltage - (G^2 Kt)/(R r^2 Kv) velocity
      var G = m_driveMotorGearing;
      var r = m_driveWheelRadiusMeters;
      var m = m_driveMotor;

      final var v = G * m.m_KtNMPerAmp / (m.m_rOhms * r) * wheelVoltage - (G * G * m.m_KtNMPerAmp)
          / (m.m_rOhms * r * r * m.m_KvRadPerSecPerVolt) * wheelVelocityMps;
      System.out.println(v);
      return v;
    }

    public Translation2d getModulePosition() {
      return m_position;
    }
  }
}
