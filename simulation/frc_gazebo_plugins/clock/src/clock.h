#pragma once


#include "simulation/gz_msgs/msgs.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo.hh>


using namespace gazebo;

/**
 * \brief Plugin for publishing the simulation time.
 *
 * This plugin publishes the simualtaion time in seconds every physics
 * update.
 *
 * To add a clock to your robot, add the following XML to your robot
 * model:
 *
 *     <plugin name="my_clock" filename="libgz_clock.so">
 *       <topic>~/my/topic</topic>
 *     </plugin>
 *
 * - `topic`: Optional. Message will be published as a gazebo.msgs.Float64.
 *
 * \todo Make WorldPlugin?
 */
class Clock: public ModelPlugin {
public:
  Clock();
  ~Clock();

  /// \brief Load the clock and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out time each timestep.
  void Update(const common::UpdateInfo &info);

private:
  std::string topic;               ///< \brief Publish the time on this topic.
  physics::ModelPtr model;         ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn; ///< \brief Pointer to the world update function.
  transport::NodePtr node;         ///< \brief The node we're advertising on.
  transport::PublisherPtr pub;     ///< \brief Publisher handle.
};

