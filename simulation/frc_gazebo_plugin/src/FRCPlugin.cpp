
#include "FRCPlugin.h"

#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/physics/Joint.hh>
#include <gazebo/physics/PhysicsEngine.hh>
#include <gazebo/sensors/sensors.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

#include <string.h>
#include <boost/lexical_cast.hpp>

#include "RobotMapParser.h"
#include "components/Motor.h"
#include "components/Potentiometer.h"
#include "components/Encoder.h"

using namespace gazebo;
GZ_REGISTER_MODEL_PLUGIN(FRCPlugin)

FRCPlugin::FRCPlugin() {}

FRCPlugin::~FRCPlugin() {}


void FRCPlugin::Load(physics::ModelPtr _model, sdf::ElementPtr _sdf) {
  model = _model;

  // Create robot components from RobotMap
  if (_sdf->HasElement("robotmap")) {
    std::string robotmap = common::SystemPaths::Instance()->FindFileURI(_sdf->Get<std::string>("robotmap"));
    std::cout << "Loading RobotMap: " << robotmap << std::endl;
    RobotMapParser map("simulator", robotmap, model);
    map.parse(components);
  } else {
    std::cerr << "No RobotMap specified this robot cannnot be controlled." << std::endl;
  }

  // Connect to Gazebo transport for messaging
  gzNode = transport::NodePtr(new transport::Node());
  gzNode->Init("frc");
  for (int i = 0; i < components.size(); i++) {
    components[i]->connect(gzNode);
  }
  enabled = false;
  sub = gzNode->Subscribe("~/ds/state", &FRCPlugin::dsCallback, this);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&FRCPlugin::Update, this, _1));
}

// Update all RobotComponents
void FRCPlugin::Update(const common::UpdateInfo &_info) {
  for (int i = 0; i < components.size(); i++) {
    components[i]->update(enabled);
  }
}


void FRCPlugin::dsCallback(const msgs::ConstDriverStationPtr &msg) {
  enabled = msg->enabled();
}
