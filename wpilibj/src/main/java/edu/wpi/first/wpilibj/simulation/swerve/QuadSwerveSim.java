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

    static public final int FL = 0; // Front Left Module Index
    static public final int FR = 1; // Front Right Module Index
    static public final int BL = 2; // Back Left Module Index
    static public final int BR = 3; // Back Right Module Index
    static public final int NUM_MODULES = 4;

    List<SwerveModuleSim> m_modules;

    Vector2d m_accel_prev = new Vector2d();
    Vector2d m_vel_prev   = new Vector2d();
    double   m_rotAccel_prev = 0;
    double   m_rotVel_prev   = 0;

    public final List<Translation2d> m_robotToModuleTL;
    public final List<Transform2d> m_robotToModuleTF;

    Pose2d m_curPose = new Pose2d();

    double m_robotMass_kg;
    double m_robotMOI;

    public QuadSwerveSim(
        double wheelBaseWidth_m,
        double wheelBaseLength_m,
        double robotMass_kg,
        double robotMOI,
        List<SwerveModuleSim> modules
    ) {
        this.m_modules = modules;

        m_robotToModuleTL = Arrays.asList(
            new Translation2d( wheelBaseWidth_m / 2,  wheelBaseLength_m / 2),
            new Translation2d( wheelBaseWidth_m / 2, -wheelBaseLength_m / 2),
            new Translation2d(-wheelBaseWidth_m / 2,  wheelBaseLength_m / 2),
            new Translation2d(-wheelBaseWidth_m / 2, -wheelBaseLength_m / 2)
        );

        m_robotToModuleTF = Arrays.asList(
            new Transform2d(m_robotToModuleTL.get(FL), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(FR), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(BL), new Rotation2d(0.0)),
            new Transform2d(m_robotToModuleTL.get(BR), new Rotation2d(0.0))
        );  

        this.m_robotMass_kg = robotMass_kg;
        this.m_robotMOI = robotMOI;
       
    }

    public void modelReset(Pose2d pose) {
        m_accel_prev = new Vector2d();
        m_vel_prev   = new Vector2d();
        m_rotAccel_prev = 0;
        m_rotVel_prev   = 0;
        for(int idx = 0; idx < NUM_MODULES; idx++) {
            m_modules.get(idx).reset(pose.transformBy(m_robotToModuleTF.get(idx)));
        }
        m_curPose = pose;
    }

    public void update(double dtSeconds) {

        Pose2d fieldReferenceFrame = new Pose2d(); // global origin
        Transform2d fieldToRobotTrans = new Transform2d(fieldReferenceFrame, m_curPose);

        ////////////////////////////////////////////////////////////////
        // Component-Force Calculations to populate the free-body diagram

        // Calculate each module's new position, and step it through simulation.
        for(int idx = 0; idx < NUM_MODULES; idx++) {
            Pose2d modPose = fieldReferenceFrame.transformBy(fieldToRobotTrans).transformBy(m_robotToModuleTF.get(idx));
            m_modules.get(idx).setModulePose(modPose);
            m_modules.get(idx).update(dtSeconds);
        }

        // Force on frame from wheel motive forces (along-tread)
        ArrayList<ForceAtPose2d> wheelMotiveForces = new ArrayList<ForceAtPose2d>(NUM_MODULES);
        for(int idx = 0; idx < NUM_MODULES; idx++) {
            wheelMotiveForces.add(m_modules.get(idx).getWheelMotiveForce());
        }

        // First half of the somewhat-dubious friction model
        Force2d preFricNetForce = new Force2d();
        wheelMotiveForces.forEach((ForceAtPose2d mf) -> {
            preFricNetForce.accum(mf.getForceInRefFrame(m_curPose)); //Add up all the forces that friction gets a chance to fight against
        });

        Force2d sidekickForce = new Force2d(0, 0); //TODO - make a generic "external force" input?

        preFricNetForce.accum(sidekickForce);

        ForceAtPose2d preFricNetForceRobotCenter = new ForceAtPose2d(preFricNetForce, m_curPose);

        // Calculate the forces from cross-tread friction at each module
        ArrayList<ForceAtPose2d> netXtreadFricForces = new ArrayList<ForceAtPose2d>(NUM_MODULES);
        for(int idx = 0; idx < NUM_MODULES; idx++) {
            SwerveModuleSim mod = m_modules.get(idx);
            double perWheelForceFrac = 1.0/NUM_MODULES; //Assume force evenly applied to all modules.
            Force2d preFricForceAtModule = preFricNetForceRobotCenter.getForceInRefFrame(mod.getModulePose()).times(perWheelForceFrac);
            netXtreadFricForces.add(mod.getCrossTreadFrictionalForce(preFricForceAtModule, dtSeconds));
        }

        ////////////////////////////////////////////////////////////////
        // Combine forces in free-body diagram

        // Using all the above force components, do Sum of Forces
        Force2d forceOnRobotCenter = preFricNetForce;

        netXtreadFricForces.forEach((ForceAtPose2d f) -> {
            forceOnRobotCenter.accum(f.getForceInRefFrame(m_curPose));
        });
       
        ForceAtPose2d netForce = new ForceAtPose2d(forceOnRobotCenter, m_curPose);

        Force2d robotForceInFieldRefFrame = netForce.getForceInRefFrame(fieldReferenceFrame);

        //Sum of Torques
        double netTorque = 0;

        for(int idx = 0; idx < NUM_MODULES; idx++) {
            netTorque += wheelMotiveForces.get(idx).getTorque(m_curPose);
            netTorque += netXtreadFricForces.get(idx).getTorque(m_curPose);
        }


        ////////////////////////////////////////////////////////////////
        // Apply Newton's 2nd law to get motion from forces

        //a = F/m in field frame
        Vector2d accel = robotForceInFieldRefFrame.times(1/m_robotMass_kg).getVector2d();

        Vector2d velocity = new Vector2d( m_vel_prev.x + (accel.x + m_accel_prev.x) / 2 * dtSeconds, //Trapezoidal integration
                                          m_vel_prev.y + (accel.y + m_accel_prev.y) / 2 * dtSeconds);

        Translation2d posChange = new Translation2d( (velocity.x + m_vel_prev.x) / 2 * dtSeconds, //Trapezoidal integration
                                                     (velocity.y + m_vel_prev.y) / 2 * dtSeconds);
       
        m_vel_prev = velocity;
        m_accel_prev = accel;
       
        //alpha = T/I in field frame
        double rotAccel = netTorque / m_robotMOI;
        double rotVel = m_rotVel_prev + (rotAccel + m_rotAccel_prev) / 2 * dtSeconds;
        double rotPosChange = (rotVel + m_rotVel_prev) / 2 * dtSeconds;

        m_rotVel_prev = rotVel;
        m_rotAccel_prev = rotAccel;

        posChange = posChange.rotateBy(m_curPose.getRotation().unaryMinus()); //Twist needs to be relative to robot reference frame

        Twist2d motionThisLoop = new Twist2d(posChange.getX(), posChange.getY(), rotPosChange);
       
        m_curPose = m_curPose.exp(motionThisLoop);
    }

    public Pose2d getCurPose() {
        return m_curPose;
    }

}
