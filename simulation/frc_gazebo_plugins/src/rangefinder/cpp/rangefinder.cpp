// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "rangefinder.h"

#include <boost/algorithm/string/replace.hpp>
#include <gazebo/physics/physics.hh>
#include <gazebo/sensors/sensors.hh>
#include <gazebo/transport/transport.hh>

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

GZ_REGISTER_MODEL_PLUGIN(Rangefinder)

void Rangefinder::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  sensor = std::dynamic_pointer_cast<gazebo::sensors::SonarSensor>(
      gazebo::sensors::get_sensor(sdf->Get<std::string>("sensor")));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }

  gzmsg << "Initializing rangefinder: " << topic << " sensor=" << sensor->Name()
        << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name =
      model->GetWorld()->Name() + "::" + model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init(scoped_name);
  pub = node->Advertise<gazebo::msgs::Float64>(topic);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind(&Rangefinder::Update, this, _1));
}

void Rangefinder::Update(const gazebo::common::UpdateInfo& info) {
  gazebo::msgs::Float64 msg;
  msg.set_data(sensor->Range());
  pub->Publish(msg);
}
