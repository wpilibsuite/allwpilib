#pragma once

#include "simulation/gz_msgs/msgs.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo.hh>



using namespace gazebo;

/// \brief The axis about which to measure rotation.
typedef enum {Roll /*X*/, Pitch /*Y*/, Yaw /*Z*/} ROTATION;

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
 *     <plugin name="my_gyro" filename="libgz_gyro.so">
 *       <link>Joint Name</link>
 *       <topic>~/my/topic</topic>
 *       <units>{degrees, radians}</units>
 *     </plugin>
 *
 * - `link`: Name of the link this potentiometer is attached to.
 * - `topic`: Optional. Used as the root for subtopics. `topic`/position (gazebo.msgs.Float64),
 *            `topic`/velocity (gazebo.msgs.Float64), `topic`/control (gazebo.msgs.String)
 * - `units`; Optional, defaults to radians.
 */
class Gyro: public ModelPlugin {
public:
  Gyro();
  ~Gyro();

  /// \brief Load the gyro and configures it according to the sdf.
  void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

  /// \brief Sends out the gyro reading each timestep.
  void Update(const common::UpdateInfo &info);

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
  physics::LinkPtr link;

  /// \brief Callback for handling control data
  void Callback(const msgs::ConstStringPtr &msg);

  /// \brief Gets the current angle, taking into account whether to
  ///        return radians or degrees.
  double GetAngle();

  /// \brief Gets the current velocity, taking into account whether to
  ///        return radians/second or degrees/second.
  double GetVelocity();

  /// \brief Limit the value to either [-180,180] or [-PI,PI]
  ///       depending on whether or radians or degrees are being used.
  double Limit(double value);

  physics::ModelPtr model;                  ///< \brief The model that this is attached to.
  event::ConnectionPtr updateConn;          ///< \brief Pointer to the world update function.
  transport::NodePtr node;                  ///< \brief The node we're advertising on.
  transport::SubscriberPtr command_sub;     ///< \brief Subscriber handle.
  transport::PublisherPtr pos_pub, vel_pub; ///< \brief Publisher handles.
};
