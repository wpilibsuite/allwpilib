
#include "simulation/msgs/float64.pb.h"
#include "simulation/msgs/bool.pb.h"
#include "simulation/msgs/joystick.pb.h"
#include "simulation/msgs/driver-station.pb.h"

#include <gazebo/msgs/msgs.hh>
#include <boost/shared_ptr.hpp>

#ifndef _FRC_MSGS_H_
#define _FRC_MSGS_H_

namespace gazebo {
  namespace msgs {

    typedef GzString String;
    typedef boost::shared_ptr< gazebo::msgs::String > StringPtr;
    typedef const boost::shared_ptr< const gazebo::msgs::String > ConstStringPtr;

    typedef boost::shared_ptr< msgs::Float64 > Float64Ptr;
    typedef const boost::shared_ptr< const msgs::Float64 > ConstFloat64Ptr;

    typedef boost::shared_ptr< msgs::Bool > BoolPtr;
    typedef const boost::shared_ptr< const msgs::Bool > ConstBoolPtr;
    
    typedef boost::shared_ptr< msgs::Joystick > JoystickPtr;
    typedef const boost::shared_ptr< const msgs::Joystick > ConstJoystickPtr;

    typedef boost::shared_ptr< msgs::DriverStation > DriverStationPtr;
    typedef const boost::shared_ptr< const msgs::DriverStation > ConstDriverStationPtr;
  }
}

#endif /* _FRC_MSGS_H_ */
