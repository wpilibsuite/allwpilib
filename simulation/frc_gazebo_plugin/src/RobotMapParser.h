
#include "components/RobotComponent.h"
#include <gazebo/sensors/sensors.hh>
#include <gazebo/physics/Model.hh>

#include <tinyxml2.h>

#ifndef _ROBOTMAPPARSER_H_
#define _ROBOTMAPPARSER_H_

using namespace std;
using namespace tinyxml2;
using namespace gazebo;

class RobotMapParser {
public:
  RobotMapParser(string robot, string filename, physics::ModelPtr model);
  void parse(vector<RobotComponentPtr> &components);

private:
  string robot, filename;
  physics::ModelPtr model;
  physics::WorldPtr world;
  
  RobotComponentPtr parseMotor(XMLElement *node);
  RobotComponentPtr parsePiston(XMLElement *node);
  RobotComponentPtr parsePotentiometer(XMLElement *node);
  RobotComponentPtr parseEncoder(XMLElement *node);
  RobotComponentPtr parseGyro(XMLElement *node);
  RobotComponentPtr parseInternalLimitSwitch(XMLElement *node);
  RobotComponentPtr parseExternalLimitSwitch(XMLElement *node);
  RobotComponentPtr parseRangefinder(XMLElement *node);

  string locationToPath(XMLElement *location);
  string portLocationToPath(XMLElement *location);
  string doublePortLocationToPath(XMLElement *location);

  string getChildTagValue(XMLElement *node, string tag, string default_value="");
  string getTagAttribute(XMLElement *node, string attr, string default_value="");
  string getChildTagAttribute(XMLElement *node, string tag, string attr, string default_value="");
  physics::JointPtr getJoint(XMLElement *node);
  physics::LinkPtr getLink(XMLElement *node);
  sensors::SensorPtr getSensor(XMLElement *node);
};

#endif /* _ROBOTMAPPARSER_H_ */
