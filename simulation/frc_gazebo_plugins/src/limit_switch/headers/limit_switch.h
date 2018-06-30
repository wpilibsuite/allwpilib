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

#include "external_limit_switch.h"
#include "internal_limit_switch.h"
#include "simulation/gz_msgs/msgs.h"
#include "switch.h"

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
 *     <plugin name="my_limit_switch" filename="liblimit_switch.so">
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
 *     <plugin name="my_limit_switch" filename="liblimit_switch.so">
 *       <topic>~/my/topic</topic>
 *       <type>external</type>
 *       <sensor>Sensor Name</sensor>
 *     </plugin>
 *
 * Common:
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Bool.
 *            Recommended values are of the form:
 *              /gazebo/frc/simulator/dio/n
 * - `type`: Required. The type of limit switch that this is
 * - `invert`: Optional. If given, the output meaning will be inverted
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
class LimitSwitch : public gazebo::ModelPlugin {
 public:
  /// \brief Load the limit switch and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the limit switch reading each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Publish the limit switch value on this topic.
  std::string topic;

  /// \brief LimitSwitch object, currently internal or external.
  Switch* ls;

  /// \brief Indicate if we should invert the output
  bool invert;

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Publisher handle.
  gazebo::transport::PublisherPtr pub;
};
