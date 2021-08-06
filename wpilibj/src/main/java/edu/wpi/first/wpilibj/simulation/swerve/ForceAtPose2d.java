package edu.wpi.first.wpilibj.simulation.swerve;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.wpilibj.drive.Vector2d;
import java.util.Objects;

public class ForceAtPose2d {
  public Force2d m_force;
  public Pose2d m_pos;

  /**
   * Constructs a ForceAtDistance2d that's all zeroed out.
   */
  public ForceAtPose2d() {
    this(new Force2d(), new Pose2d());
  }

  /**
   * Constructs from a given force and position that force acts at.
   * @param force The force
   * @param pos The position the force is acting at
   */
  public ForceAtPose2d( Force2d force, Pose2d pos) {
    m_force = force;
    m_pos = pos;
  }

  /**
   * Returns the torque associated with this force at distance.
   * positive is counter-clockwise, negative is clockwise
   */
  public double getTorque(Pose2d centerOfRotation) {
    Transform2d transCORtoF = new Transform2d(centerOfRotation, m_pos);

    //Align the force to the reference frame of the center of rotation
    Force2d alignedForce = getForceInRefFrame(centerOfRotation);

    //Calculate the lever arm the force acts at
    Vector2d leverArm = new Vector2d(transCORtoF.getX(), transCORtoF.getY());
    return leverArm.cross(alignedForce.getVector2d());
  }

  public Force2d getForceInRefFrame(Pose2d refFrame) {
    Transform2d trans = new Transform2d(refFrame, m_pos);
    return m_force.rotateBy(trans.getRotation());
  }

  @Override
  public String toString() {
    return String.format("ForceAtDistance2D(X: %.2fN, Y: %.2fN)", m_force.getX(), m_force.getY());
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
      return Math.abs(((ForceAtPose2d) obj).m_force.getX() - m_force.getX()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).m_force.getY() - m_force.getY()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).m_pos.getX() - m_pos.getX()) < 1E-9
          && Math.abs(((ForceAtPose2d) obj).m_pos.getY() - m_pos.getY()) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_force.getX(), m_force.getY(), m_pos.getX(), m_pos.getY());
  }
}
