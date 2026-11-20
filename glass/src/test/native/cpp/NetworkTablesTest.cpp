// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NetworkTables.hpp"

#include <stdint.h>

#include <cstddef>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/MessagePack.hpp"
#include "wpi/util/mpack.h"
#include "wpi/util/raw_ostream.hpp"

namespace {
struct TopicPublisherMetadata {
  std::string_view client;
  int64_t pubuid;
};

struct TopicSubscriberMetadata {
  std::string_view client;
  int64_t subuid;
  bool topicsOnly = false;
  bool sendAll = false;
  bool prefixMatch = false;
};

class MsgpackWriter : public mpack::mpack_writer_t {
 public:
  MsgpackWriter() {
    mpack::mpack_writer_init(this, m_buf, sizeof(m_buf));
    mpack::mpack_writer_set_context(this, &m_os);
    mpack::mpack_writer_set_flush(this, [](mpack::mpack_writer_t* writer,
                                           const char* buffer, size_t count) {
      static_cast<wpi::util::raw_ostream*>(writer->context)
          ->write(buffer, count);
    });
  }

  std::vector<uint8_t> Finish() {
    REQUIRE(mpack::mpack_writer_destroy(this) == mpack::mpack_ok);
    return std::move(m_bytes);
  }

 private:
  std::vector<uint8_t> m_bytes;
  wpi::util::raw_uvector_ostream m_os{m_bytes};
  char m_buf[128];
};

std::vector<uint8_t> MakeClientsMetadata(
    std::span<const std::string_view> clientIds) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, static_cast<uint32_t>(clientIds.size()));
  for (auto clientId : clientIds) {
    mpack::mpack_start_map(&writer, 3);
    mpack::mpack_write_str(&writer, "id");
    mpack::mpack_write_str(&writer, clientId);
    mpack::mpack_write_str(&writer, "conn");
    mpack::mpack_write_str(&writer, "unit-test");
    mpack::mpack_write_str(&writer, "ver");
    mpack::mpack_write_u16(&writer, 4);
    mpack::mpack_finish_map(&writer);
  }
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

std::vector<uint8_t> MakeSubscribersMetadata(
    std::span<const std::string_view> topics, bool prefixMatch,
    int64_t uid = 1) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, 1);
  mpack::mpack_start_map(&writer, 3);
  mpack::mpack_write_str(&writer, "uid");
  mpack::mpack_write_int(&writer, uid);
  mpack::mpack_write_str(&writer, "topics");
  mpack::mpack_start_array(&writer, static_cast<uint32_t>(topics.size()));
  for (auto topic : topics) {
    mpack::mpack_write_str(&writer, topic);
  }
  mpack::mpack_finish_array(&writer);
  mpack::mpack_write_str(&writer, "options");
  mpack::mpack_start_map(&writer, prefixMatch ? 1u : 0u);
  if (prefixMatch) {
    mpack::mpack_write_str(&writer, "prefix");
    mpack::mpack_write_bool(&writer, true);
  }
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

std::vector<uint8_t> MakeTopicPublishersMetadata(
    std::span<const TopicPublisherMetadata> publishers) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, static_cast<uint32_t>(publishers.size()));
  for (auto&& publisher : publishers) {
    mpack::mpack_start_map(&writer, 2);
    mpack::mpack_write_str(&writer, "client");
    mpack::mpack_write_str(&writer, publisher.client);
    mpack::mpack_write_str(&writer, "pubuid");
    mpack::mpack_write_int(&writer, publisher.pubuid);
    mpack::mpack_finish_map(&writer);
  }
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

std::vector<uint8_t> MakeTopicSubscribersMetadata(
    std::span<const TopicSubscriberMetadata> subscribers) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, static_cast<uint32_t>(subscribers.size()));
  for (auto&& subscriber : subscribers) {
    mpack::mpack_start_map(&writer, 3);
    mpack::mpack_write_str(&writer, "client");
    mpack::mpack_write_str(&writer, subscriber.client);
    mpack::mpack_write_str(&writer, "subuid");
    mpack::mpack_write_int(&writer, subscriber.subuid);
    mpack::mpack_write_str(&writer, "options");
    mpack::mpack_start_map(&writer, 3);
    mpack::mpack_write_str(&writer, "topicsonly");
    mpack::mpack_write_bool(&writer, subscriber.topicsOnly);
    mpack::mpack_write_str(&writer, "all");
    mpack::mpack_write_bool(&writer, subscriber.sendAll);
    mpack::mpack_write_str(&writer, "prefix");
    mpack::mpack_write_bool(&writer, subscriber.prefixMatch);
    mpack::mpack_finish_map(&writer);
    mpack::mpack_finish_map(&writer);
  }
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

NT_Publisher PublishMsgpack(wpi::nt::NetworkTableInstance inst,
                            std::string_view name,
                            const std::vector<uint8_t>& value) {
  auto publisher = wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), name),
                                    NT_RAW, "msgpack");
  REQUIRE(publisher != 0);
  REQUIRE(wpi::nt::SetRaw(publisher, value));
  return publisher;
}

}  // namespace

TEST_CASE("NetworkTablesModel TopicSubscribersUnavailableWithoutMetadata",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto* entry =
        model.AddEntry(wpi::nt::GetTopic(inst.GetHandle(), "/Prefix/Value"));
    REQUIRE(entry);
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    PublishMsgpack(inst, "$clients", MakeClientsMetadata({{"client"}}));
    PublishMsgpack(inst, "$clientsub$client",
                   MakeSubscribersMetadata({{"/Prefix"}}, true));
    PublishMsgpack(inst, "$serversub",
                   MakeSubscribersMetadata({{"", "$"}}, true));
    model.Update();

    REQUIRE(model.GetServer().subscribers.size() == 1);
    REQUIRE(model.GetClients().size() == 1);
    REQUIRE(model.GetClients().at("client").subscribers.size() == 1);
    // Matching subscriptions do not provide per-topic metadata, including
    // subscriber lists for the meta-topics themselves.
    for (auto* topicEntry : model.GetEntries()) {
      CHECK_FALSE(topicEntry->GetTopicSubscribers().has_value());
    }
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel UpdatesTopicPubSubMetadata", "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};

    constexpr std::string_view topic = "/MetadataTopic";
    auto publisher = wpi::nt::Publish(
        wpi::nt::GetTopic(inst.GetHandle(), topic), NT_DOUBLE, "double");
    REQUIRE(publisher != 0);
    model.Update();
    auto* entry = model.GetEntry(topic);
    REQUIRE(entry);
    CHECK_FALSE(entry->GetTopicPublishers().has_value());
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    TopicPublisherMetadata publishers[] = {{"client-a", -10}, {"client-b", 20}};
    TopicSubscriberMetadata subscribers[] = {
        {"client-c", -30, false, true, false},
        {"client-d", 40, true, false, true}};
    auto pubMetadata = PublishMsgpack(inst, "$pub$/MetadataTopic",
                                      MakeTopicPublishersMetadata(publishers));
    auto subMetadata = PublishMsgpack(
        inst, "$sub$/MetadataTopic", MakeTopicSubscribersMetadata(subscribers));
    model.Update();

    auto actualPublishers = std::as_const(*entry).GetTopicPublishers();
    REQUIRE(actualPublishers.has_value());
    REQUIRE(actualPublishers->size() == 2);
    CHECK((*actualPublishers)[0].client == "client-a");
    CHECK((*actualPublishers)[0].pubuid == -10);
    CHECK((*actualPublishers)[1].client == "client-b");
    CHECK((*actualPublishers)[1].pubuid == 20);
    auto actualSubscribers = std::as_const(*entry).GetTopicSubscribers();
    REQUIRE(actualSubscribers.has_value());
    REQUIRE(actualSubscribers->size() == 2);
    CHECK((*actualSubscribers)[0].client == "client-c");
    CHECK((*actualSubscribers)[0].subuid == -30);
    CHECK((*actualSubscribers)[0].options.sendAll);
    CHECK_FALSE((*actualSubscribers)[0].options.prefixMatch);
    CHECK((*actualSubscribers)[1].client == "client-d");
    CHECK((*actualSubscribers)[1].subuid == 40);
    CHECK((*actualSubscribers)[1].options.topicsOnly);
    CHECK((*actualSubscribers)[1].options.prefixMatch);

    REQUIRE(wpi::nt::SetRaw(pubMetadata, MakeTopicPublishersMetadata({})));
    REQUIRE(wpi::nt::SetRaw(subMetadata, MakeTopicSubscribersMetadata({})));
    model.Update();

    actualPublishers = entry->GetTopicPublishers();
    REQUIRE(actualPublishers.has_value());
    CHECK(actualPublishers->empty());
    actualSubscribers = entry->GetTopicSubscribers();
    REQUIRE(actualSubscribers.has_value());
    CHECK(actualSubscribers->empty());

    wpi::nt::Unpublish(publisher);
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel TracksTopicPublisherMetadataAvailability",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto* entry = model.AddEntry(wpi::nt::GetTopic(inst.GetHandle(), "/Value"));
    REQUIRE(entry);
    CHECK_FALSE(entry->GetTopicPublishers().has_value());

    auto metaTopic = wpi::nt::GetTopic(inst.GetHandle(), "$pub$/Value");
    auto publisher = wpi::nt::Publish(metaTopic, NT_RAW, "msgpack");
    REQUIRE(publisher != 0);
    model.Update();
    // An announcement alone does not supply a publisher list.
    CHECK_FALSE(entry->GetTopicPublishers().has_value());

    const std::vector<uint8_t> invalidMetadata = {0xc0};  // MessagePack nil
    REQUIRE(wpi::nt::SetRaw(publisher, invalidMetadata));
    model.Update();
    CHECK_FALSE(entry->GetTopicPublishers().has_value());

    size_t expectedSize = 0;
    SECTION("Empty publisher list") {
      REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicPublishersMetadata({})));
    }
    SECTION("Populated publisher list") {
      TopicPublisherMetadata publishers[] = {{"client", 42}};
      REQUIRE(
          wpi::nt::SetRaw(publisher, MakeTopicPublishersMetadata(publishers)));
      expectedSize = 1;
    }
    model.Update();
    auto publishers = entry->GetTopicPublishers();
    REQUIRE(publishers.has_value());
    CHECK(publishers->size() == expectedSize);

    // Failed decoding retains the last successfully decoded metadata.
    REQUIRE(wpi::nt::SetRaw(publisher, invalidMetadata));
    model.Update();
    publishers = entry->GetTopicPublishers();
    REQUIRE(publishers.has_value());
    CHECK(publishers->size() == expectedSize);

    wpi::nt::Unpublish(publisher);
    model.Update();
    CHECK_FALSE(entry->GetTopicPublishers().has_value());

    publisher = wpi::nt::Publish(metaTopic, NT_RAW, "msgpack");
    REQUIRE(publisher != 0);
    model.Update();
    CHECK_FALSE(entry->GetTopicPublishers().has_value());

    REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicPublishersMetadata({})));
    model.Update();
    publishers = entry->GetTopicPublishers();
    REQUIRE(publishers.has_value());
    CHECK(publishers->empty());
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel TopicPublisherMetadataWaitsForEntry",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto publisher =
        PublishMsgpack(inst, "$pub$/Value", MakeTopicPublishersMetadata({}));
    model.Update();
    CHECK(model.GetEntry("/Value") == nullptr);

    TopicPublisherMetadata metadata[] = {{"client", 42}};
    REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicPublishersMetadata(metadata)));
    model.Update();
    CHECK(model.GetEntry("/Value") == nullptr);

    auto topicPublisher = wpi::nt::Publish(
        wpi::nt::GetTopic(inst.GetHandle(), "/Value"), NT_DOUBLE, "double");
    REQUIRE(topicPublisher != 0);
    model.Update();
    auto* entry = model.GetEntry("/Value");
    REQUIRE(entry);
    auto publishers = entry->GetTopicPublishers();
    REQUIRE(publishers.has_value());
    REQUIRE(publishers->size() == 1);
    CHECK((*publishers)[0].client == "client");
    CHECK((*publishers)[0].pubuid == 42);
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicPublishersMetadata({})));
    model.Update();
    REQUIRE(model.GetEntry("/Value") == entry);
    publishers = entry->GetTopicPublishers();
    REQUIRE(publishers.has_value());
    CHECK(publishers->empty());
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel AppliesMetadataReceivedBeforeTopic",
          "[networktables]") {
  bool empty = GENERATE(false, true);
  bool publisherFirst = GENERATE(false, true);
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    TopicPublisherMetadata publishers[] = {{"publisher", 24}};
    TopicSubscriberMetadata subscribers[] = {
        {"subscriber", 42, true, true, true}};
    auto pubData = empty ? MakeTopicPublishersMetadata({})
                         : MakeTopicPublishersMetadata(publishers);
    auto subData = empty ? MakeTopicSubscribersMetadata({})
                         : MakeTopicSubscribersMetadata(subscribers);
    if (publisherFirst) {
      PublishMsgpack(inst, "$pub$/Value", pubData);
      PublishMsgpack(inst, "$sub$/Value", subData);
    } else {
      PublishMsgpack(inst, "$sub$/Value", subData);
      PublishMsgpack(inst, "$pub$/Value", pubData);
    }

    auto topic = wpi::nt::GetTopic(inst.GetHandle(), "/Value");
    wpi::glass::NetworkTablesModel::Entry* entry = nullptr;
    SECTION("Announcement in the same event batch") {
      REQUIRE(wpi::nt::Publish(topic, NT_DOUBLE, "double") != 0);
      model.Update();
      entry = model.GetEntry("/Value");
    }
    SECTION("Announcement in a later event batch") {
      model.Update();
      CHECK(model.GetEntry("/Value") == nullptr);
      REQUIRE(wpi::nt::Publish(topic, NT_DOUBLE, "double") != 0);
      model.Update();
      entry = model.GetEntry("/Value");
    }
    SECTION("Entry added explicitly") {
      model.Update();
      CHECK(model.GetEntry("/Value") == nullptr);
      entry = model.AddEntry(topic);
    }

    REQUIRE(entry);
    auto actualPublishers = entry->GetTopicPublishers();
    auto actualSubscribers = entry->GetTopicSubscribers();
    REQUIRE(actualPublishers.has_value());
    REQUIRE(actualSubscribers.has_value());
    if (empty) {
      CHECK(actualPublishers->empty());
      CHECK(actualSubscribers->empty());
    } else {
      REQUIRE(actualPublishers->size() == 1);
      CHECK((*actualPublishers)[0].client == "publisher");
      CHECK((*actualPublishers)[0].pubuid == 24);
      REQUIRE(actualSubscribers->size() == 1);
      CHECK((*actualSubscribers)[0].client == "subscriber");
      CHECK((*actualSubscribers)[0].subuid == 42);
      CHECK((*actualSubscribers)[0].options.topicsOnly);
      CHECK((*actualSubscribers)[0].options.sendAll);
      CHECK((*actualSubscribers)[0].options.prefixMatch);
    }
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel UsesLatestMetadataReceivedBeforeTopic",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    TopicPublisherMetadata publishers[] = {{"publisher", 24}};
    TopicSubscriberMetadata subscribers[] = {{"subscriber", 42}};
    auto pub = PublishMsgpack(inst, "$pub$/Value",
                              MakeTopicPublishersMetadata(publishers));
    auto sub = PublishMsgpack(inst, "$sub$/Value",
                              MakeTopicSubscribersMetadata(subscribers));
    model.Update();
    CHECK(model.GetEntry("/Value") == nullptr);

    bool available = false;
    SECTION("Lists replaced with empty metadata") {
      REQUIRE(wpi::nt::SetRaw(pub, MakeTopicPublishersMetadata({})));
      REQUIRE(wpi::nt::SetRaw(sub, MakeTopicSubscribersMetadata({})));
      available = true;
    }
    SECTION("Metadata removed") {
      wpi::nt::Unpublish(pub);
      wpi::nt::Unpublish(sub);
    }
    SECTION("Metadata removed and announced without a value") {
      wpi::nt::Unpublish(pub);
      wpi::nt::Unpublish(sub);
      REQUIRE(
          wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), "$pub$/Value"),
                           NT_RAW, "msgpack") != 0);
      REQUIRE(
          wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), "$sub$/Value"),
                           NT_RAW, "msgpack") != 0);
    }
    REQUIRE(wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), "/Value"),
                             NT_DOUBLE, "double") != 0);
    model.Update();
    auto* entry = model.GetEntry("/Value");
    REQUIRE(entry);
    auto actualPublishers = entry->GetTopicPublishers();
    auto actualSubscribers = entry->GetTopicSubscribers();
    if (available) {
      REQUIRE(actualPublishers.has_value());
      REQUIRE(actualSubscribers.has_value());
      CHECK(actualPublishers->empty());
      CHECK(actualSubscribers->empty());
    } else {
      CHECK_FALSE(actualPublishers.has_value());
      CHECK_FALSE(actualSubscribers.has_value());
    }
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel IgnoresInvalidMetadataReceivedBeforeTopic",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    SECTION("Invalid MessagePack payload") {
      PublishMsgpack(inst, "$pub$/Value", {0xc0});
      PublishMsgpack(inst, "$sub$/Value", {0xc0});
    }
    SECTION("Raw data with a different type string") {
      auto pub = wpi::nt::Publish(
          wpi::nt::GetTopic(inst.GetHandle(), "$pub$/Value"), NT_RAW, "raw");
      auto sub = wpi::nt::Publish(
          wpi::nt::GetTopic(inst.GetHandle(), "$sub$/Value"), NT_RAW, "raw");
      REQUIRE(pub != 0);
      REQUIRE(sub != 0);
      REQUIRE(wpi::nt::SetRaw(pub, MakeTopicPublishersMetadata({})));
      REQUIRE(wpi::nt::SetRaw(sub, MakeTopicSubscribersMetadata({})));
    }
    REQUIRE(wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), "/Value"),
                             NT_DOUBLE, "double") != 0);
    model.Update();
    auto* entry = model.GetEntry("/Value");
    REQUIRE(entry);
    CHECK_FALSE(entry->GetTopicPublishers().has_value());
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel TracksTopicSubscriberMetadataAvailability",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto* entry = model.AddEntry(wpi::nt::GetTopic(inst.GetHandle(), "/Value"));
    REQUIRE(entry);

    auto metaTopic = wpi::nt::GetTopic(inst.GetHandle(), "$sub$/Value");
    auto publisher = wpi::nt::Publish(metaTopic, NT_RAW, "msgpack");
    REQUIRE(publisher != 0);
    model.Update();
    // An announcement alone does not supply a subscriber list.
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    const std::vector<uint8_t> invalidMetadata = {0xc0};  // MessagePack nil
    REQUIRE(wpi::nt::SetRaw(publisher, invalidMetadata));
    model.Update();
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    size_t expectedSize = 0;
    SECTION("Empty subscriber list") {
      REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicSubscribersMetadata({})));
    }
    SECTION("Populated subscriber list") {
      TopicSubscriberMetadata subscribers[] = {{"client", 42}};
      REQUIRE(wpi::nt::SetRaw(publisher,
                              MakeTopicSubscribersMetadata(subscribers)));
      expectedSize = 1;
    }
    model.Update();
    auto subscribers = entry->GetTopicSubscribers();
    REQUIRE(subscribers.has_value());
    CHECK(subscribers->size() == expectedSize);

    // Failed decoding retains the last successfully decoded metadata.
    REQUIRE(wpi::nt::SetRaw(publisher, invalidMetadata));
    model.Update();
    subscribers = entry->GetTopicSubscribers();
    REQUIRE(subscribers.has_value());
    CHECK(subscribers->size() == expectedSize);

    wpi::nt::Unpublish(publisher);
    model.Update();
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    publisher = wpi::nt::Publish(metaTopic, NT_RAW, "msgpack");
    REQUIRE(publisher != 0);
    model.Update();
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());

    REQUIRE(wpi::nt::SetRaw(publisher, MakeTopicSubscribersMetadata({})));
    model.Update();
    subscribers = entry->GetTopicSubscribers();
    REQUIRE(subscribers.has_value());
    CHECK(subscribers->empty());
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel TopicSubscriberMetadataIndependentOfClients",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto* entry = model.AddEntry(wpi::nt::GetTopic(inst.GetHandle(), "/Value"));
    REQUIRE(entry);
    auto clients =
        PublishMsgpack(inst, "$clients", MakeClientsMetadata({{"client"}}));
    auto clientSub =
        PublishMsgpack(inst, "$clientsub$client",
                       MakeSubscribersMetadata({{"/Value"}}, false));
    auto serverSub = PublishMsgpack(
        inst, "$serversub", MakeSubscribersMetadata({{"/Value"}}, false));
    TopicSubscriberMetadata metadata[] = {{"explicit", 42}};
    auto topicSub = PublishMsgpack(inst, "$sub$/Value",
                                   MakeTopicSubscribersMetadata(metadata));
    model.Update();

    size_t expectedSize = 1;
    SECTION("Populated topic metadata") {}
    SECTION("Empty topic metadata") {
      REQUIRE(wpi::nt::SetRaw(topicSub, MakeTopicSubscribersMetadata({})));
      expectedSize = 0;
    }

    REQUIRE(wpi::nt::SetRaw(clientSub,
                            MakeSubscribersMetadata({{"/Other"}}, false, 2)));
    REQUIRE(wpi::nt::SetRaw(serverSub,
                            MakeSubscribersMetadata({{"/Other"}}, false, 2)));
    REQUIRE(wpi::nt::SetRaw(clients, MakeClientsMetadata({})));
    model.Update();
    auto subscribers = entry->GetTopicSubscribers();
    REQUIRE(subscribers.has_value());
    REQUIRE(subscribers->size() == expectedSize);
    if (!subscribers->empty()) {
      CHECK((*subscribers)[0].client == "explicit");
      CHECK((*subscribers)[0].subuid == 42);
    }

    wpi::nt::Unpublish(clientSub);
    wpi::nt::Unpublish(serverSub);
    wpi::nt::Unpublish(clients);
    model.Update();
    subscribers = entry->GetTopicSubscribers();
    REQUIRE(subscribers.has_value());
    CHECK(subscribers->size() == expectedSize);

    wpi::nt::Unpublish(topicSub);
    model.Update();
    CHECK_FALSE(entry->GetTopicSubscribers().has_value());
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTablesModel RepublishedTopicsStartWithoutPubSubMetadata",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};
    auto topic = wpi::nt::GetTopic(inst.GetHandle(), "/Value");
    for (int i = 0; i < 2; ++i) {
      auto publisher = wpi::nt::Publish(topic, NT_DOUBLE, "double");
      REQUIRE(publisher != 0);
      model.Update();
      auto* entry = model.GetEntry("/Value");
      REQUIRE(entry);
      CHECK_FALSE(entry->GetTopicPublishers().has_value());
      CHECK_FALSE(entry->GetTopicSubscribers().has_value());

      TopicPublisherMetadata pubMetadata[] = {{"client", 24}};
      auto topicPub = PublishMsgpack(inst, "$pub$/Value",
                                     MakeTopicPublishersMetadata(pubMetadata));
      TopicSubscriberMetadata metadata[] = {{"client", 42}};
      auto topicSub = PublishMsgpack(inst, "$sub$/Value",
                                     MakeTopicSubscribersMetadata(metadata));
      model.Update();
      auto publishers = entry->GetTopicPublishers();
      REQUIRE(publishers.has_value());
      REQUIRE(publishers->size() == 1);
      CHECK((*publishers)[0].pubuid == 24);
      auto subscribers = entry->GetTopicSubscribers();
      REQUIRE(subscribers.has_value());
      REQUIRE(subscribers->size() == 1);
      CHECK((*subscribers)[0].subuid == 42);

      REQUIRE(wpi::nt::SetDouble(publisher, 1.0));
      model.Update();
      publishers = entry->GetTopicPublishers();
      REQUIRE(publishers.has_value());
      REQUIRE(publishers->size() == 1);
      CHECK((*publishers)[0].pubuid == 24);
      subscribers = entry->GetTopicSubscribers();
      REQUIRE(subscribers.has_value());
      REQUIRE(subscribers->size() == 1);
      CHECK((*subscribers)[0].subuid == 42);

      wpi::nt::Unpublish(topicPub);
      wpi::nt::Unpublish(topicSub);
      wpi::nt::Unpublish(publisher);
      model.Update();
      CHECK(model.GetEntry("/Value") == nullptr);
    }
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}
