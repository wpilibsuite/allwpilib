/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "encoder.h"

GZ_REGISTER_MODEL_PLUGIN(Encoder)

Encoder::Encoder() {}

Encoder::~Encoder() {}

void Encoder::Load(physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/"+sdf->GetAttribute("name")->GetAsString();
  }

  if (sdf->HasElement("units")) {
    radians = sdf->Get<std::string>("units") != "degrees";
  } else {
    radians = true;
  }
  zero = GetAngle();
  stopped = true;
  stop_value = 0;

  gzmsg << "Initializing encoder: " << topic << " joint=" << joint->GetName()
        << " radians=" << radians << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = transport::NodePtr(new transport::Node());
  node->Init(scoped_name);
  command_sub = node->Subscribe(topic+"/control", &Encoder::Callback, this);
  pos_pub = node->Advertise<msgs::Float64>(topic+"/position");
  vel_pub = node->Advertise<msgs::Float64>(topic+"/velocity");

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&Encoder::Update, this, _1));
}

void Encoder::Update(const common::UpdateInfo &info) {
  msgs::Float64 pos_msg, vel_msg;
  if (stopped) {
    pos_msg.set_data(stop_value);
    pos_pub->Publish(pos_msg);
    vel_msg.set_data(0);
    vel_pub->Publish(vel_msg);
  } else {
    pos_msg.set_data(GetAngle() - zero);
    pos_pub->Publish(pos_msg);
    vel_msg.set_data(GetVelocity());
    vel_pub->Publish(vel_msg);
  }
}

void Encoder::Callback(const msgs::ConstStringPtr &msg) {
  std::string command = msg->data();
  if (command == "reset") {
    zero = GetAngle();
  } else if (command == "start") {
    stopped = false;
    zero = (GetAngle() - stop_value);
  } else if  (command == "stop") {
    stopped = true;
    stop_value = GetAngle();
  } else {
    gzerr << "WARNING: Encoder got unknown command '" << command << "'." << std::endl;
  }
}

double Encoder::GetAngle() {
  if (radians) {
    return joint->GetAngle(0).Radian();
  } else {
    return joint->GetAngle(0).Degree();
  }
}

double Encoder::GetVelocity() {
  if (radians) {
    return joint->GetVelocity(0);
  } else {
    return joint->GetVelocity(0) * (180.0 / M_PI);
  }
}

