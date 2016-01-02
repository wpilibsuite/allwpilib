/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/gz_msgs/msgs.h"

#include "switch.h"

#include "internal_limit_switch.h"
#include "external_limit_switch.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo.hh>

using namespace gazebo;

/**
 * \brief Plugin for reading limit switches.
 *
 * This plugin publishes whether or not the limit switch has been
 * triggered every physics update. There are two types of limit switches:
 *
 * - Internal: Measure joint limits. Triggerd if the joint is within
 *             some range.
 * - External: Measure interactions with the outside world. Triggerd
 *             if some link is in collision.
 *
 * To add a limit swithch to your robot, add the following XML to your
 * robot model.
 *
 * Internal:
 *
 *     <plugin name="my_limit_switch" filename="libgz_limit_switch.so">
 *       <topic>~/my/topic</topic>
 *       <type>internal</type>
 *       <joint>Joint Name</joint>
 *       <units>{degrees, radians}</units>
 *       <min>Number</min>
 *       <max>Number</max>
 *     </plugin>
 *
 * External:
 *
 *     <plugin name="my_limit_switch" filename="libgz_limit_switch.so">
 *       <topic>~/my/topic</topic>
 *       <type>external</type>
 *       <sensor>Sensor Name</sensor>
 *     </plugin>
 *
 * Common:
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Float64.
 * - `type`: Required. The type of limit switch that this is
 *
 * Internal
 * - `joint`: Name of the joint this potentiometer is attached to.
 * - `units`: Optional. Defaults to radians.
 * - `min`: Minimum angle considered triggered.
 * - `max`: Maximum angle considered triggered.
 *
 * External
 * - `sensor`: Name of the contact sensor that this limit switch uses.
 */
class LimitSwitch: public ModelPlugin {
public:
  LimitSwitch();
  ~LimitSwitch();

  /// \brief Load the limit switch and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the limit switch reading each timestep.
  void Update(const common::UpdateInfo &info);

private:
  /// \brief Publish the limit switch value on this topic.
  std::string topic;

  /// \brief LimitSwitch object, currently internal or external.
  Switch* ls;


  physics::ModelPtr model;         ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn; ///< \brief Pointer to the world update function.
  transport::NodePtr node;         ///< \brief The node we're advertising on.
  transport::PublisherPtr pub;     ///< \brief Publisher handle.
};
