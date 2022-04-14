// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "potentiometer.h"

#include <boost/algorithm/string/replace.hpp>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

GZ_REGISTER_MODEL_PLUGIN(Potentiometer)

void Potentiometer::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }

  if (sdf->HasElement("units")) {
    radians = sdf->Get<std::string>("units") != "degrees";
  } else {
    radians = true;
  }

  multiplier = 1.0;
  if (sdf->HasElement("multiplier"))
    multiplier = sdf->Get<double>("multiplier");

  gzmsg << "Initializing potentiometer: " << topic
        << " joint=" << joint->GetName() << " radians=" << radians
        << " multiplier=" << multiplier << std::endl;

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
      boost::bind(&Potentiometer::Update, this, _1));
}

void Potentiometer::Update(const gazebo::common::UpdateInfo& info) {
  gazebo::msgs::Float64 msg;
  if (radians) {
    msg.set_data(joint->Position(0) * multiplier);
  } else {
    msg.set_data(joint->Position(0) * (180.0 / M_PI) * multiplier);
  }
  pub->Publish(msg);
}
