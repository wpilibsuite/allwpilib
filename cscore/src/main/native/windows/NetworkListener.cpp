/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkListener.h"

#include <winsock2.h>  // NOLINT(build/include_order)

#include <windows.h>  // NOLINT(build/include_order)

#include <ws2def.h>  // NOLINT(build/include_order)

#include <ws2ipdef.h>  // NOLINT(build/include_order)

#include <iphlpapi.h>  // NOLINT(build/include_order)

#include <netioapi.h>  // NOLINT(build/include_order)

#pragma comment(lib, "Iphlpapi.lib")

using namespace cs;

class NetworkListener::Impl {
 public:
  HANDLE eventHandle = 0;
};

// Static Callback function for NotifyIpInterfaceChange API.
static void WINAPI OnInterfaceChange(PVOID callerContext,
                                     PMIB_IPINTERFACE_ROW row,
                                     MIB_NOTIFICATION_TYPE notificationType) {
  NetworkListener* listener = reinterpret_cast<NetworkListener*>(callerContext);
  listener->interfacesChanged();
}

NetworkListener::NetworkListener(wpi::Logger& logger)
    : m_impl(std::make_unique<Impl>()) {}

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  NotifyIpInterfaceChange(AF_INET, OnInterfaceChange, this, true,
                          &m_impl->eventHandle);
}

void NetworkListener::Stop() {
  if (m_impl->eventHandle) {
    CancelMibChangeNotify2(m_impl->eventHandle);
  }
}
