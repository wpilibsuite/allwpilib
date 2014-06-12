
#include "components/RobotComponent.h"
#include <gazebo/sensors/sensors.hh>
#include <gazebo/physics/Model.hh>

#include <tinyxml2.h>

#ifndef _ROBOTMAPPARSER_H_
#define _ROBOTMAPPARSER_H_

using namespace tinyxml2;
using namespace gazebo;

class RobotMapParser {
public:
  RobotMapParser(std::string robot, std::string filename, physics::ModelPtr model);
  void parse(std::vector<RobotComponentPtr> &components);

private:
  std::string robot, filename;
  physics::ModelPtr model;
  physics::WorldPtr world;
  
  RobotComponentPtr parseMotor(XMLElement *node);
  RobotComponentPtr parsePotentiometer(XMLElement *node);
  RobotComponentPtr parseEncoder(XMLElement *node);
  RobotComponentPtr parseGyro(XMLElement *node);
  RobotComponentPtr parseInternalLimitSwitch(XMLElement *node);
  RobotComponentPtr parseExternalLimitSwitch(XMLElement *node);
  RobotComponentPtr parseRangefinder(XMLElement *node);
  
  std::string locationToPath(XMLElement *location);
  std::string portLocationToPath(XMLElement *location);
  std::string doublePortLocationToPath(XMLElement *location);

  std::string getTagValue(XMLElement *node, std::string tag, std::string default_value="");
  physics::JointPtr getJoint(XMLElement *node);
  physics::LinkPtr getLink(XMLElement *node);
  sensors::SensorPtr getSensor(XMLElement *node);
};

#endif /* _ROBOTMAPPARSER_H_ */
