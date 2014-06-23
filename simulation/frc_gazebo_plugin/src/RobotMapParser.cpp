
#include "RobotMapParser.h"

#include "components/Motor.h"
#include "components/Piston.h"
#include "components/Potentiometer.h"
#include "components/Encoder.h"
#include "components/Gyro.h"
#include "components/InternalLimitSwitch.h"
#include "components/ExternalLimitSwitch.h"
#include "components/Rangefinder.h"
#include <string.h>
#include <boost/lexical_cast.hpp>
#include <boost/pointer_cast.hpp>
#include <sdf/sdf.hh>

RobotMapParser::RobotMapParser(string robot, string filename, physics::ModelPtr model) {
  this->robot = robot;
  this->filename = filename;
  this->model = model;
  this->world = model->GetWorld();
}

/*
 * Parse the file and return a list of robot components for the robot
 */
void RobotMapParser::parse(vector<RobotComponentPtr> &components) {
  // Load an xml document with <robotmap> root tag
  XMLDocument doc;
  doc.LoadFile(filename.c_str());

  // Iterate over all of the child tags and parse them accordingly.
  for (XMLNode *current = doc.FirstChildElement("robotmap")->FirstChild();
       current != NULL; current = current->NextSibling()) {
    if (current->ToElement() == NULL) {
      // We don't care about comments and other non-element types
    } else if (strcmp(current->Value(), "motor") == 0) {
      components.push_back(parseMotor(current->ToElement()));
    } else if (strcmp(current->Value(), "piston") == 0) {
      components.push_back(parsePiston(current->ToElement()));
    } else if (strcmp(current->Value(), "potentiometer") == 0) {
      components.push_back(parsePotentiometer(current->ToElement()));
    } else if (strcmp(current->Value(), "encoder") == 0) {
      components.push_back(parseEncoder(current->ToElement()));
    } else if (strcmp(current->Value(), "gyro") == 0) {
      components.push_back(parseGyro(current->ToElement()));
    } else if (strcmp(current->Value(), "internal-limit-switch") == 0) {
      components.push_back(parseInternalLimitSwitch(current->ToElement()));
    } else if (strcmp(current->Value(), "external-limit-switch") == 0) {
      components.push_back(parseExternalLimitSwitch(current->ToElement()));
    } else if (strcmp(current->Value(), "rangefinder") == 0) {
      components.push_back(parseRangefinder(current->ToElement()));
    } else {
      cerr << "Unknown element: " << current->Value() << endl;
    }
  }
}

/**
 * Parse a motor tag. Motor tags have a joint the act on, a location
 * they're "plugged into" and parameters that control their
 * behavior. The template of a motor tag is as follows:
 *
 *  <motor>
 *    <joint>{{ joint name (string) }}</joint>
 *    <location>{{ location (see below for formats }}</location>
 *    <parameters>
 *      <multiplier>{{ torque multiplication factor (number) }}</multiplier>
 *    </parameters>
 *  </motor>
 */
RobotComponentPtr RobotMapParser::parseMotor(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);
  double multiplier = boost::lexical_cast<double>(getChildTagValue(
      node->FirstChildElement("parameters"), "multiplier").c_str());
  return new Motor(location, joint, multiplier);
}

/**
 * Parse a piston tag. Piston tags have a joint the act on, a location
 * they're "plugged into", the area of the cylinder and optionally a
 * direction. The template of a piston| tag is as follows:
 *
 *  <piston>
 *    <joint>{{ joint name (string) }}</joint>
 *    <location>{{ location (see below for formats }}</location>
 *    <area units="{{ square inches (default) or square meters}}">
 *      {{ area in proper units (number) }}
 *    </area>
 *    <forward-force>{{ area in newtons (number) }}</forward-force>
 *    <reverse-force>{{ area in newtons (number) }}</reverse-force>
 *    <direction>{{ forward (default) or reversed }}</direction>
 *  </piston>
 */
RobotComponentPtr RobotMapParser::parsePiston(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);
  double forward = boost::lexical_cast<double>(getChildTagValue(node, "forward-force").c_str());
  double reverse = boost::lexical_cast<double>(getChildTagValue(node, "reverse-force").c_str());
  if (getChildTagValue(node, "direction") == "reversed") {
    forward = -forward;
    reverse = -reverse;
  }
  return new Piston(location, joint, forward, reverse);
}

/**
 * Parse a potentiometer tag. Potentiometer tags have a joint they measure, a
 * location they're "plugged into" and one of three types (linear,
 * degrees or radians). The template of a potentiometer tag is as
 * follows:
 *
 *  <potentiometer>
 *    <joint>{{ joint name (string) }}</joint>
 *    <location>{{ location (see below for formats }}</location>
 *    <type>{{ one of linear, degrees, radians }}</type>
 *  </potentiometer>
 */
RobotComponentPtr RobotMapParser::parsePotentiometer(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);

  string radians_str = getChildTagValue(node, "type", "degrees");
  bool radians;
  if (radians_str == "linear" || radians_str == "radians") {
    radians = true;
  } else {
    radians = false;
  }
  
  return new Potentiometer(location, joint, radians);
}

/**
 * Parse an encoder tag. Encoder tags have a joint they measure, a
 * location they're "plugged into". The template of an encoder tag
 * is as follows:
 *
 *  <encoder>
 *    <joint>{{ joint name (string) }}</joint>
 *    <location>{{ location (see below for formats }}</location>
 *  </encoder>
 */
RobotComponentPtr RobotMapParser::parseEncoder(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);
  return new Encoder(location, joint);
}

/**
 * Parse a gyro tag. Gyro tags have a joint they measure, a location
 * they're "plugged into" and one of three axes (roll, pitch,
 * yaw). The template of a gyro tag is as follows:
 *
 *  <gyro>
 *    <link>{{ link name (string) }}</link>
 *    <location>{{ location (see below for formats }}</location>
 *    <axis>{{ one of yaw, pitch, roll }}</axis>
 *  </gyro>
 */
RobotComponentPtr RobotMapParser::parseGyro(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::LinkPtr link = getLink(node);
  ROTATION axis;
  string axisString = getChildTagValue(node, "axis");
  if (axisString == "roll") axis = Roll;
  if (axisString == "pitch") axis = Pitch;
  if (axisString == "yaw") axis = Yaw;
  return new Gyro(location, link, axis);
}

/**
 * Parse a InternalLimitSwitch tag. InternalLimitSwitch tags have a
 * joint they measure and a location they're "plugged into". The
 * template of an InteranlLimitSwitch tag is as follows:
 *
 *  <internal-limit-switch>
 *    <joint>{{ joint name (string) }}</joint>
 *    <location>{{ location (see below for formats }}</location>
 *    <type>{{ one of linear, degrees, radians }}</type>
 *    <min>{{ min (number) }}</min>
 *    <max>{{ max (number }}</max>
 *  </internal-limit-switch>
 */
RobotComponentPtr RobotMapParser::parseInternalLimitSwitch(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);

  double min = boost::lexical_cast<double>(getChildTagValue(node, "min", "0"));
  double max = boost::lexical_cast<double>(getChildTagValue(node, "max", "1"));

  string radians_str = getChildTagValue(node, "type", "degrees");
  bool radians;
  if (radians_str == "linear" || radians_str == "radians") {
    radians = true;
  } else {
    radians = false;
  }
  return new InternalLimitSwitch(location, joint, min, max, radians);
}

/**
 * Parse a ExternalLimitSwitch tag. ExternalLimitSwitch tags have a
 * joint they measure and a location they're "plugged into". The
 * template of an ExternalLimitSwitch tag is as follows:
 *
 *  <external-limit-switch>
 *    <sensor>{{ sensor name (string) }}</sensor>
 *    <location>{{ location (see below for formats }}</location>
 *  </external-limit-switch>
 */
RobotComponentPtr RobotMapParser::parseExternalLimitSwitch(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  sensors::ContactSensorPtr sensor =
    boost::dynamic_pointer_cast<sensors::ContactSensor>(getSensor(node));
  return new ExternalLimitSwitch(location, sensor);
}

/**
 * Parse a rangefinder tag. Rangefinder tags have a joint they measure
 * and a location they're "plugged into". The template of a rangfinder
 * tag is as follows:
 *
 *  <rangefinder>
 *    <sensor>{{ sensor name (string) }}</sensor>
 *    <location>{{ location (see below for formats }}</location>
 *  </rangefinder>
 */
RobotComponentPtr RobotMapParser::parseRangefinder(XMLElement *node) {
  string location = locationToPath(node->FirstChildElement("location"));
  sensors::SonarSensorPtr sensor =
    boost::dynamic_pointer_cast<sensors::SonarSensor>(getSensor(node));
  return new Rangefinder(location, sensor);
}


/**
 * Convert the location tag to a topic name to subscribe/publish
 * to. Currently, there are two supported location styles: double_port
 * and single port.
 */
string RobotMapParser::locationToPath(XMLElement *location) {
  if (location == NULL)
    return "";
  else if (location->Attribute("style", "double_port"))
    return doublePortLocationToPath(location);
  else
    return portLocationToPath(location);
}

string RobotMapParser::portLocationToPath(XMLElement *location) {
  string type = getChildTagValue(location, "type");
  string module = getChildTagValue(location, "module");
  string pin = getChildTagValue(location, "pin");
  return "~/" + robot + "/" + type + "/" + module + "/" + pin;
}


string RobotMapParser::doublePortLocationToPath(XMLElement *location) {
  string type = getChildTagValue(location, "type");
  string module1 = getChildTagValue(location, "module1");
  string pin1 = getChildTagValue(location, "pin1");
  string module2 = getChildTagValue(location, "module2");
  string pin2 = getChildTagValue(location, "pin2");
  // Swap pins if needed, to maintain consistent ordering
  if ((module2 < module1) || ((module2 == module1) && (pin2 < pin1))) { 
    string module = module2;
    string pin = pin2;
    module2 = module1;
    pin2 = pin1;
    module1 = module;
    pin1 = pin;
  }
  return "~/" + robot + "/" + type + "/" + module1 + "/" + pin1 + "/" + module2 + "/" + pin2;
}

/**
 * Returns the value of the first child tag element of the given node
 * with the matching tag name.
 */
string RobotMapParser::getChildTagValue(XMLElement *node, string tag, string default_value) {
  if (node->FirstChildElement(tag.c_str()) != NULL) {
    return node->FirstChildElement(tag.c_str())->FirstChild()->Value();
  } else {
    return default_value;
  }
}

/**
 * Returns the matching attribute of the given node.
 */
string RobotMapParser::getTagAttribute(XMLElement *node, string attr, string default_value) {
  if (node->Attribute(attr.c_str())) {
    return node->Attribute(attr.c_str());
  } else {
    return default_value;
  }
}

/**
 * Returns the matching attribute of the first child tag element of the given node
 * with the matching tag name.
 */
string RobotMapParser::getChildTagAttribute(XMLElement *node, string tag,
                                       string attr, string default_value) {
  if (node->FirstChildElement(tag.c_str()) != NULL) {
    string value = node->FirstChildElement(tag.c_str())->Attribute(attr.c_str());
    if (value != "") {
      return value;
    }
  }
  return default_value;
}

physics::JointPtr RobotMapParser::getJoint(XMLElement *node) {
  string joint_name = node->FirstChildElement("joint")->FirstChild()->Value();
  physics::JointPtr joint = model->GetJoint(joint_name);
  if (joint == NULL) cerr << "Joint doesn't exist: " << joint_name << endl;
  return joint;
}

physics::LinkPtr RobotMapParser::getLink(XMLElement *node) {
  string link_name = node->FirstChildElement("link")->FirstChild()->Value();
  physics::LinkPtr link = model->GetLink(link_name);
  if (link == NULL) cerr << "Link doesn't exist: " << link_name << endl;
  return link;
}

sensors::SensorPtr RobotMapParser::getSensor(XMLElement *node) {
  string sensor_name = node->FirstChildElement("sensor")->FirstChild()->Value();
  sensors::SensorPtr sensor = sensors::get_sensor(sensor_name);
  if (sensor == NULL) cerr << "Sensor doesn't exist: " << sensor_name << endl;
  return sensor;
}
