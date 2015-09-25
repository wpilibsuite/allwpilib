
#ifndef _SIM_MAIN_NODE_H
#define _SIM_MAIN_NODE_H

#include "simulation/gz_msgs/msgs.h"
#include <gazebo/transport/transport.hh>
#include <gazebo/gazebo_client.hh>

using namespace gazebo;

class MainNode {
public:
	static MainNode* GetInstance() {
		static MainNode instance;
		return &instance;
	}

	template<typename M>
	static transport::PublisherPtr Advertise(const std::string &topic,
                                             unsigned int _queueLimit = 10,
                                             bool _latch = false) {
		return GetInstance()->main->Advertise<M>(topic, _queueLimit, _latch);
	}

	template<typename M, typename T>
	static transport::SubscriberPtr Subscribe(const std::string &topic,
                            void(T::*fp)(const boost::shared_ptr<M const> &), T *obj,
                            bool _latching = false) {
		return GetInstance()->main->Subscribe(topic, fp, obj, _latching);
	}

	template<typename M>
	static transport::SubscriberPtr Subscribe(const std::string &topic,
                            void(*fp)(const boost::shared_ptr<M const> &),
                            bool _latching = false) {
		return GetInstance()->main->Subscribe(topic, fp, _latching);
	}

	transport::NodePtr main;
private:
	MainNode() {
		gazebo::client::setup();
		main = transport::NodePtr(new transport::Node());
		main->Init("frc");
		gazebo::transport::run();
	}
};

#endif
