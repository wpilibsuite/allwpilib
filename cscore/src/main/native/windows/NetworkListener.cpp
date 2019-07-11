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

#include "Instance.h"
#include "Log.h"
#include "Notifier.h"

#pragma comment(lib, "Iphlpapi.lib")

using namespace cs;

class NetworkListener::Impl {
 public:
  Impl(wpi::Logger& logger, Notifier& notifier)
      : m_logger(logger), m_notifier(notifier) {}
  wpi::Logger& m_logger;
  Notifier& m_notifier;
  HANDLE eventHandle = 0;
};

// Static Callback function for NotifyIpInterfaceChange API.
static void WINAPI OnInterfaceChange(PVOID callerContext,
                                     PMIB_IPINTERFACE_ROW row,
                                     MIB_NOTIFICATION_TYPE notificationType) {
  Notifier* notifier = reinterpret_cast<Notifier*>(callerContext);
  notifier->NotifyNetworkInterfacesChanged();
}

NetworkListener::NetworkListener(wpi::Logger& logger, Notifier& notifier)
    : m_impl(std::make_unique<Impl>(logger, notifier)) {}

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  NotifyIpInterfaceChange(AF_INET, OnInterfaceChange, &m_impl->m_notifier, true,
                          &m_impl->eventHandle);
}

void NetworkListener::Stop() {
  if (m_impl->eventHandle) {
    CancelMibChangeNotify2(m_impl->eventHandle);
  }
}
