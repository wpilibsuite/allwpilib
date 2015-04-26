#include "clock.h"

GZ_REGISTER_MODEL_PLUGIN(Clock)

Clock::Clock() {}

Clock::~Clock() {}

void Clock::Load(physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/"+sdf->GetAttribute("name")->GetAsString();
  }

  gzmsg << "Initializing clock: " << topic << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = transport::NodePtr(new transport::Node());
  node->Init(scoped_name);
  pub = node->Advertise<msgs::Float64>(topic);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&Clock::Update, this, _1));
}

void Clock::Update(const common::UpdateInfo &info) {
  msgs::Float64 msg;
  msg.set_data(info.simTime.Double());
  pub->Publish(msg);
}
