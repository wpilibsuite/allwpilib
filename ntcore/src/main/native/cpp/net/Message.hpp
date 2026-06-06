// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <new>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "PubSubOptions.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/json.hpp"

namespace wpi::nt::net {

#if __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

struct PublishMsg {
  static constexpr std::string_view kMethodStr = "publish";
  int pubuid{0};
  std::string name;
  std::string typeStr;
  wpi::util::json properties;
  PubSubOptionsImpl options;  // will be empty when coming from network
};

struct UnpublishMsg {
  static constexpr std::string_view kMethodStr = "unpublish";
  int pubuid{0};
};

struct SetPropertiesMsg {
  static constexpr std::string_view kMethodStr = "setproperties";
  std::string name;
  wpi::util::json update;
};

struct SubscribeMsg {
  static constexpr std::string_view kMethodStr = "subscribe";
  int subuid{0};
  std::vector<std::string> topicNames;
  PubSubOptionsImpl options;
};

struct UnsubscribeMsg {
  static constexpr std::string_view kMethodStr = "unsubscribe";
  int subuid{0};
};

struct ClientValueMsg {
  int pubuid{0};
  Value value;
};

#if __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif

class ClientMessage {
 public:
  using ValueMsg = ClientValueMsg;

  enum class Type {
    EMPTY,
    PUBLISH,
    UNPUBLISH,
    SET_PROPERTIES,
    SUBSCRIBE,
    UNSUBSCRIBE,
    VALUE
  };

  ClientMessage() = default;
  ClientMessage(const ClientMessage& other) = delete;
  ClientMessage(ClientMessage&& other) { MoveFrom(std::move(other)); }
  ClientMessage(PublishMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ClientMessage(UnpublishMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ClientMessage(SetPropertiesMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ClientMessage(SubscribeMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ClientMessage(UnsubscribeMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ClientMessage(ClientValueMsg msg) {  // NOLINT(implicit)
    Construct(std::move(msg));
  }
  ~ClientMessage() { Clear(); }

  ClientMessage& operator=(const ClientMessage& other) = delete;

  ClientMessage& operator=(ClientMessage&& other) {
    if (this != &other) {
      Clear();
      MoveFrom(std::move(other));
    }
    return *this;
  }

  Type type() const { return m_type; }

  const PublishMsg* GetPublish() const {
    return m_type == Type::PUBLISH ? &m_storage.publish : nullptr;
  }

  const UnpublishMsg* GetUnpublish() const {
    return m_type == Type::UNPUBLISH ? &m_storage.unpublish : nullptr;
  }

  const SetPropertiesMsg* GetSetProperties() const {
    return m_type == Type::SET_PROPERTIES ? &m_storage.setProperties : nullptr;
  }

  const SubscribeMsg* GetSubscribe() const {
    return m_type == Type::SUBSCRIBE ? &m_storage.subscribe : nullptr;
  }

  const UnsubscribeMsg* GetUnsubscribe() const {
    return m_type == Type::UNSUBSCRIBE ? &m_storage.unsubscribe : nullptr;
  }

  ClientValueMsg* GetValue() {
    return m_type == Type::VALUE ? &m_storage.value : nullptr;
  }
  const ClientValueMsg* GetValue() const {
    return m_type == Type::VALUE ? &m_storage.value : nullptr;
  }

 private:
  void Construct(PublishMsg&& contents) {
    new (&m_storage.publish) PublishMsg{std::move(contents)};
    m_type = Type::PUBLISH;
  }

  void Construct(UnpublishMsg&& contents) {
    new (&m_storage.unpublish) UnpublishMsg{std::move(contents)};
    m_type = Type::UNPUBLISH;
  }

  void Construct(SetPropertiesMsg&& contents) {
    new (&m_storage.setProperties) SetPropertiesMsg{std::move(contents)};
    m_type = Type::SET_PROPERTIES;
  }

  void Construct(SubscribeMsg&& contents) {
    new (&m_storage.subscribe) SubscribeMsg{std::move(contents)};
    m_type = Type::SUBSCRIBE;
  }

  void Construct(UnsubscribeMsg&& contents) {
    new (&m_storage.unsubscribe) UnsubscribeMsg{std::move(contents)};
    m_type = Type::UNSUBSCRIBE;
  }

  void Construct(ClientValueMsg&& contents) {
    new (&m_storage.value) ClientValueMsg{std::move(contents)};
    m_type = Type::VALUE;
  }

  void Clear() {
    switch (m_type) {
      case Type::EMPTY:
        break;
      case Type::PUBLISH:
        m_storage.publish.~PublishMsg();
        break;
      case Type::UNPUBLISH:
        m_storage.unpublish.~UnpublishMsg();
        break;
      case Type::SET_PROPERTIES:
        m_storage.setProperties.~SetPropertiesMsg();
        break;
      case Type::SUBSCRIBE:
        m_storage.subscribe.~SubscribeMsg();
        break;
      case Type::UNSUBSCRIBE:
        m_storage.unsubscribe.~UnsubscribeMsg();
        break;
      case Type::VALUE:
        m_storage.value.~ClientValueMsg();
        break;
    }
    m_type = Type::EMPTY;
  }

  void MoveFrom(ClientMessage&& other) {
    switch (other.m_type) {
      case Type::EMPTY:
        break;
      case Type::PUBLISH:
        Construct(std::move(other.m_storage.publish));
        break;
      case Type::UNPUBLISH:
        Construct(std::move(other.m_storage.unpublish));
        break;
      case Type::SET_PROPERTIES:
        Construct(std::move(other.m_storage.setProperties));
        break;
      case Type::SUBSCRIBE:
        Construct(std::move(other.m_storage.subscribe));
        break;
      case Type::UNSUBSCRIBE:
        Construct(std::move(other.m_storage.unsubscribe));
        break;
      case Type::VALUE:
        Construct(std::move(other.m_storage.value));
        break;
    }
  }

  Type m_type{Type::EMPTY};
  union Storage {
    Storage() {}
    ~Storage() {}

    PublishMsg publish;
    UnpublishMsg unpublish;
    SetPropertiesMsg setProperties;
    SubscribeMsg subscribe;
    UnsubscribeMsg unsubscribe;
    ClientValueMsg value;
  } m_storage;
};

struct AnnounceMsg {
  static constexpr std::string_view kMethodStr = "announce";
  std::string name;
  int id{0};
  std::string typeStr;
  std::optional<int> pubuid;
  wpi::util::json properties;
};

struct UnannounceMsg {
  static constexpr std::string_view kMethodStr = "unannounce";
  std::string name;
  int id{0};
};

struct PropertiesUpdateMsg {
  static constexpr std::string_view kMethodStr = "properties";
  std::string name;
  wpi::util::json update;
  bool ack;
};

struct ServerValueMsg {
  int topic{0};
  Value value;
};

class ServerMessage {
 public:
  using ValueMsg = ServerValueMsg;

  enum class Type { EMPTY, ANNOUNCE, UNANNOUNCE, PROPERTIES_UPDATE, VALUE };

  ServerMessage() = default;
  ServerMessage(const ServerMessage& other) = delete;
  ServerMessage(ServerMessage&& other) { MoveFrom(std::move(other)); }
  ServerMessage(AnnounceMsg msg) { Construct(std::move(msg)); }
  ServerMessage(UnannounceMsg msg) { Construct(std::move(msg)); }
  ServerMessage(PropertiesUpdateMsg msg) { Construct(std::move(msg)); }
  ServerMessage(ServerValueMsg msg) { Construct(std::move(msg)); }
  ~ServerMessage() { Clear(); }

  ServerMessage& operator=(const ServerMessage& other) = delete;

  ServerMessage& operator=(ServerMessage&& other) {
    if (this != &other) {
      Clear();
      MoveFrom(std::move(other));
    }
    return *this;
  }

  Type type() const { return m_type; }

  const AnnounceMsg* GetAnnounce() const {
    return m_type == Type::ANNOUNCE ? &m_storage.announce : nullptr;
  }

  const UnannounceMsg* GetUnannounce() const {
    return m_type == Type::UNANNOUNCE ? &m_storage.unannounce : nullptr;
  }

  const PropertiesUpdateMsg* GetPropertiesUpdate() const {
    return m_type == Type::PROPERTIES_UPDATE ? &m_storage.propertiesUpdate
                                             : nullptr;
  }

  ServerValueMsg* GetValue() {
    return m_type == Type::VALUE ? &m_storage.value : nullptr;
  }
  const ServerValueMsg* GetValue() const {
    return m_type == Type::VALUE ? &m_storage.value : nullptr;
  }

 private:
  void Construct(AnnounceMsg&& contents) {
    new (&m_storage.announce) AnnounceMsg{std::move(contents)};
    m_type = Type::ANNOUNCE;
  }

  void Construct(UnannounceMsg&& contents) {
    new (&m_storage.unannounce) UnannounceMsg{std::move(contents)};
    m_type = Type::UNANNOUNCE;
  }

  void Construct(PropertiesUpdateMsg&& contents) {
    new (&m_storage.propertiesUpdate) PropertiesUpdateMsg{std::move(contents)};
    m_type = Type::PROPERTIES_UPDATE;
  }

  void Construct(ServerValueMsg&& contents) {
    new (&m_storage.value) ServerValueMsg{std::move(contents)};
    m_type = Type::VALUE;
  }

  void Clear() {
    switch (m_type) {
      case Type::EMPTY:
        break;
      case Type::ANNOUNCE:
        m_storage.announce.~AnnounceMsg();
        break;
      case Type::UNANNOUNCE:
        m_storage.unannounce.~UnannounceMsg();
        break;
      case Type::PROPERTIES_UPDATE:
        m_storage.propertiesUpdate.~PropertiesUpdateMsg();
        break;
      case Type::VALUE:
        m_storage.value.~ServerValueMsg();
        break;
    }
    m_type = Type::EMPTY;
  }

  void MoveFrom(ServerMessage&& other) {
    switch (other.m_type) {
      case Type::EMPTY:
        break;
      case Type::ANNOUNCE:
        Construct(std::move(other.m_storage.announce));
        break;
      case Type::UNANNOUNCE:
        Construct(std::move(other.m_storage.unannounce));
        break;
      case Type::PROPERTIES_UPDATE:
        Construct(std::move(other.m_storage.propertiesUpdate));
        break;
      case Type::VALUE:
        Construct(std::move(other.m_storage.value));
        break;
    }
  }

  Type m_type{Type::EMPTY};
  union Storage {
    Storage() {}
    ~Storage() {}

    AnnounceMsg announce;
    UnannounceMsg unannounce;
    PropertiesUpdateMsg propertiesUpdate;
    ServerValueMsg value;
  } m_storage;
};

}  // namespace wpi::nt::net
