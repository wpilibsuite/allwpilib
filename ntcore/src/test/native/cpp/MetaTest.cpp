// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <type_traits>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/MessagePack.hpp"
#include "wpi/util/mpack.h"
#include "wpi/util/raw_ostream.hpp"

namespace {
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

std::vector<uint8_t> MakeTopicPublisherMetadata(int64_t pubuid) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, 1);
  mpack::mpack_start_map(&writer, 2);
  mpack::mpack_write_str(&writer, "client");
  mpack::mpack_write_str(&writer, "client-a");
  mpack::mpack_write_str(&writer, "pubuid");
  mpack::mpack_write_int(&writer, pubuid);
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}

std::vector<uint8_t> MakeTopicSubscriberMetadata(int64_t subuid) {
  MsgpackWriter writer;
  mpack::mpack_start_array(&writer, 1);
  mpack::mpack_start_map(&writer, 3);
  mpack::mpack_write_str(&writer, "client");
  mpack::mpack_write_str(&writer, "client-a");
  mpack::mpack_write_str(&writer, "subuid");
  mpack::mpack_write_int(&writer, subuid);
  mpack::mpack_write_str(&writer, "options");
  mpack::mpack_start_map(&writer, 0);
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_map(&writer);
  mpack::mpack_finish_array(&writer);
  return writer.Finish();
}
}  // namespace

namespace wpi::nt {

TEST_CASE("MetaTest TopicMetadataUidsAreSigned", "[ntcore][meta]") {
  static_assert(
      std::is_same_v<decltype(meta::TopicPublisher::pubuid), int64_t>);
  static_assert(
      std::is_same_v<decltype(meta::TopicSubscriber::subuid), int64_t>);
  static_assert(
      std::is_same_v<decltype(NT_Meta_TopicPublisher::pubuid), int64_t>);
  static_assert(
      std::is_same_v<decltype(NT_Meta_TopicSubscriber::subuid), int64_t>);
}

TEST_CASE("MetaTest DecodeTopicMetadataSignedUids", "[ntcore][meta]") {
  auto publisherData = MakeTopicPublisherMetadata(-10);
  auto publishers = meta::DecodeTopicPublishers(publisherData);
  REQUIRE(publishers);
  REQUIRE(publishers->size() == 1);
  CHECK((*publishers)[0].pubuid == -10);

  size_t publisherCount = 0;
  auto cPublishers = NT_Meta_DecodeTopicPublishers(
      publisherData.data(), publisherData.size(), &publisherCount);
  REQUIRE(cPublishers != nullptr);
  REQUIRE(publisherCount == 1);
  CHECK(cPublishers[0].pubuid == -10);
  NT_Meta_FreeTopicPublishers(cPublishers, publisherCount);

  auto subscriberData = MakeTopicSubscriberMetadata(-30);
  auto subscribers = meta::DecodeTopicSubscribers(subscriberData);
  REQUIRE(subscribers);
  REQUIRE(subscribers->size() == 1);
  CHECK((*subscribers)[0].subuid == -30);

  size_t subscriberCount = 0;
  auto cSubscribers = NT_Meta_DecodeTopicSubscribers(
      subscriberData.data(), subscriberData.size(), &subscriberCount);
  REQUIRE(cSubscribers != nullptr);
  REQUIRE(subscriberCount == 1);
  CHECK(cSubscribers[0].subuid == -30);
  NT_Meta_FreeTopicSubscribers(cSubscribers, subscriberCount);
}

}  // namespace wpi::nt
