/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/gz_msgs/msgs.h"

#include <gazebo/gazebo.hh>

using namespace gazebo;

/**
 * \brief Plugin for controlling a joint with a pneumatic piston.
 *
 * This plugin subscribes to a topic to get a signal. It accepts three
 * values:
 *
 * -  1: Apply the forward force to the joint.
 * -  0: Maintain last applied force
 * - -1: Apply the reverse force to the joint.
 *
 * Every physics update the joint's torque is set to reflect the
 * signal.
 *
 * To add a pneumatic piston to your robot, add the following XML to
 * your robot model:
 *
 *     <plugin name="my_piston" filename="libgz_pneumatic_piston.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <direction>{forward, reversed}</direction>
 *       <forward-force>Number</forward-force>
 *       <reverse-force>Number</reverse-force>
 *     </plugin>
 *
 * - `joint`: Name of the joint this Dc motor is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 * - `direction`: Optional. Defaults to forward. Reversed if the
 *                piston pushes in the opposite direction of the joint
 *                axis.
 * - `forward-force`: Force to apply in the forward direction.
 * - `reverse-force`: Force to apply in the reverse direction.
 *
 * \todo Signal should probably be made a tri-state message.
 */
class PneumaticPiston: public ModelPlugin {
public:
  PneumaticPiston();
  ~PneumaticPiston();

  /// \brief Load the pneumatic piston and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Updat the force the piston applies on the joint.
  void Update(const common::UpdateInfo &info);

private:
  /// \brief Topic to read control signal from.
  std::string topic;

  /// \brief The signal is one of: {-1,0,1}.
  double signal;

  /// \brief The magic force multipliers for each direction.
  double forward_force, reverse_force;

  /// \brief The joint that this pneumatic piston actuates.
  physics::JointPtr joint;

  /// \brief Callback for receiving msgs and updating the torque.
  void Callback(const msgs::ConstFloat64Ptr &msg);


  physics::ModelPtr model;         ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn; ///< \brief Pointer to the world update function.
  transport::NodePtr node;         ///< \brief The node we're advertising on.
  transport::SubscriberPtr sub;    ///< \brief Subscriber handle.
};
