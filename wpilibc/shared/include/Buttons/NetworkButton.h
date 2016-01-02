/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __NETWORK_BUTTON_H__
#define __NETWORK_BUTTON_H__

#include "Buttons/Button.h"
#include <string>
#include <memory>

class NetworkButton : public Button {
 public:
  NetworkButton(const std::string &tableName, const std::string &field);
  NetworkButton(std::shared_ptr<ITable> table, const std::string &field);
  virtual ~NetworkButton() = default;

  virtual bool Get();

 private:
  std::shared_ptr<ITable> m_netTable;
  std::string m_field;
};

#endif
