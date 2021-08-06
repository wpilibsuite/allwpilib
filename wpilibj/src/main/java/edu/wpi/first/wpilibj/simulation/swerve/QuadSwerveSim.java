package edu.wpi.first.wpilibj.simulation.swerve;


import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.wpilibj.drive.Vector2d;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class QuadSwerveSim {

    public static final int FL = 0; // Front Left Module Index
    public static final int FR = 1; // Front Right Module Index
    public static final int BL = 2; // Back Left Module Index
    public static final int BR = 3; // Back Right Module Index
    public static final int NUM_MODULES = 4;

    List<SwerveModuleSim> m_modules;

    Vector2d m_accelPrev = new Vector2d();
    Vector2d m_velPrev   = new Vector2d();
    double   m_rotAccelPrev = 0;
    double   m_rotVelPrev   = 0;

    public final List<Translation2d> m_robotToModuleTL;
    public final List<Transform2d> m_robotToModule;

    Pose2d m_curPose = new Pose2d();

    double m_robotMasskg;
    double m_robotMOI;

    /**
     * Simulates a drivetrain powered by four swerve modules.
     * @param wheelBaseWidthM left-to-right distance between the wheel/ground
     *      contact patches, in meters
     * @param wheelBaseLengthM front-to-back distance between the wheel/ground
     *      contact patches, in meters
     * @param robotMasskg Total mass of the robot, in kilograms.
     * @param robotMOI Effective moment of inertia of the robot, in kilogram-meters-squared
     * @param modules Set of swerve modules controlling the drivetrain.
     *      Order must be 0=FL, 1=FR, 2=BL, 3=BR
     */
    public QuadSwerveSim(
        double wheelBaseWidthM,
        double wheelBaseLengthM,
        double robotMasskg,
        double robotMOI,
        List<SwerveModuleSim> modules
    ) {
        this.m_modules = modules;

        m_robotToModuleTL = Arrays.asList(
            new Translation2d( wheelBaseWidthM / 2,  wheelBaseLengthM / 2),
            new Translation2d( wheelBaseWidthM / 2, -wheelBaseLengthM / 2),
            new Translation2d(-wheelBaseWidthM / 2,  wheelBaseLengthM / 2),
            new Translation2d(-wheelBaseWidthM / 2, -wheelBaseLengthM / 2)
        );

        m_robotToModule = Arrays.asList(
            new Transform2d(m_robotToModuleTL.get(FL), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(FR), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(BL), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(BR), new Rotation2d(0.0))
        );

        this.m_robotMasskg = robotMasskg;
        this.m_robotMOI = robotMOI;
     
    }

    /**
     * Resets the robot to a non-moving state somewhere on the field.
     * @param pose Position on the field to reset the robot to.
     */
    public void modelReset(Pose2d pose) {
        m_accelPrev = new Vector2d();
        m_velPrev   = new Vector2d();
        m_rotAccelPrev = 0;
        m_rotVelPrev   = 0;
        for (int idx = 0; idx < NUM_MODULES; idx++) {
            m_modules.get(idx).reset(pose.transformBy(m_robotToModule.get(idx)));
        }
        m_curPose = pose;
    }

    /**
     * Steps the module simulation forward by one discrete step.
     * @param dtSeconds size of the discrete step to take
     */
    public void update(double dtSeconds) {

        Pose2d fieldRF = new Pose2d(); // global origin
        Transform2d fieldToRobot = new Transform2d(fieldRF, m_curPose);

        ////////////////////////////////////////////////////////////////
        // Component-Force Calculations to populate the free-body diagram

        // Calculate each module's new position, and step it through simulation.
        for (int idx = 0; idx < NUM_MODULES; idx++) {
            Pose2d tmp = fieldRF.transformBy(fieldToRobot);
            Pose2d modPose = tmp.transformBy(m_robotToModule.get(idx));
            m_modules.get(idx).setModulePose(modPose);
            m_modules.get(idx).update(dtSeconds);
        }

        // Force on frame from wheel motive forces (along-tread)
        ArrayList<ForceAtPose2d> wheelMotiveForces = new ArrayList<ForceAtPose2d>(NUM_MODULES);
        for (int idx = 0; idx < NUM_MODULES; idx++) {
            wheelMotiveForces.add(m_modules.get(idx).getWheelMotiveForce());
        }

        // Friction Model
        Force2d preFricNetForce = new Force2d();
        wheelMotiveForces.forEach((ForceAtPose2d mf) -> {
            //Add up all the forces that friction gets a chance to fight against
            preFricNetForce.accum(mf.getForceInRefFrame(m_curPose));
        });

        Force2d sidekickForce = new Force2d(0, 0); //TODO - make a generic "external force" input?

        preFricNetForce.accum(sidekickForce);

        //calculate force on the robot, "pre friction" (pf)
        ForceAtPose2d pfRobotForce = new ForceAtPose2d(preFricNetForce, m_curPose);

        // Calculate the forces from cross-tread friction at each module
        ArrayList<ForceAtPose2d> xtreadFricFrc = new ArrayList<ForceAtPose2d>(NUM_MODULES);
        for (int idx = 0; idx < NUM_MODULES; idx++) {
            SwerveModuleSim mod = m_modules.get(idx);
            //Assume force evenly applied to all modules.
            double ffrac = 1.0 / NUM_MODULES;
            Force2d pfModForce = pfRobotForce.getForceInRefFrame(mod.getPose()).times(ffrac);
            xtreadFricFrc.add(mod.getCrossTreadFricForce(pfModForce, dtSeconds));
        }

        ////////////////////////////////////////////////////////////////
        // Combine forces in free-body diagram

        // Using all the above force components, do Sum of Forces
        Force2d forceOnRobotCenter = preFricNetForce;

        xtreadFricFrc.forEach((ForceAtPose2d f) -> {
            forceOnRobotCenter.accum(f.getForceInRefFrame(m_curPose));
        });
     
        ForceAtPose2d netForce = new ForceAtPose2d(forceOnRobotCenter, m_curPose);

        Force2d robotForceInFieldRefFrame = netForce.getForceInRefFrame(fieldRF);

        //Sum of Torques
        double netTorque = 0;

        for (int idx = 0; idx < NUM_MODULES; idx++) {
            netTorque += wheelMotiveForces.get(idx).getTorque(m_curPose);
            netTorque += xtreadFricFrc.get(idx).getTorque(m_curPose);
        }


        ////////////////////////////////////////////////////////////////
        // Apply Newton's 2nd law to get motion from forces

        //a = F/m in field frame
        Vector2d accel = robotForceInFieldRefFrame.times(1 / m_robotMasskg).getVector2d();

        //Trapezoidal integration
        Vector2d velocity = new Vector2d( m_velPrev.x + (accel.x + m_accelPrev.x) / 2 * dtSeconds,
                                          m_velPrev.y + (accel.y + m_accelPrev.y) / 2 * dtSeconds);

        //Trapezoidal integration
        Translation2d posChange = new Translation2d( (velocity.x + m_velPrev.x) / 2 * dtSeconds,
                                                     (velocity.y + m_velPrev.y) / 2 * dtSeconds);

        //Twist needs to be relative to robot reference frame
        posChange = posChange.rotateBy(m_curPose.getRotation().unaryMinus());

        m_velPrev = velocity;
        m_accelPrev = accel;
     
        //alpha = T/I in field frame
        double rotAccel = netTorque / m_robotMOI;
        double rotVel = m_rotVelPrev + (rotAccel + m_rotAccelPrev) / 2 * dtSeconds;
        double rotPosChange = (rotVel + m_rotVelPrev) / 2 * dtSeconds;

        m_rotVelPrev = rotVel;
        m_rotAccelPrev = rotAccel;

        Twist2d motionThisLoop = new Twist2d(posChange.getX(), posChange.getY(), rotPosChange);
     
        m_curPose = m_curPose.exp(motionThisLoop);
    }

    /**
     * Returns the current pose of the drivetrain.
     */
    public Pose2d getCurPose() {
        return m_curPose;
    }

}
