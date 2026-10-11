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

std::vector<uint8_t> MakeClientsMetadata(std::string_view clientId) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, 1);
  mpack::mpack_start_map(&writer, 3);
  mpack::mpack_write_str(&writer, "id");
  mpack::mpack_write_str(&writer, clientId);
  mpack::mpack_write_str(&writer, "conn");
  mpack::mpack_write_str(&writer, "unit-test");
  mpack::mpack_write_str(&writer, "ver");
  mpack::mpack_write_u16(&writer, 4);
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

std::vector<uint8_t> MakeSubscribersMetadata(
    std::span<const std::string_view> topics, bool prefixMatch) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, 1);
  mpack::mpack_start_map(&writer, 3);
  mpack::mpack_write_str(&writer, "uid");
  mpack::mpack_write_int(&writer, 1);
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

void PublishMsgpack(wpi::nt::NetworkTableInstance inst, std::string_view name,
                    const std::vector<uint8_t>& value) {
  auto publisher = wpi::nt::Publish(wpi::nt::GetTopic(inst.GetHandle(), name),
                                    NT_RAW, "msgpack");
  REQUIRE(publisher != 0);
  REQUIRE(wpi::nt::SetRaw(publisher, value));
}

}  // namespace

TEST_CASE("NetworkTablesModel DerivesSubscribersForTopicsWithoutMetadata",
          "[networktables]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  {
    wpi::glass::NetworkTablesModel model{inst};

    constexpr std::string_view topic = "/Prefix/Value";
    constexpr std::string_view prefix = "/Prefix";
    auto* entry = model.AddEntry(wpi::nt::GetTopic(inst.GetHandle(), topic));
    REQUIRE(entry);
    entry->value = wpi::nt::Value::MakeDouble(1.0);
    entry->UpdateFromValue(model);

    PublishMsgpack(inst, "$clients", MakeClientsMetadata("client"));
    PublishMsgpack(inst, "$clientsub$client",
                   MakeSubscribersMetadata(
                       std::span<const std::string_view>{&prefix, 1}, true));

    model.Update();

    entry = model.GetEntry(topic);
    REQUIRE(entry);
    CHECK(entry->publishers.empty());
    REQUIRE(entry->subscribers.size() == 1);
    CHECK(entry->subscribers[0].client == "client");
    CHECK(entry->subscribers[0].subuid == 1);
    CHECK(entry->subscribers[0].options.prefixMatch);
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

    TopicPublisherMetadata publishers[] = {{"client-a", -10}, {"client-b", 20}};
    TopicSubscriberMetadata subscribers[] = {
        {"client-c", -30, false, true, false},
        {"client-d", 40, true, false, true}};
    PublishMsgpack(inst, "$pub$/MetadataTopic",
                   MakeTopicPublishersMetadata(publishers));
    PublishMsgpack(inst, "$sub$/MetadataTopic",
                   MakeTopicSubscribersMetadata(subscribers));

    model.Update();

    auto* entry = model.GetEntry(topic);
    REQUIRE(entry);
    REQUIRE(entry->publishers.size() == 2);
    CHECK(entry->publishers[0].client == "client-a");
    CHECK(entry->publishers[0].pubuid == -10);
    CHECK(entry->publishers[1].client == "client-b");
    CHECK(entry->publishers[1].pubuid == 20);
    REQUIRE(entry->subscribers.size() == 2);
    CHECK(entry->subscribers[0].client == "client-c");
    CHECK(entry->subscribers[0].subuid == -30);
    CHECK(entry->subscribers[0].options.sendAll);
    CHECK_FALSE(entry->subscribers[0].options.prefixMatch);
    CHECK(entry->subscribers[1].client == "client-d");
    CHECK(entry->subscribers[1].subuid == 40);
    CHECK(entry->subscribers[1].options.topicsOnly);
    CHECK(entry->subscribers[1].options.prefixMatch);

    PublishMsgpack(
        inst, "$pub$/MetadataTopic",
        MakeTopicPublishersMetadata(std::span<const TopicPublisherMetadata>{}));
    PublishMsgpack(inst, "$sub$/MetadataTopic",
                   MakeTopicSubscribersMetadata(
                       std::span<const TopicSubscriberMetadata>{}));

    model.Update();

    entry = model.GetEntry(topic);
    REQUIRE(entry);
    CHECK(entry->publishers.empty());
    CHECK(entry->subscribers.empty());

    wpi::nt::Unpublish(publisher);
  }
  wpi::nt::NetworkTableInstance::Destroy(inst);
}
