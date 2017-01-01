/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/gazebo.hh>

#include "simulation/gz_msgs/msgs.h"

/**
 * \brief Plugin for controlling a servo.
 *
 * This plugin subscribes to a topic to get a signal in the range
 * [-1,1]. Every physics update the joint's torque is set as
 * multiplier*signal.
 *
 * To add a servo to your robot, add the following XML to your robot
 * model:
 *
 *     <plugin name="my_servo" filename="libservo.so">
 *       <joint>Joint Name</joint>
 *       <topic>/gzebo/frc/simulator/pwm/1</topic>
 *       <zero_position>0</zero_position>
 *     </plugin>
 *
 * - `link`: Name of the link the servo is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 */
class Servo : public gazebo::ModelPlugin {
 public:
  /// \brief load the servo and configure it according to the sdf
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Update the torque on the joint from the dc motor each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Topic to read control signal from.
  std::string topic;

  /// \brief the pwm signal limited to the range [-1,1]
  double signal;

  /// \brief the torque of the motor in kg/cm
  double torque;

  /// \brief the joint that this servo moves
  gazebo::physics::JointPtr joint;

  /// \brief Callback for receiving msgs and storing the signal
  void Callback(const gazebo::msgs::ConstFloat64Ptr& msg);

  /// \brief The model to which this is attached
  gazebo::physics::ModelPtr model;

  /// \brief The pointer to the world update function
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising torque
  gazebo::transport::NodePtr node;

  /// \brief The subscriber for the PWM signal
  gazebo::transport::SubscriberPtr sub;
};
