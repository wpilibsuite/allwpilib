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

/** An event polling abstraction object */
typedef struct AvahiPoll AvahiPoll;

/** States of a server object */
typedef enum {
  AVAHI_SERVER_INVALID,     /**< Invalid state (initial) */
  AVAHI_SERVER_REGISTERING, /**< Host RRs are being registered */
  AVAHI_SERVER_RUNNING,     /**< All host RRs have been established */
  AVAHI_SERVER_COLLISION, /**< There is a collision with a host RR. All host RRs
                             have been withdrawn, the user should set a new host
                             name via avahi_server_set_host_name() */
  AVAHI_SERVER_FAILURE /**< Some fatal failure happened, the server is unable to
                          proceed */
} AvahiServerState;

/** A connection context */
typedef struct AvahiClient AvahiClient;

/** States of a client object, a superset of AvahiServerState */
typedef enum {
  AVAHI_CLIENT_S_REGISTERING =
      AVAHI_SERVER_REGISTERING, /**< Server state: REGISTERING */
  AVAHI_CLIENT_S_RUNNING = AVAHI_SERVER_RUNNING, /**< Server state: RUNNING */
  AVAHI_CLIENT_S_COLLISION =
      AVAHI_SERVER_COLLISION, /**< Server state: COLLISION */
  AVAHI_CLIENT_FAILURE =
      100, /**< Some kind of error happened on the client side */
  AVAHI_CLIENT_CONNECTING =
      101 /**< We're still connecting. This state is only entered when
             AVAHI_CLIENT_NO_FAIL has been passed to avahi_client_new() and the
             daemon is not yet available. */
} AvahiClientState;

typedef enum {
  AVAHI_CLIENT_IGNORE_USER_CONFIG = 1, /**< Don't read user configuration */
  AVAHI_CLIENT_NO_FAIL =
      2 /**< Don't fail if the daemon is not available when avahi_client_new()
           is called, instead enter AVAHI_CLIENT_CONNECTING state and wait for
           the daemon to appear */
} AvahiClientFlags;

/** The function prototype for the callback of an AvahiClient */
typedef void (*AvahiClientCallback)(
    AvahiClient* s, AvahiClientState state /**< The new state of the client */,
    void* userdata /**< The user data that was passed to avahi_client_new() */);

typedef struct AvahiServiceBrowser AvahiServiceBrowser;

/** Protocol family specification, takes the values AVAHI_PROTO_INET,
 * AVAHI_PROTO_INET6, AVAHI_PROTO_UNSPEC */
typedef int AvahiProtocol;

/** Numeric network interface index. Takes OS dependent values and the special
 * constant AVAHI_IF_UNSPEC  */
typedef int AvahiIfIndex;

/** Type of callback event when browsing */
typedef enum {
  AVAHI_BROWSER_NEW,    /**< The object is new on the network */
  AVAHI_BROWSER_REMOVE, /**< The object has been removed from the network */
  AVAHI_BROWSER_CACHE_EXHAUSTED, /**< One-time event, to notify the user that
                                    all entries from the caches have been sent
                                  */
  AVAHI_BROWSER_ALL_FOR_NOW, /**< One-time event, to notify the user that more
                                records will probably not show up in the near
                                future, i.e. all cache entries have been read
                                and all static servers been queried */
  AVAHI_BROWSER_FAILURE /**< Browsing failed due to some reason which can be
                           retrieved using
                           avahi_server_errno()/avahi_client_errno() */
} AvahiBrowserEvent;

/** Some flags for lookup callback functions */
typedef enum {
  AVAHI_LOOKUP_RESULT_CACHED =
      1, /**< This response originates from the cache */
  AVAHI_LOOKUP_RESULT_WIDE_AREA =
      2, /**< This response originates from wide area DNS */
  AVAHI_LOOKUP_RESULT_MULTICAST =
      4, /**< This response originates from multicast DNS */
  AVAHI_LOOKUP_RESULT_LOCAL =
      8, /**< This record/service resides on and was announced by the local
            host. Only available in service and record browsers and only on
            AVAHI_BROWSER_NEW. */
  AVAHI_LOOKUP_RESULT_OUR_OWN =
      16, /**< This service belongs to the same local client as the browser
             object. Only available in avahi-client, and only for service
             browsers and only on AVAHI_BROWSER_NEW. */
  AVAHI_LOOKUP_RESULT_STATIC = 32 /**< The returned data has been defined
                                     statically by some configuration option */
} AvahiLookupResultFlags;

/** The function prototype for the callback of an AvahiServiceBrowser */
typedef void (*AvahiServiceBrowserCallback)(
    AvahiServiceBrowser* b, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiBrowserEvent event, const char* name, const char* type,
    const char* domain, AvahiLookupResultFlags flags, void* userdata);

/** Some flags for lookup functions */
typedef enum {
  /** \cond fulldocs */
  AVAHI_LOOKUP_USE_WIDE_AREA = 1, /**< Force lookup via wide area DNS */
  AVAHI_LOOKUP_USE_MULTICAST = 2, /**< Force lookup via multicast DNS */
                                  /** \endcond */
  AVAHI_LOOKUP_NO_TXT =
      4, /**< When doing service resolving, don't lookup TXT record */
  AVAHI_LOOKUP_NO_ADDRESS =
      8 /**< When doing service resolving, don't lookup A/AAAA record */
} AvahiLookupFlags;

/** A service resolver object */
typedef struct AvahiServiceResolver AvahiServiceResolver;

/** Type of callback event when resolving */
typedef enum {
  AVAHI_RESOLVER_FOUND,  /**< RR found, resolving successful */
  AVAHI_RESOLVER_FAILURE /**< Resolving failed due to some reason which can be
                            retrieved using
                            avahi_server_errno()/avahi_client_errno() */
} AvahiResolverEvent;

/** An IPv4 address */
typedef struct AvahiIPv4Address {
  uint32_t address; /**< Address data in network byte order. */
} AvahiIPv4Address;

/** An IPv6 address */
typedef struct AvahiIPv6Address {
  uint8_t address[16]; /**< Address data */
} AvahiIPv6Address;

/** Protocol (address family) independent address structure */
typedef struct AvahiAddress {
  AvahiProtocol proto; /**< Address family */

  union {
    AvahiIPv6Address ipv6; /**< Address when IPv6 */
    AvahiIPv4Address ipv4; /**< Address when IPv4 */
    uint8_t data[1];       /**< Type-independent data field */
  } data;
} AvahiAddress;

/** Linked list of strings that can contain any number of binary
 * characters, including NUL bytes. An empty list is created by
 * assigning a NULL to a pointer to AvahiStringList. The string list
 * is stored in reverse order, so that appending to the string list is
 * effectively a prepending to the linked list.  This object is used
 * primarily for storing DNS TXT record data. */
typedef struct AvahiStringList {
  struct AvahiStringList* next; /**< Pointer to the next linked list element */
  size_t size;                  /**< Size of text[] */
  uint8_t text[1];              /**< Character data */
} AvahiStringList;

/** The function prototype for the callback of an AvahiServiceResolver */
typedef void (*AvahiServiceResolverCallback)(
    AvahiServiceResolver* r, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiResolverEvent event, const char* name, const char* type,
    const char* domain, const char* host_name, const AvahiAddress* a,
    uint16_t port, AvahiStringList* txt, AvahiLookupResultFlags flags,
    void* userdata);

typedef struct AvahiThreadedPoll AvahiThreadedPoll;

typedef struct AvahiEntryGroup AvahiEntryGroup;

/** States of an entry group object */
typedef enum {
  AVAHI_ENTRY_GROUP_UNCOMMITED,  /**< The group has not yet been commited, the
                                    user must still call
                                    avahi_entry_group_commit() */
  AVAHI_ENTRY_GROUP_REGISTERING, /**< The entries of the group are currently
                                    being registered */
  AVAHI_ENTRY_GROUP_ESTABLISHED, /**< The entries have successfully been
                                    established */
  AVAHI_ENTRY_GROUP_COLLISION,   /**< A name collision for one of the entries in
                                    the group has been detected, the entries have
                                    been withdrawn */
  AVAHI_ENTRY_GROUP_FAILURE /**< Some kind of failure happened, the entries have
                               been withdrawn */
} AvahiEntryGroupState;

/** The function prototype for the callback of an AvahiEntryGroup */
typedef void (*AvahiEntryGroupCallback)(
    AvahiEntryGroup* g,
    AvahiEntryGroupState state /**< The new state of the entry group */, void* userdata /* The arbitrary user data pointer originally passed to avahi_entry_group_new()*/);

/** Some flags for publishing functions */
typedef enum {
  AVAHI_PUBLISH_ALL = 0,
  AVAHI_PUBLISH_UNIQUE =
      1, /**< For raw records: The RRset is intended to be unique */
  AVAHI_PUBLISH_NO_PROBE = 2, /**< For raw records: Though the RRset is intended
                                 to be unique no probes shall be sent */
  AVAHI_PUBLISH_NO_ANNOUNCE =
      4, /**< For raw records: Do not announce this RR to other hosts */
  AVAHI_PUBLISH_ALLOW_MULTIPLE =
      8, /**< For raw records: Allow multiple local records of this type, even
            if they are intended to be unique */
         /** \cond fulldocs */
  AVAHI_PUBLISH_NO_REVERSE =
      16, /**< For address records: don't create a reverse (PTR) entry */
  AVAHI_PUBLISH_NO_COOKIE = 32, /**< For service records: do not implicitly add
                                   the local service cookie to TXT data */
                                /** \endcond */
  AVAHI_PUBLISH_UPDATE =
      64, /**< Update existing records instead of adding new ones */
          /** \cond fulldocs */
  AVAHI_PUBLISH_USE_WIDE_AREA = 128, /**< Register the record using wide area
                                        DNS (i.e. unicast DNS update) */
  AVAHI_PUBLISH_USE_MULTICAST =
      256 /**< Register the record using multicast DNS */
  /** \endcond */
} AvahiPublishFlags;

/** Special values for AvahiIfIndex */
enum {
  AVAHI_IF_UNSPEC = -1 /**< Unspecified/all interface(s) */
};

/** Values for AvahiProtocol */
enum {
  AVAHI_PROTO_INET = 0,   /**< IPv4 */
  AVAHI_PROTO_INET6 = 1,  /**< IPv6 */
  AVAHI_PROTO_UNSPEC = -1 /**< Unspecified/all protocol(s) */
};

#define AvahiFunction(CapName, RetType, Parameters) \
  using CapName##_func = RetType(*) Parameters;     \
  CapName##_func CapName = nullptr

namespace wpi {
class AvahiFunctionTable {
 public:
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

  AvahiFunction(
      entry_group_add_service_strlst, int,
      (AvahiEntryGroup * group,
       AvahiIfIndex interface /**< The interface this service shall be announced
                                 on. We recommend to pass AVAHI_IF_UNSPEC here,
                                 to announce on all interfaces. */
       ,
       AvahiProtocol
           protocol /**< The protocol this service shall be announced with, i.e.
                       MDNS over IPV4 or MDNS over IPV6. We recommend to pass
                       AVAHI_PROTO_UNSPEC here, to announce this service on all
                       protocols the daemon supports. */
       ,
       AvahiPublishFlags flags /**< Usually 0, unless you know what you do */,
       const char* name /**< The name for the new service. Must be valid service
                           name. i.e. a string shorter than 63 characters and
                           valid UTF-8. May not be NULL. */
       ,
       const char* type /**< The service type for the new service, such as
                           _http._tcp. May not be NULL. */
       ,
       const char*
           domain /**< The domain to register this domain in. We recommend to
                     pass NULL here, to let the daemon decide */
       ,
       const char* host /**< The host this services is residing on. We recommend
                           to pass NULL here, the daemon will than automatically
                           insert the local host name in that case */
       ,
       uint16_t port /**< The IP port number of this service */,
       AvahiStringList*));

  AvahiFunction(entry_group_reset, int, (AvahiEntryGroup*));
  AvahiFunction(entry_group_is_empty, int, (AvahiEntryGroup*));
  AvahiFunction(entry_group_commit, int, (AvahiEntryGroup*));

  AvahiFunction(string_list_new_from_array, AvahiStringList*,
                (const char** array, int len));
  AvahiFunction(string_list_free, void, (AvahiStringList*));

  AvahiFunction(service_browser_get_client, AvahiClient*,
                (AvahiServiceBrowser*));

  AvahiFunction(unescape_label, char*, (const char**, char*, size_t));

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
  AvahiThread(const private_init&);
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
