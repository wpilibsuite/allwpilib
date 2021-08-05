package edu.wpi.first.wpilibj.simulation.swerve;

import java.util.Objects;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.wpilibj.drive.Vector2d;


public class ForceAtPose2d {
  public Force2d force;
  public Pose2d pos;

  /**
   * Constructs a ForceAtDistance2d that's all zeroed out
   */
  public ForceAtPose2d () {
    this(new Force2d(), new Pose2d());
  }

  /**
   * Constructs a Force2d with the X and Y components equal to the
   * provided values.
   *
   * @param x The x component of the force.
   * @param y The y component of the force.
   */
  public ForceAtPose2d ( Force2d force_in, Pose2d pos_in) {
    force = force_in;
    pos = pos_in;
  }

  /** 
   * Returns the torque associated with this force at distance
   * positive is counter-clockwise, negative is clockwise
   */
  public double getTorque(Pose2d centerOfRotation){
    Transform2d transCORtoF = new Transform2d(centerOfRotation, pos);

    //Align the force to the reference frame of the center of rotation
    Force2d alignedForce = getForceInRefFrame(centerOfRotation); 

    //Calculate the lever arm the force acts at
    Vector2d leverArm = new Vector2d(transCORtoF.getX(), transCORtoF.getY());
    return leverArm.cross(alignedForce.getVector2d());
  }

  public Force2d getForceInRefFrame(Pose2d refFrame){
    Transform2d trans = new Transform2d(refFrame, pos);
    return force.rotateBy(trans.getRotation());
  }

  @Override
  public String toString() {
    return String.format("ForceAtDistance2D(X: %.2fN, Y: %.2fN)", force.getX(), force.getY());
  }

  /**
   * Checks equality between this Force2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof ForceAtPose2d) {
      return Math.abs(((ForceAtPose2d) obj).force.getX() - force.getX()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).force.getY() - force.getY()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).pos.getX() - pos.getX()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).pos.getY() - pos.getY()) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(force.getX(), force.getY(), pos.getX(), pos.getY());
  }
}
