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
 * \brief Plugin for publishing the simulation time.
 *
 * This plugin publishes the simualtaion time in seconds every physics
 * update.
 *
 * To add a clock to your robot, add the following XML to your robot
 * model:
 *
 *     <plugin name="my_clock" filename="libclock.so">
 *       <topic>~/my/topic</topic>
 *     </plugin>
 *
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Float64.
 *
 * \todo Make WorldPlugin?
 */
class Clock : public gazebo::ModelPlugin {
 public:
  /// \brief Load the clock and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out time each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Publish the time on this topic.
  std::string topic;

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Publisher handle.
  gazebo::transport::PublisherPtr pub;
};
