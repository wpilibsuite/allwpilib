/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/gazebo.hh>

#include "simulation/gz_msgs/msgs.h"

/**
 * \brief Plugin for controlling a joint with a pneumatic piston.
 *
 * This plugin subscribes to topics to get the solenoid state for a piston
 *    It needs one signal for the forward signal.  For a double solenoid,
 *    a second signal can be sent.
 *    Each signal is a boolean.
 *
 * Every physics update the joint's torque is set to reflect the
 * signal, using the configured force
 *
 * To add a pneumatic piston to your robot, add the following XML to
 * your robot model:
 *
 *     <plugin name="my_piston" filename="libpneumatic_piston.so">
 *       <joint>Joint Name</joint>
 *       <topic>/gazebo/frc/simulator/pneumatics/1/1</topic>
 *       <reverse-topic>/gazebo/frc/simulator/pneumatics/1/2</reverse-topic>
 *       <direction>{forward, reversed}</direction>
 *       <forward-force>Number</forward-force>
 *       <reverse-force>Number</reverse-force>
 *     </plugin>
 *
 * - `joint`: Name of the joint this piston is attached to.
 * - `topic`: Optional. Forward Solenoid signal name. type gazebo.msgs.Bool.
 *            If not given, the name given for the plugin will be used.
 *            A pattern of /gazebo/frc/simulator/pneumatics/1/n is good.
 *            The first number represents the PCM module.  Only 1 is supported.
 *            The second number represents the channel on the PCM.
 * - `topic-reverse`: Optional. If given, represents the reverse channel.
 *            Message type should be gazebo.msgs.Bool.
 * - `direction`: Optional. Defaults to forward. Reversed if the piston
 *                pushes in the opposite direction of the joint axis.
 * - `forward-force`: Force to apply in the forward direction.
 * - `reverse-force`: Force to apply in the reverse direction.
 *                    For a single solenoid, you would expect '0',
 *                    but we allow model builders to provide a value.
 *
 */
class PneumaticPiston : public gazebo::ModelPlugin {
 public:
  /// \brief Load the pneumatic piston and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Updat the force the piston applies on the joint.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Topic to read forward control signal from.
  std::string topic;

  /// \brief Topic to read reverse control signal from.
  std::string reverse_topic;

  /// \brief Whether the solenoid to open forward is on
  bool forward_signal;

  /// \brief Whether the solenoid to open in reverse is on
  bool reverse_signal;

  /// \brief The magic force multipliers for each direction.
  double forward_force, reverse_force;

  /// \brief The joint that this pneumatic piston actuates.
  gazebo::physics::JointPtr joint;

  /// \brief Callback for receiving msgs and updating the solenoid state
  void ForwardCallback(const gazebo::msgs::ConstBoolPtr& msg);

  /// \brief Callback for receiving msgs and updating the reverse solenoid state
  void ReverseCallback(const gazebo::msgs::ConstBoolPtr& msg);

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Subscriber handle.
  gazebo::transport::SubscriberPtr sub;

  /// \brief Subscriber handle for reverse topic
  gazebo::transport::SubscriberPtr sub_reverse;
};
