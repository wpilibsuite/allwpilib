/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
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

#include "Log.h"
#include "Notifier.h"

#pragma comment(lib, "Iphlpapi.lib")

using namespace cs;

class NetworkListener::Pimpl {
 public:
  HANDLE eventHandle = 0;
};

// Static Callback function for NotifyIpInterfaceChange API.
static void WINAPI OnInterfaceChange(PVOID callerContext,
                                     PMIB_IPINTERFACE_ROW row,
                                     MIB_NOTIFICATION_TYPE notificationType) {
  Notifier::GetInstance().NotifyNetworkInterfacesChanged();
}

NetworkListener::NetworkListener() { m_data = std::make_unique<Pimpl>(); }

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  NotifyIpInterfaceChange(AF_INET, OnInterfaceChange, nullptr, true,
                          &m_data->eventHandle);
}

void NetworkListener::Stop() {
  if (m_data->eventHandle) {
    CancelMibChangeNotify2(m_data->eventHandle);
  }
}
