/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/gz_msgs/msgs.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo.hh>




using namespace gazebo;

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
 *     <plugin name="my_motor" filename="libgz_dc_motor.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <multiplier>Number</multiplier>
 *     </plugin>
 *
 * - `joint`: Name of the joint this Dc motor is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 * - `multiplier`: Optional. Defaults to 1.
 */
class DCMotor: public ModelPlugin {
public:
  DCMotor();
  ~DCMotor();

  /// \brief Load the dc motor and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Update the torque on the joint from the dc motor each timestep.
  void Update(const common::UpdateInfo &info);

private:
  /// \brief Topic to read control signal from.
  std::string topic;

  /// \brief The pwm signal limited to the range [-1,1].
  double signal;

  /// \brief The magic torque multiplier. torque=multiplier*signal
  double multiplier;

  /// \brief The joint that this dc motor drives.
  physics::JointPtr joint;

  /// \brief Callback for receiving msgs and storing the signal.
  void Callback(const msgs::ConstFloat64Ptr &msg);


  physics::ModelPtr model;         ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn; ///< \brief Pointer to the world update function.
  transport::NodePtr node;         ///< \brief The node we're advertising on.
  transport::SubscriberPtr sub;    ///< \brief Subscriber handle.
};
