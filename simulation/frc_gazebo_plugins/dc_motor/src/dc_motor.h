/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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
 * \brief Plugin for controlling a joint with a DC motor.
 *
 * This plugin subscribes to a topic to get a signal in the range
 * [-1,1]. Every physics update the joint's torque is set as
 * multiplier*signal.
 *
 * To add a DC motor to your robot, add the following XML to your
 * robot model:
 *
 *     <plugin name="my_motor" filename="libdc_motor.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <multiplier>Number</multiplier>
 *     </plugin>
 *
 * - `joint`: Name of the joint this Dc motor is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 * - `multiplier`: Optional. Defaults to 1.
 */
class DCMotor : public gazebo::ModelPlugin {
 public:
  /// \brief Load the dc motor and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Update the torque on the joint from the dc motor each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Topic to read control signal from.
  std::string topic;

  /// \brief The pwm signal limited to the range [-1,1].
  double signal;

  /// \brief The magic torque multiplier. torque=multiplier*signal
  double multiplier;

  /// \brief The joint that this dc motor drives.
  gazebo::physics::JointPtr joint;

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
