/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkListener.h"

#ifdef __linux__
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "Log.h"
#include "Notifier.h"

using namespace cs;

ATOMIC_STATIC_INIT(NetworkListener)

class NetworkListener::Thread : public wpi::SafeThread {
 public:
  void Main();

#ifdef __linux__
  int m_command_fd = -1;
#endif
};

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  auto thr = m_owner.GetThread();
  if (!thr) m_owner.Start();
}

void NetworkListener::Stop() {
  // Wake up thread
  if (auto thr = m_owner.GetThread()) {
    thr->m_active = false;
#ifdef __linux__
    if (thr->m_command_fd >= 0) eventfd_write(thr->m_command_fd, 1);
#endif
  }
  m_owner.Stop();
}

void NetworkListener::Thread::Main() {
#ifdef __linux__
  // Create event socket so we can be shut down
  m_command_fd = ::eventfd(0, 0);
  if (m_command_fd < 0) {
    ERROR("NetworkListener: could not create eventfd: " << strerror(errno));
    return;
  }

  // Create netlink socket
  int sd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (sd < 0) {
    ERROR("NetworkListener: could not create socket: " << strerror(errno));
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
    ERROR("NetworkListener: could not create socket: " << strerror(errno));
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
      ERROR("NetworkListener: select(): " << strerror(errno));
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
      ERROR("NetworkListener: could not read netlink: " << strerror(errno));
      break;  // XXX: is this the right thing to do here?
    }
    if (len == 0) continue;  // EOF?
    for (struct nlmsghdr* nh = reinterpret_cast<struct nlmsghdr*>(buf);
         NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
      if (nh->nlmsg_type == NLMSG_DONE) break;
      if (nh->nlmsg_type == RTM_NEWLINK || nh->nlmsg_type == RTM_DELLINK ||
          nh->nlmsg_type == RTM_NEWADDR || nh->nlmsg_type == RTM_DELADDR) {
        Notifier::GetInstance().NotifyNetworkInterfacesChanged();
      }
    }
  }
  ::close(sd);
  ::close(m_command_fd);
  m_command_fd = -1;
#endif
}
