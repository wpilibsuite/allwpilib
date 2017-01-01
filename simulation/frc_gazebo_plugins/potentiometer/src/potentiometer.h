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
 * \brief Plugin for reading the angle of a joint.
 *
 * This plugin publishes the angle of a joint to the topic every
 * physics update. Supports reading in either radians or degrees.
 *
 * To add a potentiometer to your robot, add the following XML to your
 * robot model:
 *
 *     <plugin name="my_pot" filename="libpotentiometer.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <units>{degrees, radians}</units>
 *     </plugin>
 *
 * - `joint`: Name of the joint this potentiometer is attached to.
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Float64.
 * - `units`: Optional. Defaults to radians.
 */
class Potentiometer : public gazebo::ModelPlugin {
 public:
  /// \brief Load the potentiometer and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the potentiometer reading each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Publish the angle on this topic.
  std::string topic;

  /// \brief Whether or not this potentiometer measures radians or degrees.
  bool radians;

  /// \brief The joint that this potentiometer measures
  gazebo::physics::JointPtr joint;

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Publisher handle.
  gazebo::transport::PublisherPtr pub;
};
