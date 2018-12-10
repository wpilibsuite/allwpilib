/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#ifndef UNIT_JSON_H_
#define UNIT_JSON_H_

#include <ostream>

#include "wpi/json.h"

#include "TestThrow.h"

namespace wpi {

inline
void PrintTo(const json& j, std::ostream* os) {
  *os << j.dump();
}

class JsonTest {
 public:
  static const json::json_value& GetValue(const json& j) {
    return j.m_value;
  }
  static json::pointer GetObject(json::iterator it) {
    return it.m_object;
  }
  static json::const_pointer GetObject(json::const_iterator it) {
    return it.m_object;
  }
  static std::string pop_back(json::json_pointer& p) {
    return p.pop_back();
  }
  static json::json_pointer top(const json::json_pointer& p) {
    return p.top();
  }
};

}  // namespace wpi

#endif
