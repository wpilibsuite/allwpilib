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

/// \brief The axis about which to measure rotation.
typedef enum { Roll /*X*/, Pitch /*Y*/, Yaw /*Z*/ } ROTATION;

/**
 * \brief Plugin for reading the speed and relative angle of a link.
 *
 * This plugin publishes the angle since last reset and the speed
 * which a link is rotating about some axis to subtopics of the given
 * topic every physics update. There is also a control topic that
 * takes one command: "reset", which sets the current angle as zero.
 *
 * To add a gyro to your robot, add the following XML to your robot
 * model:
 *
 *     <plugin name="my_gyro" filename="libgyro.so">
 *       <link>Joint Name</link>
 *       <topic>~/my/topic</topic>
 *       <units>{degrees, radians}</units>
 *     </plugin>
 *
 * - `link`: Name of the link this potentiometer is attached to.
 * - `topic`: Optional. Used as the root for subtopics. `topic`/position
 * (gazebo.msgs.Float64),
 *            `topic`/velocity (gazebo.msgs.Float64), `topic`/control
 * (gazebo.msgs.String)
 * - `units`; Optional, defaults to radians.
 */
class Gyro : public gazebo::ModelPlugin {
 public:
  /// \brief Load the gyro and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the gyro reading each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Publish the angle on this topic.
  std::string topic;

  /// \brief Whether or not this gyro measures radians or degrees.
  bool radians;

  /// \brief The axis to measure rotation about.
  ROTATION axis;

  /// \brief The zero value of the gyro.
  double zero;

  /// \brief The link that this gyro measures
  gazebo::physics::LinkPtr link;

  /// \brief Callback for handling control data
  void Callback(const gazebo::msgs::ConstStringPtr& msg);

  /// \brief Gets the current angle, taking into account whether to
  ///        return radians or degrees.
  double GetAngle();

  /// \brief Gets the current velocity, taking into account whether to
  ///        return radians/second or degrees/second.
  double GetVelocity();

  /// \brief Limit the value to either [-180,180] or [-PI,PI]
  ///       depending on whether or radians or degrees are being used.
  double Limit(double value);

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Subscriber handle.
  gazebo::transport::SubscriberPtr command_sub;

  /// \brief Publisher handles.
  gazebo::transport::PublisherPtr pos_pub, vel_pub;
};
