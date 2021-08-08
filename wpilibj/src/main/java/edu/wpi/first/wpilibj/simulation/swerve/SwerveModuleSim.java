package edu.wpi.first.wpilibj.simulation.swerve;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.wpilibj.drive.Vector2d;

public class SwerveModuleSim {

    private SimpleMotorWithMassModel m_azmthMotor;
    private MotorGearboxWheelSim m_wheelMotor;

    private final double m_azimuthEncGearRatio;    //Motor-to-azimuth-encoder reduction
    private final double m_wheelEncGearRatio;      //Motor-to-wheel-encoder reduction
    private final double m_treadStaticFricForce;
    private final double m_treadKineticFricForce;
    //TODO - make the "how much grease" factor configurable?
    private final double m_wheelGearboxLossFactor = 0.01;

    Pose2d m_prevModulePose = null;
    Pose2d m_curModulePose  = null;
    //Positive = in curAngle_deg, Negative = opposite of curAngle_deg
    double m_curLinearSpeedMpS = 0;
    //0 = toward front, 90 = toward left, 180 = toward back, 270 = toward right
    Rotation2d m_curAzmthAngle = Rotation2d.fromDegrees(0);

    double m_crossTreadFricForceMag = 0;
    double m_crossTreadVelMag = 0;
    double m_crossTreadForceMag = 0;

    double m_wheelVoltage;
    double m_azmthVoltage;

    /**
     * Simulates a single swerve module's behavior.
     * @param azimuthMotor Motor driving the azimuth angle portion of the mechanism
     * @param wheelMotor Motor driving the wheel to rotate
     * @param wheelRadiusM Wheel radius, in meters
     * @param azimuthGearRatio Motor rotations per one azimuth module
     *      rotation. Should be greater than zero
     * @param wheelGearRatio Motor rotations per one wheel rotation.
     *      Should be greater than zero
     * @param azimuthEncGearRatio Encoder rotations per one azimuth module rotation.
     *      Should be 1.0 if you have a good swerve module.
     * @param wheelEncGearRatio Encoder rotations per one wheel rotation.
     * @param treadStaticCoefFric Static coefficient of friction of the wheel
     *      tread and ground interface
     * @param treadKineticCoefFric Kinetic coefficient of friction of the wheel
     *      tread and ground interface
     * @param moduleNormalForce Downward force the robot and its mechanisms exert on this module.
     * @param azimuthEffectiveMOI Effective moment of inertia of the azimuth module.
     *      The Azimuth motion is modeled as a simple flywheel.
     */
    public SwerveModuleSim(
        DCMotor azimuthMotor,
        DCMotor wheelMotor,
        double wheelRadiusM,
        double azimuthGearRatio,    
        double wheelGearRatio,      
        double azimuthEncGearRatio, 
        double wheelEncGearRatio,   
        double treadStaticCoefFric,
        double treadKineticCoefFric,
        double moduleNormalForce,
        double azimuthEffectiveMOI
    ) {
        this.m_azmthMotor = new SimpleMotorWithMassModel(azimuthMotor,
                                                         azimuthGearRatio,
                                                         azimuthEffectiveMOI);
        this.m_wheelMotor = new MotorGearboxWheelSim(wheelMotor,
                                                     wheelGearRatio,
                                                     wheelRadiusM,
                                                     m_wheelGearboxLossFactor);
  
        this.m_azimuthEncGearRatio   = azimuthEncGearRatio;
        this.m_wheelEncGearRatio     = wheelEncGearRatio;
        this.m_treadStaticFricForce  = treadStaticCoefFric * moduleNormalForce;
        this.m_treadKineticFricForce = treadKineticCoefFric * moduleNormalForce;

        this.reset(new Pose2d());
    }

    public void setInputVoltages(double wheelVoltage, double azmthVoltage) {
        this.m_wheelVoltage = wheelVoltage;
        this.m_azmthVoltage = azmthVoltage;
    }

    public double getAzimuthEncoderPositionRev() {
        return m_azmthMotor.getMechanismPositionRev() * m_azimuthEncGearRatio;
    }

    public double getWheelEncoderPositionRev() {
        return m_wheelMotor.getPositionRev() * m_wheelEncGearRatio;
    }

    void reset(Pose2d initModulePose) {
        m_prevModulePose = m_curModulePose = initModulePose;
        m_curLinearSpeedMpS = 0;
        m_curAzmthAngle = Rotation2d.fromDegrees(0);
    }

    void update(double dtSeconds) {

        Vector2d azimuthUnitVec = new Vector2d(1 , 0);
        azimuthUnitVec.rotate(m_curAzmthAngle.getDegrees());

        // Assume the wheel does not lose traction along its wheel direction (on-tread)
        double velocityAlongAzimuth = getModRelTransVel(dtSeconds).dot(azimuthUnitVec);

        m_wheelMotor.update(velocityAlongAzimuth, m_wheelVoltage, dtSeconds);
        m_azmthMotor.update(m_azmthVoltage, dtSeconds);

        // Assume idealized azimuth control - no "twist" force at contact
        //  patch from friction or robot motion.
        m_curAzmthAngle = Rotation2d.fromDegrees(m_azmthMotor.getMechanismPositionRev() * 360);
    }

 
    /** Get a vector of the velocity of the module's contact patch moving across the field. */
    Vector2d getModRelTransVel(double dtSeconds) {
        double xvel = (m_curModulePose.getTranslation().getX()
                       - m_prevModulePose.getTranslation().getX()) / dtSeconds;
        double yvel = (m_curModulePose.getTranslation().getY()
                       - m_prevModulePose.getTranslation().getY()) / dtSeconds;
        Vector2d moduleTranslationVec = new Vector2d(xvel , yvel);
        moduleTranslationVec.rotate(-1.0 * m_curModulePose.getRotation().getDegrees());
        return moduleTranslationVec;
    }

    /**
     * Given a net force on a particular module, calculate the friction force
     * generated by the tread interacting with the ground in the direction
     * perpendicular to the wheel's rotation.
     * @param netForce input force applied to the module
     */
    ForceAtPose2d getCrossTreadFricForce(Force2d netForce, double dtSeconds) {

        //Project net force onto cross-tread vector
        Vector2d crossTreadUnitVector = new Vector2d(0 , 1);
        crossTreadUnitVector.rotate(m_curAzmthAngle.getDegrees());
        m_crossTreadVelMag = getModRelTransVel(dtSeconds).dot(crossTreadUnitVector);
        m_crossTreadForceMag = netForce.getVector2d().dot(crossTreadUnitVector);

        Force2d fricForce = new Force2d();
     
        boolean useKinFric = Math.abs(m_crossTreadForceMag) > m_treadStaticFricForce
                            || Math.abs(m_crossTreadVelMag) > 0.001;

        if (useKinFric) {
            // Force is great enough to overcome static friction, or we're already moving
            // In either case, use kinetic frictional model
            m_crossTreadFricForceMag = -1.0 * Math.signum(m_crossTreadVelMag)
                                            * m_treadKineticFricForce;
        } else {
            // Static Friction Model
            m_crossTreadFricForceMag = -1.0 * m_crossTreadForceMag;
        }
     
        fricForce = new Force2d(crossTreadUnitVector);
        fricForce = fricForce.times(m_crossTreadFricForceMag);

        return new ForceAtPose2d(fricForce, m_curModulePose);
    }

     
    /** Gets the modules on-axis (along wheel direction) force,
     * which comes from the rotation of the motor. */
    ForceAtPose2d getWheelMotiveForce() {
        return new ForceAtPose2d(new Force2d(m_wheelMotor.getGroundForce_N(), m_curAzmthAngle),
                                 m_curModulePose);
    }

    /** Set the motion of each module in the field reference frame. */
    void setModulePose(Pose2d curPos) {
        //Handle init'ing module position history to current on first pass
        if (m_prevModulePose == null) {
            m_prevModulePose = curPos;
        } else {
            m_prevModulePose = m_curModulePose;
        }

        m_curModulePose = curPos;
    }

    Pose2d getPose() {
        return m_curModulePose;
    }

}
