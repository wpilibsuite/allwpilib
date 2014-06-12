
#include "RobotMapParser.h"

#include "components/Motor.h"
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

RobotMapParser::RobotMapParser(std::string robot, std::string filename, physics::ModelPtr model) {
  this->robot = robot;
  this->filename = filename;
  this->model = model;
  this->world = model->GetWorld();
}

/*
 * Parse the file and return a list of robot components for the robot
 */
void RobotMapParser::parse(std::vector<RobotComponentPtr> &components) {
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
      std::cerr << "Unknown element: " << current->Value() << std::endl;
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
  std::string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);
  double multiplier = boost::lexical_cast<double>(getTagValue(
      node->FirstChildElement("parameters"), "multiplier").c_str());
  return new Motor(location, joint, multiplier);
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
  std::string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);

  std::string radians_str = getTagValue(node, "type", "degrees");
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
  std::string location = locationToPath(node->FirstChildElement("location"));
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
  std::string location = locationToPath(node->FirstChildElement("location"));
  physics::LinkPtr link = getLink(node);
  ROTATION axis;
  std::string axisString = getTagValue(node, "axis");
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
  std::string location = locationToPath(node->FirstChildElement("location"));
  physics::JointPtr joint = getJoint(node);

  double min = boost::lexical_cast<double>(getTagValue(node, "min", "0"));
  double max = boost::lexical_cast<double>(getTagValue(node, "max", "1"));

  std::string radians_str = getTagValue(node, "type", "degrees");
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
  std::string location = locationToPath(node->FirstChildElement("location"));
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
  std::string location = locationToPath(node->FirstChildElement("location"));
  sensors::SonarSensorPtr sensor =
    boost::dynamic_pointer_cast<sensors::SonarSensor>(getSensor(node));
  return new Rangefinder(location, sensor);
}

/**
 * Convert the location tag to a topic name to subscribe/publish
 * to. Currently, there are two supported location styles: double_port
 * and single port.
 */
std::string RobotMapParser::locationToPath(XMLElement *location) {
  if (location == NULL)
    return "";
  else if (location->Attribute("style", "double_port"))
    return doublePortLocationToPath(location);
  else
    return portLocationToPath(location);
}

std::string RobotMapParser::portLocationToPath(XMLElement *location) {
  std::string type = getTagValue(location, "type");
  std::string module = getTagValue(location, "module");
  std::string pin = getTagValue(location, "pin");
  return "~/" + robot + "/" + type + "/" + module + "/" + pin;
}


std::string RobotMapParser::doublePortLocationToPath(XMLElement *location) {
  std::string type = getTagValue(location, "type");
  std::string module1 = getTagValue(location, "module1");
  std::string pin1 = getTagValue(location, "pin1");
  std::string module2 = getTagValue(location, "module2");
  std::string pin2 = getTagValue(location, "pin2");
  // Swap pins if needed, to maintain consistent ordering
  if ((module2 < module1) || ((module2 == module1) && (pin2 < pin1))) { 
    std::string module = module2;
    std::string pin = pin2;
    module2 = module1;
    pin2 = pin1;
    module1 = module;
    pin1 = pin;
  }
  return "~/" + robot + "/" + type + "/" + module1 + "/" + pin1 + "/" + module2 + "/" + pin2;
}

std::string RobotMapParser::getTagValue(XMLElement *node, std::string tag, std::string default_value) {
  if (node->FirstChildElement(tag.c_str()) != NULL) {
    return node->FirstChildElement(tag.c_str())->FirstChild()->Value();
  } else {
    return default_value;
  }
}

physics::JointPtr RobotMapParser::getJoint(XMLElement *node) {
  std::string joint_name = node->FirstChildElement("joint")->FirstChild()->Value();
  physics::JointPtr joint = model->GetJoint(joint_name);
  if (joint == NULL) std::cerr << "Joint doesn't exist: " << joint_name << std::endl;
  return joint;
}

physics::LinkPtr RobotMapParser::getLink(XMLElement *node) {
  std::string link_name = node->FirstChildElement("link")->FirstChild()->Value();
  physics::LinkPtr link = model->GetLink(link_name);
  if (link == NULL) std::cerr << "Link doesn't exist: " << link_name << std::endl;
  return link;
}

sensors::SensorPtr RobotMapParser::getSensor(XMLElement *node) {
  std::string sensor_name = node->FirstChildElement("sensor")->FirstChild()->Value();
  sensors::SensorPtr sensor = sensors::get_sensor(sensor_name);
  if (sensor == NULL) std::cerr << "Sensor doesn't exist: " << sensor_name << std::endl;
  return sensor;
}
