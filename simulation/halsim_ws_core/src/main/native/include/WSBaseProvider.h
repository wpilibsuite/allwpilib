/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <wpi/json.h>

#include "HALSimBaseWebSocketConnection.h"

namespace wpilibws {

class HALSimWSBaseProvider {
 public:
  explicit HALSimWSBaseProvider(const std::string& key);
  virtual ~HALSimWSBaseProvider() {}

  HALSimWSBaseProvider(const HALSimWSBaseProvider&) = delete;
  HALSimWSBaseProvider& operator=(const HALSimWSBaseProvider&) = delete;

  // Called when the websocket connects. The provider should send
  // initial data to the websocket and store the SendFunc to
  // use for later sends
  virtual void OnNetworkConnected(std::shared_ptr<HALSimBaseWebSocketConnection> ws) = 0;

  // network -> sim
  virtual void OnNetValueChanged(const wpi::json& json);

 protected:
  // sim -> network
  std::weak_ptr<HALSimBaseWebSocketConnection> m_ws;
  std::string m_key;
};

}