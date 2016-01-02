/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
		bool success = gazebo::client::setup();

    if (success){
  		main = transport::NodePtr(new transport::Node());
  		main->Init("frc");
   		gazebo::transport::run();
    }
    else {
      std::cout << "An error has occured setting up gazebo_client!" << std::endl;
    }
	}
};

#endif
