// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorage.h"
#include "MockLogger.h"
#include "PubSubOptionsMatcher.h"
#include "SpanMatcher.h"
#include "TestPrinters.h"
#include "ValueMatcher.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "net/MockNetworkInterface.h"
#include "ntcore_c.h"

using ::testing::_;
using ::testing::Return;

namespace nt {

::testing::Matcher<const PubSubOptions&> IsPubSubOptions(
    const PubSubOptions& good) {
  return ::testing::AllOf(
      ::testing::Field("periodic", &PubSubOptions::periodic, good.periodic),
      ::testing::Field("pollStorageSize", &PubSubOptions::pollStorageSize,
                       good.pollStorageSize),
      ::testing::Field("logging", &PubSubOptions::sendAll, good.sendAll),
      ::testing::Field("keepDuplicates", &PubSubOptions::keepDuplicates,
                       good.keepDuplicates));
}

class LocalStorageTest : public ::testing::Test {
 public:
  LocalStorageTest() {
    storage.StartNetwork(startup);
    storage.SetNetwork(&network);
  }

  ::testing::StrictMock<net::MockNetworkStartupInterface> startup;
  ::testing::StrictMock<net::MockNetworkInterface> network;
  wpi::MockLogger logger;
  LocalStorage storage{0, logger};
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

TEST_F(LocalStorageTest, PublishNewNoProps) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", wpi::json::object(), {});

  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.properties, "{}");
}

TEST_F(LocalStorageTest, PublishNewNoPropsNull) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto info = storage.GetTopicInfo(fooTopic);
  EXPECT_EQ(info.properties, "{}");
}

TEST_F(LocalStorageTest, PublishNew) {
  wpi::json properties = {{"persistent", true}};
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, properties,
                               IsPubSubOptions({})));
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
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});

  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, SetValue(pub, val));
  storage.SetEntryValue(pub, val);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);

  auto vals = storage.ReadQueueBoolean(sub);
  ASSERT_EQ(vals.size(), 1u);
  EXPECT_EQ(vals[0].value, true);
  EXPECT_EQ(vals[0].time, 5);

  val = Value::MakeBoolean(true, 6);
  EXPECT_CALL(network, SetValue(pub, val));
  storage.SetEntryValue(pub, val);

  auto vals2 = storage.ReadQueueInteger(sub);  // mismatched type
  ASSERT_TRUE(vals2.empty());
}

TEST_F(LocalStorageTest, SubscribeNoTypeLocalPubPre) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, SetValue(pub, val));
  storage.SetEntryValue(pub, val);

  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);

  auto vals = storage.ReadQueueValue(sub);  // read queue won't get anything
  ASSERT_TRUE(vals.empty());
}

TEST_F(LocalStorageTest, EntryNoTypeLocalSet) {
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // results in a publish and value set
  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  EXPECT_CALL(network, SetValue(_, val));
  EXPECT_TRUE(storage.SetEntryValue(entry, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(entry);
  ASSERT_TRUE(value.IsBoolean());
  EXPECT_EQ(value.GetBoolean(), true);
  EXPECT_EQ(value.time(), 5);

  auto vals = storage.ReadQueueBoolean(entry);
  ASSERT_EQ(vals.size(), 1u);
  EXPECT_EQ(vals[0].value, true);
  EXPECT_EQ(vals[0].time, 5);

  // normal set with same type
  val = Value::MakeBoolean(true, 6);
  EXPECT_CALL(network, SetValue(_, val));
  EXPECT_TRUE(storage.SetEntryValue(entry, val));

  auto vals2 = storage.ReadQueueInteger(entry);  // mismatched type
  ASSERT_TRUE(vals2.empty());

  // cannot change type; won't generate network message
  EXPECT_FALSE(storage.SetEntryValue(entry, Value::MakeInteger(5, 7)));

  // should not change type or generate queue items
  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");

  auto vals3 = storage.ReadQueueInteger(entry);  // mismatched type
  ASSERT_TRUE(vals3.empty());
}

TEST_F(LocalStorageTest, PubUnpubPub) {
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto sub = storage.Subscribe(fooTopic, NT_INTEGER, "int", {});

  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  EXPECT_CALL(network, SetValue(pub, val));
  EXPECT_TRUE(storage.SetEntryValue(pub, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_TRUE(storage.ReadQueueInteger(sub).empty());

  EXPECT_CALL(network, Unpublish(pub, fooTopic));
  storage.Unpublish(pub);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_UNASSIGNED);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "");
  EXPECT_FALSE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"int"}, wpi::json::object(),
                               IsPubSubOptions({})));
  pub = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  val = Value::MakeInteger(3, 5);
  EXPECT_CALL(network, SetValue(pub, val));
  EXPECT_TRUE(storage.SetEntryValue(pub, val));

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_EQ(storage.ReadQueueInteger(sub).size(), 1u);
}

TEST_F(LocalStorageTest, LocalPubConflict) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub1 = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(logger, Call(NT_LOG_INFO, _, _,
                           "local publish to 'foo' disabled due to type "
                           "mismatch (wanted 'int', currently 'boolean')"));
  auto pub2 = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, SetValue(pub1, _));

  EXPECT_TRUE(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  EXPECT_FALSE(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));

  // unpublishing pub1 will publish pub2 to the network
  EXPECT_CALL(network, Unpublish(pub1, fooTopic));
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"int"}, wpi::json::object(),
                               IsPubSubOptions({})));
  storage.Unpublish(pub1);

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, SetValue(pub2, _));

  EXPECT_FALSE(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  EXPECT_TRUE(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));
}

TEST_F(LocalStorageTest, LocalSubConflict) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  EXPECT_CALL(logger, Call(NT_LOG_INFO, _, _,
                           "local subscribe to 'foo' disabled due to type "
                           "mismatch (wanted 'int', currently 'boolean')"));
  storage.Subscribe(fooTopic, NT_INTEGER, "int", {});
}

TEST_F(LocalStorageTest, RemotePubConflict) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));

  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  EXPECT_CALL(logger, Call(NT_LOG_INFO, _, _,
                           "network announce of 'foo' overriding local publish "
                           "(was 'boolean', now 'int')"));

  storage.NetworkAnnounce("foo", "int", wpi::json::object(), {});

  // network overrides local
  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_INTEGER);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "int");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));

  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));

  storage.NetworkUnannounce("foo");

  EXPECT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  EXPECT_EQ(storage.GetTopicTypeString(fooTopic), "boolean");
  EXPECT_TRUE(storage.GetTopicExists(fooTopic));
}

TEST_F(LocalStorageTest, SubNonExist) {
  // makes sure no warning is emitted
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
}

TEST_F(LocalStorageTest, SetDefaultSubscribe) {
  // no publish, no value on wire, this is just handled locally
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  EXPECT_TRUE(storage.SetDefaultEntryValue(sub, Value::MakeBoolean(true)));
  auto val = storage.GetEntryValue(sub);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultPublish) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  // expect a value across the wire
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, SetValue(pub, expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));

  EXPECT_CALL(network, Subscribe(_, _, IsPubSubOptions({})));
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  auto val = storage.GetEntryValue(sub);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntry) {
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto entry = storage.GetEntry(fooTopic, NT_BOOLEAN, "boolean", {});

  // expect a publish and value
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, SetValue(_, expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  auto val = storage.GetEntryValue(entry);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntryUnassigned) {
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // expect a publish and value
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"boolean"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto expectVal = Value::MakeBoolean(true, 0);
  EXPECT_CALL(network, SetValue(_, expectVal));
  EXPECT_TRUE(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  ASSERT_EQ(storage.GetTopicType(fooTopic), NT_BOOLEAN);
  auto val = storage.GetEntryValue(entry);
  ASSERT_TRUE(val.IsBoolean());
  ASSERT_TRUE(val.GetBoolean());
  ASSERT_EQ(val.time(), 0);
}

TEST_F(LocalStorageTest, SetDefaultEntryDiffType) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"string"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  EXPECT_FALSE(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));
  ASSERT_EQ(storage.GetTopicType(fooTopic), NT_STRING);
}

TEST_F(LocalStorageTest, SetValueEmptyValue) {
  EXPECT_CALL(network, Publish(_, fooTopic, std::string_view{"foo"},
                               std::string_view{"string"}, wpi::json::object(),
                               IsPubSubOptions({})));
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  EXPECT_FALSE(storage.SetEntryValue(pub, {}));
}

TEST_F(LocalStorageTest, SetValueEmptyUntypedEntry) {
  EXPECT_CALL(network, Subscribe(_, wpi::SpanEq({std::string{"foo"}}),
                                 IsPubSubOptions({})));
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});
  EXPECT_FALSE(storage.SetEntryValue(entry, {}));
}

TEST_F(LocalStorageTest, PublishUntyped) {
  EXPECT_EQ(storage.Publish(fooTopic, NT_UNASSIGNED, "", {}, {}), 0u);
}

TEST_F(LocalStorageTest, SetValueInvalidHandle) {
  EXPECT_FALSE(storage.SetEntryValue(0u, {}));
}

}  // namespace nt
