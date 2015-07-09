/*
 * NamedSendable.h
 *
 *  Created on: Oct 19, 2012
 *      Author: Mitchell Wills
 */

#ifndef NAMEDSENDABLE_H_
#define NAMEDSENDABLE_H_

#include <string>
#include "SmartDashboard/Sendable.h"

/**
 * The interface for sendable objects that gives the sendable a default name in
 * the Smart Dashboard
 *
 */
class NamedSendable : public Sendable {
 public:
  /**
   * @return the name of the subtable of SmartDashboard that the Sendable object
   * will use
   */
  virtual std::string GetName() const = 0;
};

#endif /* NAMEDSENDABLE_H_ */
