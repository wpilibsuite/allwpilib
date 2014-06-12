#ifndef _driveBot_H_
#define _driveBot_H_

#include <gazebo/gazebo.hh>
#include <gazebo/physics/PhysicsIface.hh>

#include "components/RobotComponent.h"
#include "msgs/msgs.h"

using namespace gazebo;

class FRCPlugin: public ModelPlugin {
public: 
  FRCPlugin();
  ~FRCPlugin();
  //function to load all  of the elements
  void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
  //function to update all of the elements
  void Update(const common::UpdateInfo & _info);

private:
  //the vector containing all of the robot components.
  std::vector<RobotComponentPtr> components;

  physics::ModelPtr model;

  physics::LinkPtr frame;

  /// \brief Pointer to the world update function.
  event::ConnectionPtr updateConn;

  transport::NodePtr gzNode;

  transport::SubscriberPtr sub;
  volatile bool enabled;
  void dsCallback(const msgs::ConstDriverStationPtr &msg);
};

#endif
