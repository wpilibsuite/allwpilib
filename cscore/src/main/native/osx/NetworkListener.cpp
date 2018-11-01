/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkListener.h"

#include "Log.h"
#include "Notifier.h"

using namespace cs;

class NetworkListener::Pimpl {};

NetworkListener::NetworkListener(wpi::Logger& logger, Notifier& notifier)
    : m_logger(logger), m_notifier(notifier) {
  m_data = std::make_unique<Pimpl>();
}

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {}

void NetworkListener::Stop() {}
