/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifdef _WIN32
  // Ensure that Winsock2.h is included before Windows.h, which can get
  // pulled in by anybody (e.g., Boost).
  #include <Winsock2.h>
#endif

#include "rangefinder.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/sensors/sensors.hh>

#include <boost/pointer_cast.hpp>

GZ_REGISTER_MODEL_PLUGIN(Rangefinder)

Rangefinder::Rangefinder() {}

Rangefinder::~Rangefinder() {}

void Rangefinder::Load(physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  sensor = boost::dynamic_pointer_cast<sensors::SonarSensor>(
               sensors::get_sensor(sdf->Get<std::string>("sensor")));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/"+sdf->GetAttribute("name")->GetAsString();
  }

  gzmsg << "Initializing rangefinder: " << topic << " sensor=" << sensor->GetName() << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = transport::NodePtr(new transport::Node());
  node->Init(scoped_name);
  pub = node->Advertise<msgs::Float64>(topic);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&Rangefinder::Update, this, _1));
}

void Rangefinder::Update(const common::UpdateInfo &info) {
  msgs::Float64 msg;
  msg.set_data(sensor->GetRange());
  pub->Publish(msg);
}
