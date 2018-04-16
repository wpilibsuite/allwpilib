/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

#include "simulation/gz_msgs/msgs.h"

/**
 * \brief Plugin for simulating a drive motor
 *
 * This plugin attempts to overcome a limitation in gazebo.
 * That is, most normal FRC robots rely on wheels that have good
 * traction in one direction, and less traction in the opposite
 * direction.
 *
 * Gazebo does not model that well (in fact, drive wheels are
 * quite hard to simulate).
 *
 * So this plugin subscribes to a PWM output signal and applies
 * a force to the chassis at the proscribed point in hopefully
 * the correct direction.  The SDF model can then have lower friction,
 * and it should turn more naturally.
 *
 * This plugin also attempts to simulate the limitations of a drive
 * motor, most notably the maximum speed any given motor can spin at.
 * The initial implemention is quite naive; just a linear reduction
 * in force as a product of velocity/max velocity.
 *
 * Nicely, this plugin let's you generate a force in any of
 * three axes.  That is helpful for simulating a mecanum drive.
 *
 * This plugin subscribes to a topic to get a signal in the range
 * [-1,1]. Every physics update the joint's torque is set as
 * multiplier*signal*direction.
 *
 * To add a drive motor to your robot, add the following XML to your
 * robot model:
 *
 *     <plugin name="my_motor" filename="libdrive_motor.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <multiplier>Number</multiplier>
 *       <max_speed>Number</max_speed>
 *       <dx>-1, 0, or 1</dx>
 *       <dy>-1, 0, or 1</dy>
 *       <dz>-1, 0, or 1</dz>
 *     </plugin>
 *
 * - `joint`: Name of the joint this Dc motor is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 *            A typical topic looks like this:
 *              /gazebo/frc/simulator/pwm/<n>
 * - `multiplier`: Optional. Defaults to 1.  Force applied by this motor.
 *            This is force in Newtons.
 * - `max_speed`:  Optional. Defaults to no maximum.
 *            This is, in theory, meters/second.  Note that friction
 *            and other forces will also slow down a robot.
 *            In practice, this term can be tuned until the robot feels right.
 * - `dx`:  These three constants must be set to either -1, 0, or 1
 * - `dy`:  This controls whether or not the motor produces force
 * - `dz`:  along a given axis, and what direction.  Each defaults to 0.
 *          These are relative to the frame of the parent link of the joint.
 *          So they are usually relative to a chassis.
 *          The force is applied at the point that the joint connects to
 *          the parent link.
 */
class DriveMotor : public gazebo::ModelPlugin {
 public:
  /// \brief Load the dc motor and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Update the force on the parent of the joint from each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Topic to read control signal from.
  std::string topic;

  /// \brief The pwm signal limited to the range [-1,1].
  double signal;

  /// \brief The robot's maximum speed
  double maxSpeed;

  /// \brief The magic drive force multipliers. force=multiplier*signal
  double multiplier;

  /// \brief The directional constants limited to -1, 0, or 1.
  double dx;
  double dy;
  double dz;

  /// \brief The joint that this motor drives.
  gazebo::physics::JointPtr joint;

  /// \brief The parent of this joint; usually a chassis
  gazebo::physics::LinkPtr parent;

  /// \brief The child of this joint; usually a wheel
  gazebo::physics::LinkPtr child;

  /// \brief Callback for receiving msgs and storing the signal.
  void Callback(const gazebo::msgs::ConstFloat64Ptr& msg);

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer toe the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Subscriber handle.
  gazebo::transport::SubscriberPtr sub;
};
