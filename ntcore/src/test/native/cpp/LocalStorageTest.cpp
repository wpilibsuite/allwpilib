// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>

#include "LocalStorage.h"
#include "MockListenerStorage.h"
#include "MockLogger.h"
#include "PubSubOptionsMatcher.h"
#include "TestPrinters.h"
#include "ValueMatcher.h"
#include "gmock/gmock.h"
#include "net/MockMessageHandler.h"
#include "net/MockNetworkInterface.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::Return;

namespace nt {

::testing::Matcher<const PubSubOptionsImpl&> IsPubSubOptions(
    const PubSubOptionsImpl& good) {
  return AllOf(
      Field("periodic", &PubSubOptionsImpl::periodicMs, good.periodicMs),
      Field("pollStorage", &PubSubOptionsImpl::pollStorage, good.pollStorage),
      Field("sendAll", &PubSubOptionsImpl::sendAll, good.sendAll),
      Field("keepDuplicates", &PubSubOptionsImpl::keepDuplicates,
            good.keepDuplicates));
}

::testing::Matcher<const PubSubOptionsImpl&> IsDefaultPubSubOptions() {
  static constexpr PubSubOptionsImpl kDefaultPubSubOptionsImpl;
  return IsPubSubOptions(kDefaultPubSubOptionsImpl);
}

class LocalStorageTest : public ::testing::Test {
 public:
  LocalStorageTest() { storage.StartNetwork(&network); }

  ::testing::StrictMock<net::MockClientMessageHandler> network;
  wpi::MockLogger logger;
  MockListenerStorage listenerStorage;
  LocalStorage storage{0, listenerStorage, logger};
  NT_Topic fooTopic{storage.GetTopic("foo")};
  NT_Topic barTopic{storage.GetTopic("bar")};
  NT_Topic bazTopic{storage.GetTopic("baz")};
};

TEST_F(LocalStorageTest, GetTopicsUnpublished) {
  EXPECT_TRUE(storage.GetTopics("", 0).empty());
  EXPECT_TRUE(storage.GetTopics("", {}).empty());
  EXPECT_TRUE(storage.GetTopicInfo("", 0).empty());
  EXPECT_TRUE(storage.GetTopicInfo("", {}).empty());
}

TEST_F(LocalStorageTest, GetTopic2) {
  auto foo2 = storage.GetTopic("foo");
  EXPECT_EQ(fooTopic, foo2);
  EXPECT_NE(fooTopic, barTopic);
}

TEST_F(LocalStorageTest, GetTopicEmptyName) {
  EXPECT_EQ(storage.GetTopic(""), 0u);
}

TEST_F(LocalStorageTest, GetEntryEmptyName) {
  EXPECT_EQ(storage.GetEntry(""), 0u);
}

TEST_F(LocalStorageTest, GetEntryCached) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"tocache"}}),
                                       IsDefaultPubSubOptions()));

  auto entry1 = storage.GetEntry("tocache");
  EXPECT_EQ(entry1, storage.GetEntry("tocache"));
}

TEST_F(LocalStorageTest, GetTopicName) {
  EXPECT_EQ(storage.GetTopicName(fooTopic), "foo");
  EXPECT_EQ(storage.GetTopicName(barTopic), "bar");
}

TEST_F(LocalStorageTest, GetTopicInfoUnpublished) {
  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.topic, fooTopic);
  EXPECT_EQ(info.name, "foo");
  EXPECT_EQ(info.type, NT_UNASSIGNED);
  EXPECT_TRUE(info.type_str.empty());
  EXPECT_EQ(info.properties, "{}");

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_UNASSIGNED);
  EXPECT_TRUE(storage.GetTopicTypeString(fooTopic).empty());
  EXPECT_FALSE(storage.GetTopicExists(fooTopic));
}

TEST_F(LocalStorageTest, DefaultProps) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", wpi::json::object(), {});

  EXPECT_FALSE(storage.GetTopicPersistent(fooTopic));
  EXPECT_FALSE(storage.GetTopicRetained(fooTopic));
  EXPECT_TRUE(storage.GetTopicCached(fooTopic));
}

TEST_F(LocalStorageTest, PublishNewNoProps) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", wpi::json::object(), {});

  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.properties, "{}");
}

TEST_F(LocalStorageTest, PublishNewNoPropsNull) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.properties, "{}");
}

TEST_F(LocalStorageTest, PublishNew) {
  wpi::json properties = {{"persistent", true}};
  EXPECT_CALL(network, ClientPublish(_, std::string_view{"foo"},
                                     std::string_view{"boolean"}, properties,
                                     IsDefaultPubSubOptions()));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {{"persistent", true}}, {});

  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.topic, fooTopic);
  EXPECT_EQ(info.name, "foo");
  EXPECT_EQ(info.type, NT_BOOLEAN);
  EXPECT_EQ(info.type_str, "boolean");
  EXPECT_EQ(info.properties, "{\"persistent\":true}");

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));
}

TEST_F(LocalStorageTest, SubscribeNoTypeLocalPubPost) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});

  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val));
  storage.SetEntryValue(pub, val);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);

  auto vals = storage.ReadQueue<bool>(sub);
  ASSERT_EQ(vals.size(), 1u);
  EXPECT_EQ(vals[0].value, true);
  EXPECT_EQ(vals[0].time, 5);

  val = Value::MakeBoolean(false, 6);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val));
  storage.SetEntryValue(pub, val);

  auto vals2 = storage.ReadQueue<int64_t>(sub);  // mismatched type
  ASSERT_TRUE(vals2.empty());
}

TEST_F(LocalStorageTest, SubscribeNoTypeLocalPubPre) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val));
  storage.SetEntryValue(pub, val);

  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);
}

TEST_F(LocalStorageTest, EntryNoTypeLocalSet) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // results in a publish and value set
  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  EXPECT_CALL(network, ClientSetValue(_, val));
  EXPECT_TRUE(storage.SetEntryValue(entry, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(entry);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);

  auto vals = storage.ReadQueue<bool>(entry);
  ASSERT_EQ(vals.size(), 1u);
  EXPECT_EQ(vals[0].value, true);
  EXPECT_EQ(vals[0].time, 5);

  // normal set with same type
  val = Value::MakeBoolean(false, 6);
  EXPECT_CALL(network, ClientSetValue(_, val));
  EXPECT_TRUE(storage.SetEntryValue(entry, val));

  auto vals2 = storage.ReadQueue<int64_t>(entry);  // mismatched type
  ASSERT_TRUE(vals2.empty());

  // cannot change type; won't generate network message
  EXPECT_FALSE(storage.SetEntryValue(entry, Value::MakeInteger(5, 7)));

  // should not change type or generate queue items
  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");

  auto vals3 = storage.ReadQueue<int64_t>(entry);  // mismatched type
  ASSERT_TRUE(vals3.empty());
}

TEST_F(LocalStorageTest, PubUnpubPub) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto sub = storage.Subscribe(fooTopic, NT_INTEGER, "int", {});

  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  EXPECT_CALL(logger,
              Call(NT_LOG_INFO, _, _,
                   std::string_view{
                       "local subscribe to 'foo' disabled due to type "
                       "mismatch (wanted 'int', published as 'boolean')"}));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val));
  EXPECT_TRUE(storage.SetEntryValue(pub, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_TRUE(storage.ReadQueue<int64_t>(sub).empty());

  EXPECT_CALL(network, ClientUnpublish(Handle{pub}.GetIndex()));
  storage.Unpublish(pub);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_UNASSIGNED);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "");
  EXPECT_FALSE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network,
              ClientPublish(_, std::string_view{"foo"}, std::string_view{"int"},
                            wpi::json::object(), IsDefaultPubSubOptions()));
  pub = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  val = Value::MakeInteger(3, 5);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val));
  EXPECT_TRUE(storage.SetEntryValue(pub, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_EQ(storage.ReadQueue<int64_t>(sub).size(), 1u);
}

TEST_F(LocalStorageTest, LocalPubConflict) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub1 = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(
      logger,
      Call(NT_LOG_INFO, _, _,
           std::string_view{"local publish to 'foo' disabled due to type "
                            "mismatch (wanted 'int', currently 'boolean')"}));
  auto pub2 = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, ClientSetValue(Handle{pub1}.GetIndex(), _));

  EXPECT_TRUE(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  EXPECT_FALSE(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));

  // unpublishing pub1 will publish pub2 to the network
  EXPECT_CALL(network, ClientUnpublish(Handle{pub1}.GetIndex()));
  EXPECT_CALL(network,
              ClientPublish(_, std::string_view{"foo"}, std::string_view{"int"},
                            wpi::json::object(), IsDefaultPubSubOptions()));
  storage.Unpublish(pub1);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, ClientSetValue(Handle{pub2}.GetIndex(), _));

  EXPECT_FALSE(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  EXPECT_TRUE(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));
}

TEST_F(LocalStorageTest, LocalSubConflict) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  EXPECT_CALL(logger,
              Call(NT_LOG_INFO, _, _,
                   std::string_view{
                       "local subscribe to 'foo' disabled due to type "
                       "mismatch (wanted 'int', published as 'boolean')"}));
  storage.Subscribe(fooTopic, NT_INTEGER, "int", {});
}

TEST_F(LocalStorageTest, RemotePubConflict) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));

  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(logger,
              Call(NT_LOG_INFO, _, _,
                   std::string_view{
                       "network announce of 'foo' overriding local publish "
                       "(was 'boolean', now 'int')"}));

  auto id = storage.ServerAnnounce("foo", 0, "int", wpi::json::object(),
                                   std::nullopt);

  // network overrides local
  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));

  storage.ServerUnannounce("foo", id);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));
}

TEST_F(LocalStorageTest, SubNonExist) {
  // makes sure no warning is emitted
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
}

TEST_F(LocalStorageTest, SetDefaultSubscribe) {
  // no publish, no value on wire, this is just handled locally
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  EXPECT_TRUE(storage.SetDefaultEntryValue(sub, Value::MakeBoolean(true)));
  auto val = storage.GetEntryValue(sub);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultPublish) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  // expect a value across the wire
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));

  EXPECT_CALL(network, ClientSubscribe(_, _, IsDefaultPubSubOptions()));
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  auto val = storage.GetEntryValue(sub);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntry) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto entry = storage.GetEntry(fooTopic, NT_BOOLEAN, "boolean", {});

  // expect a publish and value
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, ClientSetValue(_, expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  auto val = storage.GetEntryValue(entry);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntryUnassigned) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // expect a publish and value
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"boolean"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, ClientSetValue(_, expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  ASSERT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  auto val = storage.GetEntryValue(entry);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntryDiffType) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"string"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  EXPECT_FALSE(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));
  ASSERT_EQ(storage.GetTopicType(fooTopic), NT_STRING);
}

TEST_F(LocalStorageTest, SetValueEmptyValue) {
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"string"},
                    wpi::json::object(), IsDefaultPubSubOptions()));
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  EXPECT_FALSE(storage.SetEntryValue(pub, {}));
}

TEST_F(LocalStorageTest, SetValueEmptyUntypedEntry) {
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsDefaultPubSubOptions()));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});
  EXPECT_FALSE(storage.SetEntryValue(entry, {}));
}

TEST_F(LocalStorageTest, PublishUntyped) {
  EXPECT_CALL(logger,
              Call(NT_LOG_ERROR, _, _,
                   std::string_view{"cannot publish 'foo' with an unassigned "
                                    "type or empty type string"}));

  EXPECT_EQ(storage.Publish(fooTopic, NT_UNASSIGNED, "", {}, {}), 0u);
}

TEST_F(LocalStorageTest, SetValueInvalidHandle) {
  EXPECT_FALSE(storage.SetEntryValue(0u, {}));
}

class LocalStorageDuplicatesTest : public LocalStorageTest {
 public:
  void SetupPubSub(bool keepPub, bool keepSub);
  void SetValues(bool expectDuplicates);

  NT_Publisher pub;
  NT_Subscriber sub;
  Value val1 = Value::MakeDouble(1.0, 10);
  Value val2 = Value::MakeDouble(1.0, 20);  // duplicate value
  Value val3 = Value::MakeDouble(2.0, 30);
};

void LocalStorageDuplicatesTest::SetupPubSub(bool keepPub, bool keepSub) {
  PubSubOptionsImpl pubOptions;
  pubOptions.keepDuplicates = keepPub;
  EXPECT_CALL(
      network,
      ClientPublish(_, std::string_view{"foo"}, std::string_view{"double"},
                    wpi::json::object(), IsPubSubOptions(pubOptions)));
  pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {},
                        {.keepDuplicates = keepPub});

  PubSubOptionsImpl subOptions;
  subOptions.pollStorage = 10;
  subOptions.keepDuplicates = keepSub;
  EXPECT_CALL(network, ClientSubscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                       IsPubSubOptions(subOptions)));
  sub = storage.Subscribe(fooTopic, NT_DOUBLE, "double",
                          {.pollStorage = 10, .keepDuplicates = keepSub});
}

void LocalStorageDuplicatesTest::SetValues(bool expectDuplicates) {
  storage.SetEntryValue(pub, val1);
  storage.SetEntryValue(pub, val2);
  // verify the timestamp was updated (or not)
  EXPECT_EQ(storage.GetEntryLastChange(sub),
            expectDuplicates ? val2.time() : val1.time());
  storage.SetEntryValue(pub, val3);
}

TEST_F(LocalStorageDuplicatesTest, Defaults) {
  SetupPubSub(false, false);

  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val1));
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val3));
  SetValues(false);

  // verify 2nd update was dropped locally
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 2u);
  ASSERT_EQ(values[0].value, val1.GetDouble());
  ASSERT_EQ(values[0].time, val1.time());
  ASSERT_EQ(values[1].value, val3.GetDouble());
  ASSERT_EQ(values[1].time, val3.time());
}

TEST_F(LocalStorageDuplicatesTest, KeepPub) {
  SetupPubSub(true, false);

  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val1)).Times(2);
  // EXPECT_CALL(network, SetValue(pub, val2));
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val3));
  SetValues(true);

  // verify only 2 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 2u);
}

TEST_F(LocalStorageDuplicatesTest, KeepSub) {
  SetupPubSub(false, true);

  // second update should NOT go to the network
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val1));
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val3));
  SetValues(false);

  // verify 2 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 2u);
}

TEST_F(LocalStorageDuplicatesTest, KeepPubSub) {
  SetupPubSub(true, true);

  // second update SHOULD go to the network
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val1)).Times(2);
  // EXPECT_CALL(network, SetValue(pub, val2));
  EXPECT_CALL(network, ClientSetValue(Handle{pub}.GetIndex(), val3));
  SetValues(true);

  // verify all 3 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 3u);
}

TEST_F(LocalStorageDuplicatesTest, FromNetworkDefault) {
  SetupPubSub(false, false);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double", {{}}, std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  EXPECT_EQ(storage.GetEntryLastChange(sub), val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was dropped for local subscriber
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 2u);
  ASSERT_EQ(values[0].value, val1.GetDouble());
  ASSERT_EQ(values[0].time, val1.time());
  ASSERT_EQ(values[1].value, val3.GetDouble());
  ASSERT_EQ(values[1].time, val3.time());
}

TEST_F(LocalStorageDuplicatesTest, FromNetworkKeepPub) {
  SetupPubSub(true, false);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double", {{}}, std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  EXPECT_EQ(storage.GetEntryLastChange(sub), val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was dropped for local subscriber
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 2u);
  ASSERT_EQ(values[0].value, val1.GetDouble());
  ASSERT_EQ(values[0].time, val1.time());
  ASSERT_EQ(values[1].value, val3.GetDouble());
  ASSERT_EQ(values[1].time, val3.time());
}
TEST_F(LocalStorageDuplicatesTest, FromNetworkKeepSub) {
  SetupPubSub(false, true);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double", {{}}, std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  EXPECT_EQ(storage.GetEntryLastChange(sub), val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was received by local subscriber
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 3u);
  ASSERT_EQ(values[0].value, val1.GetDouble());
  ASSERT_EQ(values[0].time, val1.time());
  ASSERT_EQ(values[1].value, val2.GetDouble());
  ASSERT_EQ(values[1].time, val2.time());
  ASSERT_EQ(values[2].value, val3.GetDouble());
  ASSERT_EQ(values[2].time, val3.time());
}

TEST_F(LocalStorageDuplicatesTest, FromNetworkKeepPubSub) {
  SetupPubSub(true, true);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double", {{}}, std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  EXPECT_EQ(storage.GetEntryLastChange(sub), val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was received by local subscriber
  auto values = storage.ReadQueue<double>(sub);
  ASSERT_EQ(values.size(), 3u);
  ASSERT_EQ(values[0].value, val1.GetDouble());
  ASSERT_EQ(values[0].time, val1.time());
  ASSERT_EQ(values[1].value, val2.GetDouble());
  ASSERT_EQ(values[1].time, val2.time());
  ASSERT_EQ(values[2].value, val3.GetDouble());
  ASSERT_EQ(values[2].time, val3.time());
}

class LocalStorageNumberVariantsTest : public LocalStorageTest {
 public:
  void CreateSubscriber(NT_Handle* handle, std::string_view name, NT_Type type,
                        std::string_view typeStr);
  void CreateSubscribers();
  void CreateSubscribersArray();

  NT_Subscriber sub1, sub2, sub3, sub4;
  NT_Entry entry;

  struct SubEntry {
    SubEntry(NT_Handle subentry, NT_Type type, std::string_view name)
        : subentry{subentry}, type{type}, name{name} {}
    NT_Handle subentry;
    NT_Type type;
    std::string name;
  };
  std::vector<SubEntry> subentries;
};

void LocalStorageNumberVariantsTest::CreateSubscriber(
    NT_Handle* handle, std::string_view name, NT_Type type,
    std::string_view typeStr) {
  *handle = storage.Subscribe(fooTopic, type, typeStr, {});
  subentries.emplace_back(*handle, type, name);
}

void LocalStorageNumberVariantsTest::CreateSubscribers() {
  EXPECT_CALL(logger,
              Call(NT_LOG_INFO, _, _,
                   std::string_view{
                       "local subscribe to 'foo' disabled due to type "
                       "mismatch (wanted 'boolean', published as 'double')"}));
  CreateSubscriber(&sub1, "subDouble", NT_DOUBLE, "double");
  CreateSubscriber(&sub2, "subInteger", NT_INTEGER, "int");
  CreateSubscriber(&sub3, "subFloat", NT_FLOAT, "float");
  CreateSubscriber(&sub4, "subBoolean", NT_BOOLEAN, "boolean");
  entry = storage.GetEntry("foo");
  subentries.emplace_back(entry, NT_UNASSIGNED, "entry");
}

void LocalStorageNumberVariantsTest::CreateSubscribersArray() {
  EXPECT_CALL(
      logger,
      Call(NT_LOG_INFO, _, _,
           std::string_view{
               "local subscribe to 'foo' disabled due to type "
               "mismatch (wanted 'boolean[]', published as 'double[]')"}));
  CreateSubscriber(&sub1, "subDouble", NT_DOUBLE_ARRAY, "double[]");
  CreateSubscriber(&sub2, "subInteger", NT_INTEGER_ARRAY, "int[]");
  CreateSubscriber(&sub3, "subFloat", NT_FLOAT_ARRAY, "float[]");
  CreateSubscriber(&sub4, "subBoolean", NT_BOOLEAN_ARRAY, "boolean[]");
  entry = storage.GetEntry("foo");
  subentries.emplace_back(entry, NT_UNASSIGNED, "entry");
}

TEST_F(LocalStorageNumberVariantsTest, GetEntryPubAfter) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(5);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(1);
  CreateSubscribers();
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // all subscribers get the actual type and time
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    EXPECT_EQ(storage.GetEntryType(subentry.subentry), NT_DOUBLE);
    EXPECT_EQ(storage.GetEntryLastChange(subentry.subentry), 50);
  }
  // for subscribers, they get a converted value or nothing on mismatch
  EXPECT_EQ(storage.GetEntryValue(sub1), Value::MakeDouble(1.0, 50));
  EXPECT_EQ(storage.GetEntryValue(sub2), Value::MakeInteger(1, 50));
  EXPECT_EQ(storage.GetEntryValue(sub3), Value::MakeFloat(1.0, 50));
  EXPECT_EQ(storage.GetEntryValue(sub4), Value{});
  // entries just get whatever the value is
  EXPECT_EQ(storage.GetEntryValue(entry), Value::MakeDouble(1.0, 50));
}

TEST_F(LocalStorageNumberVariantsTest, GetEntryPubBefore) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(5);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(1);
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // all subscribers get the actual type and time
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    EXPECT_EQ(storage.GetEntryType(subentry.subentry), NT_DOUBLE);
    EXPECT_EQ(storage.GetEntryLastChange(subentry.subentry), 50);
  }
  // for subscribers, they get a converted value or nothing on mismatch
  EXPECT_EQ(storage.GetEntryValue(sub1), Value::MakeDouble(1.0, 50));
  EXPECT_EQ(storage.GetEntryValue(sub2), Value::MakeInteger(1, 50));
  EXPECT_EQ(storage.GetEntryValue(sub3), Value::MakeFloat(1.0, 50));
  EXPECT_EQ(storage.GetEntryValue(sub4), Value{});
  // entries just get whatever the value is
  EXPECT_EQ(storage.GetEntryValue(entry), Value::MakeDouble(1.0, 50));
}

template <typename T>
::testing::Matcher<const T&> TSEq(auto value, int64_t time) {
  return AllOf(Field("value", &T::value, value), Field("time", &T::time, time));
}

TEST_F(LocalStorageNumberVariantsTest, GetAtomic) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(5);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(1);
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));

  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    EXPECT_THAT(storage.GetAtomic<double>(subentry.subentry, 0),
                TSEq<TimestampedDouble>(1.0, 50));
    EXPECT_THAT(storage.GetAtomic<int64_t>(subentry.subentry, 0),
                TSEq<TimestampedInteger>(1, 50));
    EXPECT_THAT(storage.GetAtomic<float>(subentry.subentry, 0),
                TSEq<TimestampedFloat>(1.0, 50));
    EXPECT_THAT(storage.GetAtomic<bool>(subentry.subentry, false),
                TSEq<TimestampedBoolean>(false, 0));
  }
}

template <typename T, typename U>
::testing::Matcher<const T&> TSSpanEq(std::span<U> value, int64_t time) {
  return AllOf(
      Field("value", &T::value, wpi::SpanEq(std::span<const U>(value))),
      Field("time", &T::time, time));
}

TEST_F(LocalStorageNumberVariantsTest, GetAtomicArray) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(5);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(1);
  auto pub = storage.Publish(fooTopic, NT_DOUBLE_ARRAY, "double[]", {}, {});
  CreateSubscribersArray();
  storage.SetEntryValue(pub, Value::MakeDoubleArray({1.0}, 50));

  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    double doubleVal = 1.0;
    EXPECT_THAT(storage.GetAtomic<double[]>(subentry.subentry, {}),
                TSSpanEq<TimestampedDoubleArray>(std::span{&doubleVal, 1}, 50));
    int64_t intVal = 1;
    EXPECT_THAT(storage.GetAtomic<int64_t[]>(subentry.subentry, {}),
                TSSpanEq<TimestampedIntegerArray>(std::span{&intVal, 1}, 50));
    float floatVal = 1.0;
    EXPECT_THAT(storage.GetAtomic<float[]>(subentry.subentry, {}),
                TSSpanEq<TimestampedFloatArray>(std::span{&floatVal, 1}, 50));
    EXPECT_THAT(storage.GetAtomic<bool[]>(subentry.subentry, {}),
                TSSpanEq<TimestampedBooleanArray>(std::span<int>{}, 0));
  }
}

TEST_F(LocalStorageNumberVariantsTest, ReadQueue) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(5);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(4);
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();

  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      EXPECT_THAT(storage.ReadQueue<double>(subentry.subentry), IsEmpty());
    } else {
      EXPECT_THAT(storage.ReadQueue<double>(subentry.subentry),
                  ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(2.0, 50));
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      EXPECT_THAT(storage.ReadQueue<int64_t>(subentry.subentry), IsEmpty());
    } else {
      EXPECT_THAT(storage.ReadQueue<int64_t>(subentry.subentry),
                  ElementsAre(TSEq<TimestampedInteger>(2, 50)));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(3.0, 50));
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      EXPECT_THAT(storage.ReadQueue<float>(subentry.subentry), IsEmpty());
    } else {
      EXPECT_THAT(storage.ReadQueue<float>(subentry.subentry),
                  ElementsAre(TSEq<TimestampedFloat>(3.0, 50)));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(4.0, 50));
  for (auto&& subentry : subentries) {
    SCOPED_TRACE(subentry.name);
    EXPECT_THAT(storage.ReadQueue<bool>(subentry.subentry), IsEmpty());
  }
}

TEST_F(LocalStorageTest, MultiSubSpecial) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(2);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(2);
  EXPECT_CALL(network, ClientSetValue(_, _)).Times(2);
  EXPECT_CALL(listenerStorage, Activate(_, _, _)).Times(2);

  auto subnormal = storage.SubscribeMultiple({{""}}, {});
  auto subspecial = storage.SubscribeMultiple({{"", "$"}}, {});
  auto pubnormal = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto specialTopic = storage.GetTopic("$topic");
  auto pubspecial = storage.Publish(specialTopic, NT_DOUBLE, "double", {}, {});
  storage.AddListener(1, subnormal, NT_EVENT_VALUE_ALL);
  storage.AddListener(2, subspecial, NT_EVENT_VALUE_ALL);

  EXPECT_CALL(
      listenerStorage,
      Notify(wpi::SpanEq(std::span<const NT_Listener>{{2}}), _, _, _, _));
  storage.SetEntryValue(pubspecial, Value::MakeDouble(1.0, 30));

  EXPECT_CALL(
      listenerStorage,
      Notify(wpi::SpanEq(std::span<const NT_Listener>{{1}}), _, _, _, _));
  EXPECT_CALL(
      listenerStorage,
      Notify(wpi::SpanEq(std::span<const NT_Listener>{{2}}), _, _, _, _));
  storage.SetEntryValue(pubnormal, Value::MakeDouble(2.0, 40));
}

TEST_F(LocalStorageTest, NetworkDuplicateDetect) {
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _));
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto remoteTopic = storage.ServerAnnounce("foo", 0, "double",
                                            wpi::json::object(), std::nullopt);

  // local set
  EXPECT_CALL(network, ClientSetValue(_, _));
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // 2nd local set with same value - no SetValue call to network
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 60));
  // network set with different value
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 70));
  // 3rd local set with same value generates a SetValue call to network
  EXPECT_CALL(network, ClientSetValue(_, _));
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 80));
}

TEST_F(LocalStorageTest, ReadQueueLocalRemote) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(3);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);

  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", kDefaultPubSubOptions);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto remoteTopic = storage.ServerAnnounce("foo", 0, "double",
                                            wpi::json::object(), std::nullopt);

  // local set
  EXPECT_CALL(network, ClientSetValue(_, _));
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  EXPECT_THAT(storage.ReadQueue<double>(subBoth),
              ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
  EXPECT_THAT(storage.ReadQueue<double>(subLocal),
              ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
  EXPECT_THAT(storage.ReadQueue<double>(subRemote), IsEmpty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  EXPECT_THAT(storage.ReadQueue<double>(subBoth),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
  EXPECT_THAT(storage.ReadQueue<double>(subRemote),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
  EXPECT_THAT(storage.ReadQueue<double>(subLocal), IsEmpty());
}

TEST_F(LocalStorageTest, SubExcludePub) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(2);
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _)).Times(1);

  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto subActive = storage.Subscribe(fooTopic, NT_DOUBLE, "double", {});
  auto subExclude = storage.Subscribe(fooTopic, NT_DOUBLE, "double",
                                      {.excludePublisher = pub});
  auto remoteTopic = storage.ServerAnnounce("foo", 0, "double",
                                            wpi::json::object(), std::nullopt);

  // local set
  EXPECT_CALL(network, ClientSetValue(_, _));
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  EXPECT_THAT(storage.ReadQueue<double>(subActive),
              ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
  EXPECT_THAT(storage.ReadQueue<double>(subExclude), IsEmpty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  EXPECT_THAT(storage.ReadQueue<double>(subActive),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
  EXPECT_THAT(storage.ReadQueue<double>(subExclude),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
}

TEST_F(LocalStorageTest, EntryExcludeSelf) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _));

  auto entry =
      storage.GetEntry(fooTopic, NT_DOUBLE, "double", {.excludeSelf = true});
  auto remoteTopic = storage.ServerAnnounce("foo", 0, "double",
                                            wpi::json::object(), std::nullopt);

  // local set
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _));
  EXPECT_CALL(network, ClientSetValue(_, _));
  storage.SetEntryValue(entry, Value::MakeDouble(1.0, 50));
  EXPECT_THAT(storage.ReadQueue<double>(entry), IsEmpty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  EXPECT_THAT(storage.ReadQueue<double>(entry),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
}

TEST_F(LocalStorageTest, ReadQueueInitialLocal) {
  EXPECT_CALL(network, ClientPublish(_, _, _, _, _));
  EXPECT_CALL(network, ClientSetValue(_, _));
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(3);

  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));

  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", kDefaultPubSubOptions);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});

  EXPECT_THAT(storage.ReadQueue<double>(subBoth),
              ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
  EXPECT_THAT(storage.ReadQueue<double>(subLocal),
              ElementsAre(TSEq<TimestampedDouble>(1.0, 50)));
  EXPECT_THAT(storage.ReadQueue<double>(subRemote), IsEmpty());
}

TEST_F(LocalStorageTest, ReadQueueInitialRemote) {
  EXPECT_CALL(network, ClientSubscribe(_, _, _)).Times(3);

  auto remoteTopic = storage.ServerAnnounce("foo", 0, "double",
                                            wpi::json::object(), std::nullopt);
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));

  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", kDefaultPubSubOptions);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});

  // network set
  EXPECT_THAT(storage.ReadQueue<double>(subBoth),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
  EXPECT_THAT(storage.ReadQueue<double>(subRemote),
              ElementsAre(TSEq<TimestampedDouble>(2.0, 60)));
  EXPECT_THAT(storage.ReadQueue<double>(subLocal), IsEmpty());
}

}  // namespace nt
