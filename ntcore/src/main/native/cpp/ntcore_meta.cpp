// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/MessagePack.h>
#include <wpi/mpack.h>

#include "ntcore_cpp.h"

using namespace mpack;

using namespace nt::meta;

static SubscriberOptions DecodeSubscriberOptions(mpack_reader_t& r) {
  SubscriberOptions options;
  uint32_t numMapElem = mpack_expect_map(&r);
  for (uint32_t j = 0; j < numMapElem; ++j) {
    std::string key;
    mpack_expect_str(&r, &key);
    if (key == "topicsonly") {
      options.topicsOnly = mpack_expect_bool(&r);
    } else if (key == "all") {
      options.sendAll = mpack_expect_bool(&r);
    } else if (key == "periodic") {
      options.periodic = mpack_expect_float(&r);
    } else if (key == "prefix") {
      options.prefixMatch = mpack_expect_bool(&r);
    } else {
      // TODO: Save other options
      mpack_discard(&r);
    }
  }
  mpack_done_map(&r);
  return options;
}

std::optional<std::vector<ClientPublisher>> nt::meta::DecodeClientPublishers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numPub = mpack_expect_array_max(&r, 10000);
  std::vector<ClientPublisher> publishers;
  publishers.reserve(numPub);
  for (uint32_t i = 0; i < numPub; ++i) {
    ClientPublisher pub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "uid") {
        pub.uid = mpack_expect_i64(&r);
      } else if (key == "topic") {
        mpack_expect_str(&r, &pub.topic);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    publishers.emplace_back(std::move(pub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    return {std::move(publishers)};
  } else {
    return {};
  }
}

std::optional<std::vector<ClientSubscriber>> nt::meta::DecodeClientSubscribers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numSub = mpack_expect_array_max(&r, 10000);
  std::vector<ClientSubscriber> subscribers;
  subscribers.reserve(numSub);
  for (uint32_t i = 0; i < numSub; ++i) {
    ClientSubscriber sub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "uid") {
        sub.uid = mpack_expect_i64(&r);
      } else if (key == "topics") {
        uint32_t numPrefix = mpack_expect_array_max(&r, 100);
        sub.topics.reserve(numPrefix);
        for (uint32_t k = 0; k < numPrefix; ++k) {
          std::string val;
          if (mpack_expect_str(&r, &val) == mpack_ok) {
            sub.topics.emplace_back(std::move(val));
          }
        }
        mpack_done_array(&r);
      } else if (key == "options") {
        sub.options = DecodeSubscriberOptions(r);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    subscribers.emplace_back(std::move(sub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    return {std::move(subscribers)};
  } else {
    return {};
  }
}

std::optional<std::vector<TopicPublisher>> nt::meta::DecodeTopicPublishers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numPub = mpack_expect_array_max(&r, 1000);
  std::vector<TopicPublisher> publishers;
  publishers.reserve(numPub);
  for (uint32_t i = 0; i < numPub; ++i) {
    TopicPublisher pub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "pubuid") {
        pub.pubuid = mpack_expect_i64(&r);
      } else if (key == "client") {
        mpack_expect_str(&r, &pub.client);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    publishers.emplace_back(std::move(pub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    return {std::move(publishers)};
  } else {
    return {};
  }
}

std::optional<std::vector<TopicSubscriber>> nt::meta::DecodeTopicSubscribers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numSub = mpack_expect_array_max(&r, 1000);
  std::vector<TopicSubscriber> subscribers;
  subscribers.reserve(numSub);
  for (uint32_t i = 0; i < numSub; ++i) {
    TopicSubscriber sub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "subuid") {
        sub.subuid = mpack_expect_i64(&r);
      } else if (key == "client") {
        mpack_expect_str(&r, &sub.client);
      } else if (key == "options") {
        sub.options = DecodeSubscriberOptions(r);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    subscribers.emplace_back(std::move(sub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    return {std::move(subscribers)};
  } else {
    return {};
  }
}

std::optional<std::vector<Client>> nt::meta::DecodeClients(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numClients = mpack_expect_array_max(&r, 100);
  std::vector<Client> clients;
  clients.reserve(numClients);
  for (uint32_t i = 0; i < numClients; ++i) {
    Client client;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "id") {
        mpack_expect_str(&r, &client.id);
      } else if (key == "conn") {
        mpack_expect_str(&r, &client.conn);
      } else if (key == "ver") {
        client.version = mpack_expect_u16(&r);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    clients.emplace_back(std::move(client));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    return {std::move(clients)};
  } else {
    return {};
  }
}
