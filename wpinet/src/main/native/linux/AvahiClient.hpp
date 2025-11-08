// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>

/***
  This file is part of avahi.
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

typedef struct AvahiPoll AvahiPoll;

typedef enum {
  AVAHI_SERVER_INVALID,
  AVAHI_SERVER_REGISTERING,
  AVAHI_SERVER_RUNNING,
  AVAHI_SERVER_COLLISION,
  AVAHI_SERVER_FAILURE
} AvahiServerState;

typedef struct AvahiClient AvahiClient;

typedef enum {
  AVAHI_CLIENT_S_REGISTERING = AVAHI_SERVER_REGISTERING,
  AVAHI_CLIENT_S_RUNNING = AVAHI_SERVER_RUNNING,
  AVAHI_CLIENT_S_COLLISION = AVAHI_SERVER_COLLISION,
  AVAHI_CLIENT_FAILURE = 100,
  AVAHI_CLIENT_CONNECTING = 101
} AvahiClientState;

typedef enum {
  AVAHI_CLIENT_IGNORE_USER_CONFIG = 1,
  AVAHI_CLIENT_NO_FAIL = 2
} AvahiClientFlags;

typedef void (*AvahiClientCallback)(AvahiClient* s, AvahiClientState state,
                                    void* userdata);

typedef struct AvahiServiceBrowser AvahiServiceBrowser;

typedef int AvahiProtocol;

typedef int AvahiIfIndex;

typedef enum {
  AVAHI_BROWSER_NEW,
  AVAHI_BROWSER_REMOVE,
  AVAHI_BROWSER_CACHE_EXHAUSTED,
  AVAHI_BROWSER_ALL_FOR_NOW,
  AVAHI_BROWSER_FAILURE
} AvahiBrowserEvent;

typedef enum {
  AVAHI_LOOKUP_RESULT_CACHED = 1,
  AVAHI_LOOKUP_RESULT_WIDE_AREA = 2,
  AVAHI_LOOKUP_RESULT_MULTICAST = 4,
  AVAHI_LOOKUP_RESULT_LOCAL = 8,
  AVAHI_LOOKUP_RESULT_OUR_OWN = 16,
  AVAHI_LOOKUP_RESULT_STATIC = 32
} AvahiLookupResultFlags;

typedef void (*AvahiServiceBrowserCallback)(
    AvahiServiceBrowser* b, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiBrowserEvent event, const char* name, const char* type,
    const char* domain, AvahiLookupResultFlags flags, void* userdata);

typedef enum {
  AVAHI_LOOKUP_USE_WIDE_AREA = 1,
  AVAHI_LOOKUP_USE_MULTICAST = 2,

  AVAHI_LOOKUP_NO_TXT = 4,
  AVAHI_LOOKUP_NO_ADDRESS = 8
} AvahiLookupFlags;

typedef struct AvahiServiceResolver AvahiServiceResolver;

typedef enum {
  AVAHI_RESOLVER_FOUND,
  AVAHI_RESOLVER_FAILURE
} AvahiResolverEvent;

typedef struct AvahiIPv4Address {
  uint32_t address;
} AvahiIPv4Address;

typedef struct AvahiIPv6Address {
  uint8_t address[16];
} AvahiIPv6Address;

typedef struct AvahiAddress {
  AvahiProtocol proto;

  union {
    AvahiIPv6Address ipv6;
    AvahiIPv4Address ipv4;
    uint8_t data[1];
  } data;
} AvahiAddress;

typedef struct AvahiStringList {
  struct AvahiStringList* next;
  size_t size;
  uint8_t text[1];
} AvahiStringList;

typedef void (*AvahiServiceResolverCallback)(
    AvahiServiceResolver* r, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiResolverEvent event, const char* name, const char* type,
    const char* domain, const char* host_name, const AvahiAddress* a,
    uint16_t port, AvahiStringList* txt, AvahiLookupResultFlags flags,
    void* userdata);

typedef struct AvahiThreadedPoll AvahiThreadedPoll;

typedef struct AvahiEntryGroup AvahiEntryGroup;

typedef enum {
  AVAHI_ENTRY_GROUP_UNCOMMITED,
  AVAHI_ENTRY_GROUP_REGISTERING,
  AVAHI_ENTRY_GROUP_ESTABLISHED,
  AVAHI_ENTRY_GROUP_COLLISION,
  AVAHI_ENTRY_GROUP_FAILURE
} AvahiEntryGroupState;

typedef void (*AvahiEntryGroupCallback)(AvahiEntryGroup* g,
                                        AvahiEntryGroupState state,
                                        void* userdata);

typedef enum {
  AVAHI_PUBLISH_UNIQUE = 1,
  AVAHI_PUBLISH_NO_PROBE = 2,
  AVAHI_PUBLISH_NO_ANNOUNCE = 4,
  AVAHI_PUBLISH_ALLOW_MULTIPLE = 8,

  AVAHI_PUBLISH_NO_REVERSE = 16,
  AVAHI_PUBLISH_NO_COOKIE = 32,
  AVAHI_PUBLISH_UPDATE = 64,
  AVAHI_PUBLISH_USE_WIDE_AREA = 128,
  AVAHI_PUBLISH_USE_MULTICAST = 256
} AvahiPublishFlags;

enum { AVAHI_IF_UNSPEC = -1 };

enum { AVAHI_PROTO_INET = 0, AVAHI_PROTO_INET6 = 1, AVAHI_PROTO_UNSPEC = -1 };

enum {
  AVAHI_OK = 0,
  AVAHI_ERR_FAILURE = -1,
  AVAHI_ERR_BAD_STATE = -2,
  AVAHI_ERR_INVALID_HOST_NAME = -3,
  AVAHI_ERR_INVALID_DOMAIN_NAME = -4,
  AVAHI_ERR_NO_NETWORK = -5,
  AVAHI_ERR_INVALID_TTL = -6,
  AVAHI_ERR_IS_PATTERN = -7,
  AVAHI_ERR_COLLISION = -8,
  AVAHI_ERR_INVALID_RECORD = -9,

  AVAHI_ERR_INVALID_SERVICE_NAME = -10,
  AVAHI_ERR_INVALID_SERVICE_TYPE = -11,
  AVAHI_ERR_INVALID_PORT = -12,
  AVAHI_ERR_INVALID_KEY = -13,
  AVAHI_ERR_INVALID_ADDRESS = -14,
  AVAHI_ERR_TIMEOUT = -15,
  AVAHI_ERR_TOO_MANY_CLIENTS = -16,
  AVAHI_ERR_TOO_MANY_OBJECTS = -17,
  AVAHI_ERR_TOO_MANY_ENTRIES = -18,
  AVAHI_ERR_OS = -19,

  AVAHI_ERR_ACCESS_DENIED = -20,
  AVAHI_ERR_INVALID_OPERATION = -21,
  AVAHI_ERR_DBUS_ERROR = -22,
  AVAHI_ERR_DISCONNECTED = -23,
  AVAHI_ERR_NO_MEMORY = -24,
  AVAHI_ERR_INVALID_OBJECT = -25,
  AVAHI_ERR_NO_DAEMON = -26,
  AVAHI_ERR_INVALID_INTERFACE = -27,
  AVAHI_ERR_INVALID_PROTOCOL = -28,
  AVAHI_ERR_INVALID_FLAGS = -29,

  AVAHI_ERR_NOT_FOUND = -30,
  AVAHI_ERR_INVALID_CONFIG = -31,
  AVAHI_ERR_VERSION_MISMATCH = -32,
  AVAHI_ERR_INVALID_SERVICE_SUBTYPE = -33,
  AVAHI_ERR_INVALID_PACKET = -34,
  AVAHI_ERR_INVALID_DNS_ERROR = -35,
  AVAHI_ERR_DNS_FORMERR = -36,
  AVAHI_ERR_DNS_SERVFAIL = -37,
  AVAHI_ERR_DNS_NXDOMAIN = -38,
  AVAHI_ERR_DNS_NOTIMP = -39,

  AVAHI_ERR_DNS_REFUSED = -40,
  AVAHI_ERR_DNS_YXDOMAIN = -41,
  AVAHI_ERR_DNS_YXRRSET = -42,
  AVAHI_ERR_DNS_NXRRSET = -43,
  AVAHI_ERR_DNS_NOTAUTH = -44,
  AVAHI_ERR_DNS_NOTZONE = -45,
  AVAHI_ERR_INVALID_RDATA = -46,
  AVAHI_ERR_INVALID_DNS_CLASS = -47,
  AVAHI_ERR_INVALID_DNS_TYPE = -48,
  AVAHI_ERR_NOT_SUPPORTED = -49,

  AVAHI_ERR_NOT_PERMITTED = -50,
  AVAHI_ERR_INVALID_ARGUMENT = -51,
  AVAHI_ERR_IS_EMPTY = -52,
  AVAHI_ERR_NO_CHANGE = -53,

  AVAHI_ERR_MAX = -54
};

namespace wpi {
class AvahiFunctionTable {
 public:
#define AvahiFunction(CapName, RetType, Parameters) \
  using CapName##_func = RetType(*) Parameters;     \
  CapName##_func CapName = nullptr

  AvahiFunction(threaded_poll_new, AvahiThreadedPoll*, (void));
  AvahiFunction(threaded_poll_free, void, (AvahiThreadedPoll*));
  AvahiFunction(threaded_poll_get, const AvahiPoll*, (AvahiThreadedPoll*));
  AvahiFunction(threaded_poll_start, int, (AvahiThreadedPoll*));
  AvahiFunction(threaded_poll_stop, int, (AvahiThreadedPoll*));
  AvahiFunction(threaded_poll_lock, int, (AvahiThreadedPoll*));
  AvahiFunction(threaded_poll_unlock, int, (AvahiThreadedPoll*));

  AvahiFunction(client_new, AvahiClient*,
                (const AvahiPoll* poll_api, AvahiClientFlags flags,
                 AvahiClientCallback callback, void* userdata, int* error));
  AvahiFunction(client_free, void, (AvahiClient*));

  AvahiFunction(service_browser_new, AvahiServiceBrowser*,
                (AvahiClient * client, AvahiIfIndex interface,
                 AvahiProtocol protocol, const char* type, const char* domain,
                 AvahiLookupFlags flags, AvahiServiceBrowserCallback callback,
                 void* userdata));

  AvahiFunction(service_browser_free, int, (AvahiServiceBrowser*));

  AvahiFunction(service_resolver_new, AvahiServiceResolver*,
                (AvahiClient * client, AvahiIfIndex interface,
                 AvahiProtocol protocol, const char* name, const char* type,
                 const char* domain, AvahiProtocol aprotocol,
                 AvahiLookupFlags flags, AvahiServiceResolverCallback callback,
                 void* userdata));
  AvahiFunction(service_resolver_free, int, (AvahiServiceResolver*));

  AvahiFunction(entry_group_new, AvahiEntryGroup*,
                (AvahiClient*, AvahiEntryGroupCallback, void*));
  AvahiFunction(entry_group_free, int, (AvahiEntryGroup*));

  AvahiFunction(entry_group_add_service, int,
                (AvahiEntryGroup * group, AvahiIfIndex interface,
                 AvahiProtocol protocol, AvahiPublishFlags flags,
                 const char* name, const char* type, const char* domain,
                 const char* host, uint16_t port, ...));

  AvahiFunction(entry_group_add_service_strlst, int,
                (AvahiEntryGroup * group, AvahiIfIndex interface,
                 AvahiProtocol protocol, AvahiPublishFlags flags,
                 const char* name, const char* type, const char* domain,
                 const char* host, uint16_t port, AvahiStringList*));

  AvahiFunction(entry_group_reset, int, (AvahiEntryGroup*));
  AvahiFunction(entry_group_is_empty, int, (AvahiEntryGroup*));
  AvahiFunction(entry_group_commit, int, (AvahiEntryGroup*));
  AvahiFunction(entry_group_get_client, AvahiClient*, (AvahiEntryGroup*));

  AvahiFunction(string_list_new_from_array, AvahiStringList*,
                (const char** array, int len));
  AvahiFunction(string_list_free, void, (AvahiStringList*));

  AvahiFunction(service_browser_get_client, AvahiClient*,
                (AvahiServiceBrowser*));

  AvahiFunction(unescape_label, char*, (const char**, char*, size_t));
  AvahiFunction(alternative_service_name, char*, (const char*));
  AvahiFunction(free, void, (void*));

  bool IsValid() const { return valid; }

  static AvahiFunctionTable& Get();

 private:
  AvahiFunctionTable();
  bool valid;
};

class AvahiThread {
 private:
  struct private_init {};

 public:
  explicit AvahiThread(const private_init&);
  ~AvahiThread() noexcept;

  void lock();
  const AvahiPoll* GetPoll() const;
  void unlock();

  static std::shared_ptr<AvahiThread> Get();

 private:
  AvahiThreadedPoll* threadedPoll;
  AvahiFunctionTable& table = AvahiFunctionTable::Get();
};

}  // namespace wpi
