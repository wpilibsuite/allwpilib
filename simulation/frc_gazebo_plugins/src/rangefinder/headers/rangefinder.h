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
 * \brief Plugin for reading the range of obstacles.
 *
 * This plugin publishes the range of obstacles detected by a sonar
 * rangefinder every physics update.
 *
 * To add a rangefinder to your robot, add the following XML to your
 * robot model:
 *
 *     <plugin name="my_rangefinder" filename="librangefinder.so">
 *       <sensor>Sensor Name</sensor>
 *       <topic>~/my/topic</topic>
 *     </plugin>
 *
 * - `sensor`: Name of the sonar sensor that this rangefinder uses.
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Float64.
 */
class Rangefinder : public gazebo::ModelPlugin {
 public:
  /// \brief Load the rangefinder and configures it according to the sdf.
  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the rangefinder reading each timestep.
  void Update(const gazebo::common::UpdateInfo& info);

 private:
  /// \brief Publish the range on this topic.
  std::string topic;

  /// \brief The sonar sensor that this rangefinder uses
  gazebo::sensors::SonarSensorPtr sensor;

  /// \brief The model to which this is attached.
  gazebo::physics::ModelPtr model;

  /// \brief Pointer to the world update function.
  gazebo::event::ConnectionPtr updateConn;

  /// \brief The node on which we're advertising.
  gazebo::transport::NodePtr node;

  /// \brief Publisher handle.
  gazebo::transport::PublisherPtr pub;
};
