
#include "msgs/msgs.h"

#include <gazebo/transport/transport.hh>
#include <boost/shared_ptr.hpp>

#ifndef _ROBOTCOMPONENT_H_
#define _ROBOTCOMPONENT_H_

#define MSG_BUFFER_SIZE 1000

using namespace gazebo;

class RobotComponent {
public:
  RobotComponent() {}
  virtual ~RobotComponent() {}

  virtual void connect(transport::NodePtr node) {};
  virtual void update(bool enabled) {};
};

typedef RobotComponent *RobotComponentPtr;

#endif /* _ROBOTCOMPONENT_H_ */
