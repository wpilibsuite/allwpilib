/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkListener.h"

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>

#include <wpi/SafeThread.h>

#include "Log.h"
#include "Notifier.h"

using namespace cs;

class NetworkListener::Impl {
 public:
  Impl(wpi::Logger& logger, Notifier& notifier)
      : m_logger(logger), m_notifier(notifier) {}

  wpi::Logger& m_logger;
  Notifier& m_notifier;

  class Thread : public wpi::SafeThread {
   public:
    Thread(wpi::Logger& logger, Notifier& notifier)
        : m_logger(logger), m_notifier(notifier) {}
    void Main();

    wpi::Logger& m_logger;
    Notifier& m_notifier;
    int m_command_fd = -1;
  };

  wpi::SafeThreadOwner<Thread> m_owner;
};

NetworkListener::NetworkListener(wpi::Logger& logger, Notifier& notifier)
    : m_impl(std::make_unique<Impl>(logger, notifier)) {}

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  m_impl->m_owner.Start(m_impl->m_logger, m_impl->m_notifier);
}

void NetworkListener::Stop() {
  // Wake up thread
  if (auto thr = m_impl->m_owner.GetThread()) {
    thr->m_active = false;
    if (thr->m_command_fd >= 0) eventfd_write(thr->m_command_fd, 1);
  }
  m_impl->m_owner.Stop();
}

void NetworkListener::Impl::Thread::Main() {
  // Create event socket so we can be shut down
  m_command_fd = ::eventfd(0, 0);
  if (m_command_fd < 0) {
    ERROR(
        "NetworkListener: could not create eventfd: " << std::strerror(errno));
    return;
  }

  // Create netlink socket
  int sd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (sd < 0) {
    ERROR("NetworkListener: could not create socket: " << std::strerror(errno));
    ::close(m_command_fd);
    m_command_fd = -1;
    return;
  }

  // Bind to netlink socket
  struct sockaddr_nl addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;
  if (bind(sd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    ERROR("NetworkListener: could not create socket: " << std::strerror(errno));
    ::close(sd);
    ::close(m_command_fd);
    m_command_fd = -1;
    return;
  }

  char buf[4096];

  while (m_active) {
    // select timeout
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    // select on applicable read descriptors
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_command_fd, &readfds);
    FD_SET(sd, &readfds);
    int nfds = std::max(m_command_fd, sd) + 1;

    if (::select(nfds, &readfds, nullptr, nullptr, &tv) < 0) {
      ERROR("NetworkListener: select(): " << std::strerror(errno));
      break;  // XXX: is this the right thing to do here?
    }

    // Double-check to see if we're shutting down
    if (!m_active) break;

    if (!FD_ISSET(sd, &readfds)) continue;

    std::memset(&addr, 0, sizeof(addr));
    struct iovec iov = {buf, sizeof(buf)};
    struct msghdr msg = {&addr, sizeof(addr), &iov, 1, nullptr, 0, 0};
    int len = ::recvmsg(sd, &msg, 0);
    if (len < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) continue;
      ERROR(
          "NetworkListener: could not read netlink: " << std::strerror(errno));
      break;  // XXX: is this the right thing to do here?
    }
    if (len == 0) continue;  // EOF?
    for (struct nlmsghdr* nh = reinterpret_cast<struct nlmsghdr*>(buf);
         NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
      if (nh->nlmsg_type == NLMSG_DONE) break;
      if (nh->nlmsg_type == RTM_NEWLINK || nh->nlmsg_type == RTM_DELLINK ||
          nh->nlmsg_type == RTM_NEWADDR || nh->nlmsg_type == RTM_DELADDR) {
        m_notifier.NotifyNetworkInterfacesChanged();
      }
    }
  }
  ::close(sd);
  ::close(m_command_fd);
  m_command_fd = -1;
}
