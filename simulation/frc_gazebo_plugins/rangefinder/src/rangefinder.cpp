#include "rangefinder.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/sensors/sensors.hh>

#include <boost/pointer_cast.hpp>

#include "msgs/msgs.h"

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
