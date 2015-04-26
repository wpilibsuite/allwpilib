#pragma once

#include "simulation/gz_msgs/msgs.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo.hh>


using namespace gazebo;

/**
 * \brief Plugin for reading the speed and relative angle of a joint.
 *
 * This plugin publishes the angle since last reset and the speed of a
 * given joint to subtopics of the given topic every physics
 * update. There is also a control subtopic that takes three commands:
 * "start", "stop" and "reset":
 *
 * - "start": Start counting ticks from the current count.
 * - "stop":  Stop counting ticks, pauses updates.
 * - "reset": Set the current angle to zero.
 *
 * To add a encoder to your robot, add the following XML to your
 * robot model:
 *
 *     <plugin name="my_encoder" filename="libgz_encoder.so">
 *       <joint>Joint Name</joint>
 *       <topic>~/my/topic</topic>
 *       <units>{degrees, radians}</units>
 *     </plugin>
 *
 * - `joint`: Name of the joint this encoder is attached to.
 * - `topic`: Optional. Used as the root for subtopics. `topic`/position (gazebo.msgs.Float64),
 *            `topic`/velocity (gazebo.msgs.Float64), `topic`/control (gazebo.msgs.String)
 * - `units`: Optional. Defaults to radians.
 */
class Encoder: public ModelPlugin {
public:
  Encoder();
  ~Encoder();

  /// \brief Load the encoder and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the encoder reading each timestep.
  void Update(const common::UpdateInfo &info);

private:
  /// \brief Root topic for subtopics on this topic.
  std::string topic;

  /// \brief Whether or not this encoder measures radians or degrees.
  bool radians;

  /// \brief Whether or not the encoder has been stopped.
  bool stopped;

  /// \brief The zero value of the encoder.
  double zero;

  /// \brief The value the encoder stopped counting at
  double stop_value;

  /// \brief The joint that this encoder measures
  physics::JointPtr joint;

  /// \brief Callback for handling control data
  void Callback(const msgs::ConstStringPtr &msg);

  /// \brief Gets the current angle, taking into account whether to
  ///        return radians or degrees.
  double GetAngle();

  /// \brief Gets the current velocity, taking into account whether to
  ///        return radians/second or degrees/second.
  double GetVelocity();

  physics::ModelPtr model;                  ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn;          ///< \brief Pointer to the world update function.
  transport::NodePtr node;                  ///< \brief The node we're advertising on.
  transport::SubscriberPtr command_sub;     ///< \brief Subscriber handle.
  transport::PublisherPtr pos_pub, vel_pub; ///< \brief Publisher handles.
};
