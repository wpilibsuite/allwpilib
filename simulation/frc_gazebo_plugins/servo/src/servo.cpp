#ifdef _WIN32
  // Ensure that Winsock2.h is included before Windows.h, which can get
  // pulled in by anybody (e.g., Boost).
  #include <Winsock2.h>
#endif

#include "servo.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

GZ_REGISTER_MODEL_PLUGIN(Servo)

Servo::Servo() {}

Servo::~Servo() {}

void Servo::Load(physics::ModelPtr model, sdf::ElementPtr sdf){
	this->model = model;
	signal = 0;

	//parse SDF Properries
	joint = model->GetJoint(sdf->Get<std::string>("joint"));
	if (sdf->HasElement("topic")) {
		topic = sdf->Get<std::string>("topic");
	}
	else {
		topic = "~/"+sdf->GetAttribute("name")->GetAsString();
	}

	if (sdf->HasElement("torque")) {
		torque = sdf->Get<double>("torque");
	}
	else {
		torque = 5;
	}

	gzmsg << "initializing awesome servo: " << topic
		<< " joint=" << joint->GetName()
		<< " torque=" << torque << std::endl;

	//Connect to Gazebo transport for messaging
	std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
	boost::replace_all(scoped_name, "::","/");
	node = transport::NodePtr(new transport::Node());
	node->Init(scoped_name);
	sub = node->Subscribe(topic, &Servo::Callback, this);

	//connect to the world update event
	//this will call update every iteration
	updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&Servo::Update, this, _1));
}

void Servo::Update(const common::UpdateInfo &info){
	//torque is in kg*cm
	//joint->SetAngle(0,signal*180);
  if (joint->GetAngle(0) < signal){
    joint->SetForce(0,torque);
  }
  else if (joint->GetAngle(0) > signal){
    joint->SetForce(0,torque);
  }
  joint->SetForce(0,0);
}

void Servo::Callback(const msgs::ConstFloat64Ptr &msg){
  	signal = msg->data();
    if (signal < -1) { signal = -1; }
    else if (signal > 1) { signal = 1; }
}
