// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorage.hpp"

#include <algorithm>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "MockAssertions.hpp"
#include "MockListenerStorage.hpp"
#include "MockLogger.hpp"
#include "TestPrinters.hpp"
#include "net/MockMessageHandler.hpp"
#include "net/MockNetworkInterface.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"

namespace wpi::nt {

class LocalStorageTest {
 public:
  LocalStorageTest() { storage.StartNetwork(&network); }

  net::MockClientMessageHandler network;
  wpi::MockLogger logger;
  MockListenerStorage listenerStorage;
  LocalStorage storage{0, listenerStorage, logger};
  NT_Topic fooTopic{storage.GetTopic("foo")};
  NT_Topic barTopic{storage.GetTopic("bar")};
  NT_Topic bazTopic{storage.GetTopic("baz")};
};

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetTopicsUnpublished",
                 "[ntcore][local-storage]") {
  CHECK(storage.GetTopics("", 0).empty());
  CHECK(storage.GetTopics("", {}).empty());
  CHECK(storage.GetTopicInfo("", 0).empty());
  CHECK(storage.GetTopicInfo("", {}).empty());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetTopic2",
                 "[ntcore][local-storage]") {
  auto foo2 = storage.GetTopic("foo");
  CHECK(fooTopic == foo2);
  CHECK(fooTopic != barTopic);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetTopicEmptyName",
                 "[ntcore][local-storage]") {
  CHECK(storage.GetTopic("") == 0);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetEntryEmptyName",
                 "[ntcore][local-storage]") {
  CHECK(storage.GetEntry("") == 0);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetEntryCached",
                 "[ntcore][local-storage]") {
  auto entry1 = storage.GetEntry("tocache");
  CHECK(entry1 == storage.GetEntry("tocache"));

  CheckNetworkCounts(network, 0, 1, 0);
  CheckSubscribe(network.subscribeCalls[0], {"tocache"});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetTopicName",
                 "[ntcore][local-storage]") {
  CHECK(storage.GetTopicName(fooTopic) == "foo");
  CHECK(storage.GetTopicName(barTopic) == "bar");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest GetTopicInfoUnpublished",
                 "[ntcore][local-storage]") {
  auto info = storage.GetTopicInfo(fooTopic);
  CHECK(info.topic == fooTopic);
  CHECK(info.name == "foo");
  CHECK(info.type == NT_UNASSIGNED);
  CHECK(info.type_str.empty());
  CHECK(info.properties == "{}");

  CHECK(storage.GetTopicType(fooTopic) == NT_UNASSIGNED);
  CHECK(storage.GetTopicTypeString(fooTopic).empty());
  CHECK_FALSE(storage.GetTopicExists(fooTopic));
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest DefaultProps",
                 "[ntcore][local-storage]") {
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", wpi::util::json::object(),
                  {});

  CHECK_FALSE(storage.GetTopicPersistent(fooTopic));
  CHECK_FALSE(storage.GetTopicRetained(fooTopic));
  CHECK(storage.GetTopicCached(fooTopic));

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest PublishNewNoProps",
                 "[ntcore][local-storage]") {
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", wpi::util::json::object(),
                  {});

  auto info = storage.GetTopicInfo(fooTopic);
  CHECK(info.properties == "{}");

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest PublishNewNoPropsNull",
                 "[ntcore][local-storage]") {
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto info = storage.GetTopicInfo(fooTopic);
  CHECK(info.properties == "{}");

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest PublishNew",
                 "[ntcore][local-storage]") {
  auto properties = wpi::util::json::object("persistent", true);
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean",
                  wpi::util::json::object("persistent", true), {});

  auto info = storage.GetTopicInfo(fooTopic);
  CHECK(info.topic == fooTopic);
  CHECK(info.name == "foo");
  CHECK(info.type == NT_BOOLEAN);
  CHECK(info.type_str == "boolean");
  CHECK(info.properties == "{\"persistent\":true}");

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean", properties);
}

TEST_CASE_METHOD(LocalStorageTest,
                 "LocalStorageTest SubscribeNoTypeLocalPubPost",
                 "[ntcore][local-storage]") {
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  storage.SetEntryValue(pub, val);

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  REQUIRE(value.IsBoolean());
  CHECK(value.GetBoolean() == true);
  CHECK(value.time() == 5);

  auto vals = storage.ReadQueue<bool>(sub);
  REQUIRE(vals.size() == 1u);
  CHECK(vals[0].value == true);
  CHECK(vals[0].time == 5);

  val = Value::MakeBoolean(false, 6);
  storage.SetEntryValue(pub, val);

  auto vals2 = storage.ReadQueue<int64_t>(sub);  // mismatched type
  REQUIRE(vals2.empty());

  CheckNetworkCounts(network, 1, 1, 2);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeBoolean(true, 5));
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(),
                Value::MakeBoolean(false, 6));
}

TEST_CASE_METHOD(LocalStorageTest,
                 "LocalStorageTest SubscribeNoTypeLocalPubPre",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  storage.SetEntryValue(pub, val);
  auto sub = storage.Subscribe(fooTopic, NT_UNASSIGNED, "", {});

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(sub);
  REQUIRE(value.IsBoolean());
  CHECK(value.GetBoolean() == true);
  CHECK(value.time() == 5);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest EntryNoTypeLocalSet",
                 "[ntcore][local-storage]") {
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // results in a publish and value set
  auto val = Value::MakeBoolean(true, 5);
  CHECK(storage.SetEntryValue(entry, val));

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  auto value = storage.GetEntryValue(entry);
  REQUIRE(value.IsBoolean());
  CHECK(value.GetBoolean() == true);
  CHECK(value.time() == 5);

  auto vals = storage.ReadQueue<bool>(entry);
  REQUIRE(vals.size() == 1u);
  CHECK(vals[0].value == true);
  CHECK(vals[0].time == 5);

  // normal set with same type
  val = Value::MakeBoolean(false, 6);
  CHECK(storage.SetEntryValue(entry, val));

  auto vals2 = storage.ReadQueue<int64_t>(entry);  // mismatched type
  REQUIRE(vals2.empty());

  // cannot change type; won't generate network message
  CHECK_FALSE(storage.SetEntryValue(entry, Value::MakeInteger(5, 7)));

  // should not change type or generate queue items
  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");

  auto vals3 = storage.ReadQueue<int64_t>(entry);  // mismatched type
  REQUIRE(vals3.empty());

  CheckNetworkCounts(network, 1, 1, 2);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], network.publishCalls[0].pubuid,
                Value::MakeBoolean(true, 5));
  CheckSetValue(network.setValueCalls[1], network.publishCalls[0].pubuid,
                Value::MakeBoolean(false, 6));
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest PubUnpubPub",
                 "[ntcore][local-storage]") {
  auto sub = storage.Subscribe(fooTopic, NT_INTEGER, "int", {});
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto val = Value::MakeBoolean(true, 5);
  CHECK(storage.SetEntryValue(pub, val));

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  CHECK(storage.ReadQueue<int64_t>(sub).empty());
  storage.Unpublish(pub);

  CHECK(storage.GetTopicType(fooTopic) == NT_UNASSIGNED);
  CHECK(storage.GetTopicTypeString(fooTopic) == "");
  CHECK_FALSE(storage.GetTopicExists(fooTopic));
  pub = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  val = Value::MakeInteger(3, 5);
  CHECK(storage.SetEntryValue(pub, val));

  CHECK(storage.GetTopicType(fooTopic) == NT_INTEGER);
  CHECK(storage.GetTopicTypeString(fooTopic) == "int");
  CHECK(storage.GetTopicExists(fooTopic));

  CHECK(storage.ReadQueue<int64_t>(sub).size() == 1u);

  CheckNetworkCounts(network, 2, 1, 2, 1);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckPublish(network.publishCalls[1], "foo", "int",
               wpi::util::json::object());
  CHECK(network.unpublishCalls[0] == network.publishCalls[0].pubuid);
  CheckSetValue(network.setValueCalls[0], network.publishCalls[0].pubuid,
                Value::MakeBoolean(true, 5));
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(),
                Value::MakeInteger(3, 5));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted 'int', "
      "published as 'boolean')");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest LocalPubConflict",
                 "[ntcore][local-storage]") {
  auto pub1 = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});
  auto pub2 = storage.Publish(fooTopic, NT_INTEGER, "int", {}, {});

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  CHECK(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  CHECK_FALSE(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));

  // unpublishing pub1 will publish pub2 to the network
  storage.Unpublish(pub1);

  CHECK(storage.GetTopicType(fooTopic) == NT_INTEGER);
  CHECK(storage.GetTopicTypeString(fooTopic) == "int");
  CHECK(storage.GetTopicExists(fooTopic));

  CHECK_FALSE(storage.SetEntryValue(pub1, Value::MakeBoolean(true, 5)));
  CHECK(storage.SetEntryValue(pub2, Value::MakeInteger(3, 5)));

  CheckNetworkCounts(network, 2, 0, 2, 1);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckPublish(network.publishCalls[1], "foo", "int",
               wpi::util::json::object());
  CHECK(network.unpublishCalls[0] == static_cast<int>(Handle{pub1}.GetIndex()));
  CheckSetValue(network.setValueCalls[0], Handle{pub1}.GetIndex(),
                Value::MakeBoolean(true, 5));
  CheckSetValue(network.setValueCalls[1], Handle{pub2}.GetIndex(),
                Value::MakeInteger(3, 5));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local publish to 'foo' disabled due to type mismatch (wanted 'int', "
      "currently 'boolean')");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest LocalSubConflict",
                 "[ntcore][local-storage]") {
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});
  storage.Subscribe(fooTopic, NT_INTEGER, "int", {});

  CheckNetworkCounts(network, 1, 1, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted 'int', "
      "published as 'boolean')");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest RemotePubConflict",
                 "[ntcore][local-storage]") {
  storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  auto id = storage.ServerAnnounce("foo", 0, "int", wpi::util::json::object(),
                                   std::nullopt);

  // network overrides local
  CHECK(storage.GetTopicType(fooTopic) == NT_INTEGER);
  CHECK(storage.GetTopicTypeString(fooTopic) == "int");
  CHECK(storage.GetTopicExists(fooTopic));

  storage.ServerUnannounce("foo", id);

  CHECK(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  CHECK(storage.GetTopicTypeString(fooTopic) == "boolean");
  CHECK(storage.GetTopicExists(fooTopic));

  CheckNetworkCounts(network, 2, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckPublish(network.publishCalls[1], "foo", "boolean",
               wpi::util::json::object());
  logger.CheckMessage(
      NT_LOG_INFO,
      "network announce of 'foo' overriding local publish (was 'boolean', now "
      "'int')");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SubNonExist",
                 "[ntcore][local-storage]") {
  // makes sure no warning is emitted
  storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});

  CheckNetworkCounts(network, 0, 1, 0);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CHECK(logger.messages.empty());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetDefaultSubscribe",
                 "[ntcore][local-storage]") {
  // no publish, no value on wire, this is just handled locally
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  CHECK(storage.SetDefaultEntryValue(sub, Value::MakeBoolean(true)));
  auto val = storage.GetEntryValue(sub);
  REQUIRE(val.IsBoolean());
  REQUIRE(val.GetBoolean());
  REQUIRE(val.time() == 0);

  CheckNetworkCounts(network, 0, 1, 0);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetDefaultPublish",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_BOOLEAN, "boolean", {}, {});

  // expect a value across the wire
  auto expectVal = Value::MakeBoolean(true, 0);
  CHECK(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));
  auto sub = storage.Subscribe(fooTopic, NT_BOOLEAN, "boolean", {});
  auto val = storage.GetEntryValue(sub);
  REQUIRE(val.IsBoolean());
  REQUIRE(val.GetBoolean());
  REQUIRE(val.time() == 0);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), expectVal);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetDefaultEntry",
                 "[ntcore][local-storage]") {
  auto entry = storage.GetEntry(fooTopic, NT_BOOLEAN, "boolean", {});

  // expect a publish and value
  auto expectVal = Value::MakeBoolean(true, 0);
  CHECK(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  auto val = storage.GetEntryValue(entry);
  REQUIRE(val.IsBoolean());
  REQUIRE(val.GetBoolean());
  REQUIRE(val.time() == 0);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], network.publishCalls[0].pubuid,
                expectVal);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetDefaultEntryUnassigned",
                 "[ntcore][local-storage]") {
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});

  // expect a publish and value
  auto expectVal = Value::MakeBoolean(true, 0);
  CHECK(storage.SetDefaultEntryValue(entry, Value::MakeBoolean(true)));

  REQUIRE(storage.GetTopicType(fooTopic) == NT_BOOLEAN);
  auto val = storage.GetEntryValue(entry);
  REQUIRE(val.IsBoolean());
  REQUIRE(val.GetBoolean());
  REQUIRE(val.time() == 0);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
  CheckPublish(network.publishCalls[0], "foo", "boolean",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], network.publishCalls[0].pubuid,
                expectVal);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetDefaultEntryDiffType",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  CHECK_FALSE(storage.SetDefaultEntryValue(pub, Value::MakeBoolean(true)));
  REQUIRE(storage.GetTopicType(fooTopic) == NT_STRING);

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "string",
               wpi::util::json::object());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetValueEmptyValue",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_STRING, "string", {}, {});

  CHECK_FALSE(storage.SetEntryValue(pub, {}));

  CheckNetworkCounts(network, 1, 0, 0);
  CheckPublish(network.publishCalls[0], "foo", "string",
               wpi::util::json::object());
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetValueEmptyUntypedEntry",
                 "[ntcore][local-storage]") {
  auto entry = storage.GetEntry(fooTopic, NT_UNASSIGNED, "", {});
  CHECK_FALSE(storage.SetEntryValue(entry, {}));

  CheckNetworkCounts(network, 0, 1, 0);
  CheckSubscribe(network.subscribeCalls[0], {"foo"});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest PublishUntyped",
                 "[ntcore][local-storage]") {
  CHECK(storage.Publish(fooTopic, NT_UNASSIGNED, "", {}, {}) == 0);

  CheckNoClientCalls(network);
  logger.CheckMessage(
      NT_LOG_ERROR,
      "cannot publish 'foo' with an unassigned type or empty type string");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SetValueInvalidHandle",
                 "[ntcore][local-storage]") {
  CHECK_FALSE(storage.SetEntryValue(0, {}));
  CheckNoClientCalls(network);
}

TEST_CASE_METHOD(
    LocalStorageTest,
    "LocalStorageTest DisableSignalSubscriberQueuesWithoutSignaling",
    "[ntcore][local-storage]") {
  PubSubOptionsImpl subOptions;
  subOptions.disableSignal = true;
  auto sub =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableSignal = true});

  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});

  auto val = Value::MakeDouble(1.0, 50);
  storage.SetEntryValue(pub, val);

  bool timedOut = false;
  CHECK_FALSE(wpi::util::WaitForObject(sub, 0, &timedOut));
  CHECK(timedOut);
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 1u);
  CHECK(values[0].value == 1.0);
  CHECK(values[0].time == 50);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckSubscribe(network.subscribeCalls[0], {"foo"}, subOptions);
  CheckPublish(network.publishCalls[0], "foo", "double",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val);
}

TEST_CASE_METHOD(
    LocalStorageTest,
    "LocalStorageTest DisableSignalMultiSubscriberDoesNotSignalHandle",
    "[ntcore][local-storage]") {
  PubSubOptionsImpl subOptions;
  subOptions.disableSignal = true;
  subOptions.prefixMatch = true;
  auto sub = storage.SubscribeMultiple({{""}}, {.disableSignal = true});

  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});

  auto val = Value::MakeDouble(1.0, 50);
  storage.SetEntryValue(pub, val);

  bool timedOut = false;
  CHECK_FALSE(wpi::util::WaitForObject(sub, 0, &timedOut));
  CHECK(timedOut);

  CheckNetworkCounts(network, 1, 1, 1);
  CheckSubscribe(network.subscribeCalls[0], {""}, subOptions);
  CheckPublish(network.publishCalls[0], "foo", "double",
               wpi::util::json::object());
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val);
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
  auto publishCount = network.publishCalls.size();
  auto unpublishCount = network.unpublishCalls.size();
  auto setPropertiesCount = network.setPropertiesCalls.size();
  auto subscribeCount = network.subscribeCalls.size();
  auto unsubscribeCount = network.unsubscribeCalls.size();
  auto setValueCount = network.setValueCalls.size();

  PubSubOptionsImpl pubOptions;
  pubOptions.keepDuplicates = keepPub;
  pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {},
                        {.keepDuplicates = keepPub});

  PubSubOptionsImpl subOptions;
  subOptions.pollStorage = 10;
  subOptions.keepDuplicates = keepSub;
  sub = storage.Subscribe(fooTopic, NT_DOUBLE, "double",
                          {.pollStorage = 10, .keepDuplicates = keepSub});

  CheckClientMessageCounts(network, {
                                        .publish = publishCount + 1,
                                        .unpublish = unpublishCount,
                                        .setProperties = setPropertiesCount,
                                        .subscribe = subscribeCount + 1,
                                        .unsubscribe = unsubscribeCount,
                                        .setValue = setValueCount,
                                    });
  CheckPublish(network.publishCalls[publishCount], "foo", "double",
               wpi::util::json::object(), pubOptions);
  CheckSubscribe(network.subscribeCalls[subscribeCount], {"foo"}, subOptions);
}

void LocalStorageDuplicatesTest::SetValues(bool expectDuplicates) {
  storage.SetEntryValue(pub, val1);
  storage.SetEntryValue(pub, val2);
  // verify the timestamp was updated (or not)
  CHECK(storage.GetEntryLastChange(sub) ==
        (expectDuplicates ? val2.time() : val1.time()));
  storage.SetEntryValue(pub, val3);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest Defaults",
                 "[ntcore][local-storage]") {
  SetupPubSub(false, false);
  SetValues(false);

  // verify 2nd update was dropped locally
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 2u);
  REQUIRE(values[0].value == val1.GetDouble());
  REQUIRE(values[0].time == val1.time());
  REQUIRE(values[1].value == val3.GetDouble());
  REQUIRE(values[1].time == val3.time());

  CheckNetworkCounts(network, 1, 1, 2);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val1);
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(), val3);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest KeepPub",
                 "[ntcore][local-storage]") {
  SetupPubSub(true, false);
  SetValues(true);

  // verify only 2 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 2u);

  CheckNetworkCounts(network, 1, 1, 3);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val1);
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(), val2);
  CheckSetValue(network.setValueCalls[2], Handle{pub}.GetIndex(), val3);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest KeepSub",
                 "[ntcore][local-storage]") {
  SetupPubSub(false, true);

  // second update should NOT go to the network
  SetValues(false);

  // verify 2 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 2u);

  CheckNetworkCounts(network, 1, 1, 2);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val1);
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(), val3);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest KeepPubSub",
                 "[ntcore][local-storage]") {
  SetupPubSub(true, true);

  // second update SHOULD go to the network
  SetValues(true);

  // verify all 3 updates were received locally
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 3u);

  CheckNetworkCounts(network, 1, 1, 3);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(), val1);
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(), val2);
  CheckSetValue(network.setValueCalls[2], Handle{pub}.GetIndex(), val3);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest FromNetworkDefault",
                 "[ntcore][local-storage]") {
  SetupPubSub(false, false);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double",
                                      wpi::util::json::object(), std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  CHECK(storage.GetEntryLastChange(sub) == val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was dropped for local subscriber
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 2u);
  REQUIRE(values[0].value == val1.GetDouble());
  REQUIRE(values[0].time == val1.time());
  REQUIRE(values[1].value == val3.GetDouble());
  REQUIRE(values[1].time == val3.time());

  CheckNetworkCounts(network, 1, 1, 0);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest FromNetworkKeepPub",
                 "[ntcore][local-storage]") {
  SetupPubSub(true, false);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double",
                                      wpi::util::json::object(), std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  CHECK(storage.GetEntryLastChange(sub) == val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was dropped for local subscriber
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 2u);
  REQUIRE(values[0].value == val1.GetDouble());
  REQUIRE(values[0].time == val1.time());
  REQUIRE(values[1].value == val3.GetDouble());
  REQUIRE(values[1].time == val3.time());

  CheckNetworkCounts(network, 1, 1, 0);
}
TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest FromNetworkKeepSub",
                 "[ntcore][local-storage]") {
  SetupPubSub(false, true);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double",
                                      wpi::util::json::object(), std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  CHECK(storage.GetEntryLastChange(sub) == val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was received by local subscriber
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 3u);
  REQUIRE(values[0].value == val1.GetDouble());
  REQUIRE(values[0].time == val1.time());
  REQUIRE(values[1].value == val2.GetDouble());
  REQUIRE(values[1].time == val2.time());
  REQUIRE(values[2].value == val3.GetDouble());
  REQUIRE(values[2].time == val3.time());

  CheckNetworkCounts(network, 1, 1, 0);
}

TEST_CASE_METHOD(LocalStorageDuplicatesTest,
                 "LocalStorageDuplicatesTest FromNetworkKeepPubSub",
                 "[ntcore][local-storage]") {
  SetupPubSub(true, true);

  // incoming from the network are treated like a normal local publish
  auto topic = storage.ServerAnnounce("foo", 0, "double",
                                      wpi::util::json::object(), std::nullopt);
  storage.ServerSetValue(topic, val1);
  storage.ServerSetValue(topic, val2);
  // verify the timestamp was updated
  CHECK(storage.GetEntryLastChange(sub) == val2.time());
  storage.ServerSetValue(topic, val3);

  // verify 2nd update was received by local subscriber
  auto values = storage.ReadQueue<double>(sub);
  REQUIRE(values.size() == 3u);
  REQUIRE(values[0].value == val1.GetDouble());
  REQUIRE(values[0].time == val1.time());
  REQUIRE(values[1].value == val2.GetDouble());
  REQUIRE(values[1].time == val2.time());
  REQUIRE(values[2].value == val3.GetDouble());
  REQUIRE(values[2].time == val3.time());

  CheckNetworkCounts(network, 1, 1, 0);
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
  CreateSubscriber(&sub1, "subDouble", NT_DOUBLE, "double");
  CreateSubscriber(&sub2, "subInteger", NT_INTEGER, "int");
  CreateSubscriber(&sub3, "subFloat", NT_FLOAT, "float");
  CreateSubscriber(&sub4, "subBoolean", NT_BOOLEAN, "boolean");
  entry = storage.GetEntry("foo");
  subentries.emplace_back(entry, NT_UNASSIGNED, "entry");
}

void LocalStorageNumberVariantsTest::CreateSubscribersArray() {
  CreateSubscriber(&sub1, "subDouble", NT_DOUBLE_ARRAY, "double[]");
  CreateSubscriber(&sub2, "subInteger", NT_INTEGER_ARRAY, "int[]");
  CreateSubscriber(&sub3, "subFloat", NT_FLOAT_ARRAY, "float[]");
  CreateSubscriber(&sub4, "subBoolean", NT_BOOLEAN_ARRAY, "boolean[]");
  entry = storage.GetEntry("foo");
  subentries.emplace_back(entry, NT_UNASSIGNED, "entry");
}

TEST_CASE_METHOD(LocalStorageNumberVariantsTest,
                 "LocalStorageNumberVariantsTest GetEntryPubAfter",
                 "[ntcore][local-storage]") {
  CreateSubscribers();
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // all subscribers get the actual type and time
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    CHECK(storage.GetEntryType(subentry.subentry) == NT_DOUBLE);
    CHECK(storage.GetEntryLastChange(subentry.subentry) == 50);
  }
  // for subscribers, they get a converted value or nothing on mismatch
  CHECK(storage.GetEntryValue(sub1) == Value::MakeDouble(1.0, 50));
  CHECK(storage.GetEntryValue(sub2) == Value::MakeInteger(1, 50));
  CHECK(storage.GetEntryValue(sub3) == Value::MakeFloat(1.0, 50));
  CHECK(storage.GetEntryValue(sub4) == Value{});
  // entries just get whatever the value is
  CHECK(storage.GetEntryValue(entry) == Value::MakeDouble(1.0, 50));

  CheckNetworkCounts(network, 1, 5, 1);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeDouble(1.0, 50));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted "
      "'boolean', published as 'double')");
}

TEST_CASE_METHOD(LocalStorageNumberVariantsTest,
                 "LocalStorageNumberVariantsTest GetEntryPubBefore",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // all subscribers get the actual type and time
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    CHECK(storage.GetEntryType(subentry.subentry) == NT_DOUBLE);
    CHECK(storage.GetEntryLastChange(subentry.subentry) == 50);
  }
  // for subscribers, they get a converted value or nothing on mismatch
  CHECK(storage.GetEntryValue(sub1) == Value::MakeDouble(1.0, 50));
  CHECK(storage.GetEntryValue(sub2) == Value::MakeInteger(1, 50));
  CHECK(storage.GetEntryValue(sub3) == Value::MakeFloat(1.0, 50));
  CHECK(storage.GetEntryValue(sub4) == Value{});
  // entries just get whatever the value is
  CHECK(storage.GetEntryValue(entry) == Value::MakeDouble(1.0, 50));

  CheckNetworkCounts(network, 1, 5, 1);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeDouble(1.0, 50));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted "
      "'boolean', published as 'double')");
}

template <typename T, typename U>
bool TimestampedEq(const T& actual, U value, int64_t time) {
  return actual.value == value && actual.time == time;
}

TEST_CASE_METHOD(LocalStorageNumberVariantsTest,
                 "LocalStorageNumberVariantsTest GetAtomic",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));

  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    CHECK(TimestampedEq(storage.GetAtomic<double>(subentry.subentry, 0), 1.0,
                        50));
    CHECK(
        TimestampedEq(storage.GetAtomic<int64_t>(subentry.subentry, 0), 1, 50));
    CHECK(TimestampedEq(storage.GetAtomic<float>(subentry.subentry, 0), 1.0f,
                        50));
    CHECK(TimestampedEq(storage.GetAtomic<bool>(subentry.subentry, false),
                        false, 0));
  }

  CheckNetworkCounts(network, 1, 5, 1);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeDouble(1.0, 50));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted "
      "'boolean', published as 'double')");
}

template <typename T, typename U>
bool TimestampedSpanEq(const T& actual, std::span<U> value, int64_t time) {
  if (actual.time != time || actual.value.size() != value.size()) {
    return false;
  }

  using ActualValue = std::remove_cvref_t<decltype(actual.value[0])>;
  using ExpectedValue = std::remove_cvref_t<decltype(value[0])>;
  using CommonValue = std::common_type_t<ActualValue, ExpectedValue>;
  for (size_t i = 0; i < value.size(); ++i) {
    if (static_cast<CommonValue>(actual.value[i]) !=
        static_cast<CommonValue>(value[i])) {
      return false;
    }
  }
  return true;
}

template <typename T, typename U>
bool SingleTimestampedEq(const T& actual, U value, int64_t time) {
  return actual.size() == 1 && TimestampedEq(actual[0], value, time);
}

TEST_CASE_METHOD(LocalStorageNumberVariantsTest,
                 "LocalStorageNumberVariantsTest GetAtomicArray",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE_ARRAY, "double[]", {}, {});
  CreateSubscribersArray();
  storage.SetEntryValue(pub, Value::MakeDoubleArray({1.0}, 50));

  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    double doubleVal = 1.0;
    CHECK(TimestampedSpanEq(storage.GetAtomic<double[]>(subentry.subentry, {}),
                            std::span{&doubleVal, 1}, 50));
    int64_t intVal = 1;
    CHECK(TimestampedSpanEq(storage.GetAtomic<int64_t[]>(subentry.subentry, {}),
                            std::span{&intVal, 1}, 50));
    float floatVal = 1.0;
    CHECK(TimestampedSpanEq(storage.GetAtomic<float[]>(subentry.subentry, {}),
                            std::span{&floatVal, 1}, 50));
    CHECK(TimestampedSpanEq(storage.GetAtomic<bool[]>(subentry.subentry, {}),
                            std::span<const bool>{}, 0));
  }

  CheckNetworkCounts(network, 1, 5, 1);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeDoubleArray({1.0}, 50));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted "
      "'boolean[]', published as 'double[]')");
}

TEST_CASE_METHOD(LocalStorageNumberVariantsTest,
                 "LocalStorageNumberVariantsTest ReadQueue",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  CreateSubscribers();

  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      CHECK(storage.ReadQueue<double>(subentry.subentry).empty());
    } else {
      CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subentry.subentry),
                                1.0, 50));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(2.0, 50));
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      CHECK(storage.ReadQueue<int64_t>(subentry.subentry).empty());
    } else {
      CHECK(SingleTimestampedEq(storage.ReadQueue<int64_t>(subentry.subentry),
                                2, 50));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(3.0, 50));
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    if (subentry.type == NT_BOOLEAN) {
      CHECK(storage.ReadQueue<float>(subentry.subentry).empty());
    } else {
      CHECK(SingleTimestampedEq(storage.ReadQueue<float>(subentry.subentry),
                                3.0f, 50));
    }
  }

  storage.SetEntryValue(pub, Value::MakeDouble(4.0, 50));
  for (auto&& subentry : subentries) {
    INFO(subentry.name);
    CHECK(storage.ReadQueue<bool>(subentry.subentry).empty());
  }

  CheckNetworkCounts(network, 1, 5, 4);
  CheckSetValue(network.setValueCalls[0], Handle{pub}.GetIndex(),
                Value::MakeDouble(1.0, 50));
  CheckSetValue(network.setValueCalls[1], Handle{pub}.GetIndex(),
                Value::MakeDouble(2.0, 50));
  CheckSetValue(network.setValueCalls[2], Handle{pub}.GetIndex(),
                Value::MakeDouble(3.0, 50));
  CheckSetValue(network.setValueCalls[3], Handle{pub}.GetIndex(),
                Value::MakeDouble(4.0, 50));
  logger.CheckMessage(
      NT_LOG_INFO,
      "local subscribe to 'foo' disabled due to type mismatch (wanted "
      "'boolean', published as 'double')");
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest MultiSubSpecial",
                 "[ntcore][local-storage]") {
  auto subnormal = storage.SubscribeMultiple({{""}}, {});
  auto subspecial = storage.SubscribeMultiple({{"", "$"}}, {});
  auto pubnormal = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto specialTopic = storage.GetTopic("$topic");
  auto pubspecial = storage.Publish(specialTopic, NT_DOUBLE, "double", {}, {});
  storage.AddListener(1, subnormal, NT_EVENT_VALUE_ALL);
  storage.AddListener(2, subspecial, NT_EVENT_VALUE_ALL);
  storage.SetEntryValue(pubspecial, Value::MakeDouble(1.0, 30));
  storage.SetEntryValue(pubnormal, Value::MakeDouble(2.0, 40));

  CheckNetworkCounts(network, 2, 2, 2);
  REQUIRE(listenerStorage.activateCalls.size() == 2);
  REQUIRE(listenerStorage.valueNotifyCalls.size() == 3);
  CheckValueNotifyHandles(listenerStorage.valueNotifyCalls[0], {2});
  CheckValueNotifyHandles(listenerStorage.valueNotifyCalls[1], {1});
  CheckValueNotifyHandles(listenerStorage.valueNotifyCalls[2], {2});
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest NetworkDuplicateDetect",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto remoteTopic = storage.ServerAnnounce(
      "foo", 0, "double", wpi::util::json::object(), std::nullopt);

  // local set
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  // 2nd local set with same value - no SetValue call to network
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 60));
  // network set with different value
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 70));
  // 3rd local set with same value generates a SetValue call to network
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 80));

  CheckNetworkCounts(network, 1, 0, 2);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest ReadQueueLocalRemote",
                 "[ntcore][local-storage]") {
  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", DEFAULT_PUB_SUB_OPTIONS);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto remoteTopic = storage.ServerAnnounce(
      "foo", 0, "double", wpi::util::json::object(), std::nullopt);

  // local set
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subBoth), 1.0, 50));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subLocal), 1.0, 50));
  CHECK(storage.ReadQueue<double>(subRemote).empty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subBoth), 2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subRemote), 2.0, 60));
  CHECK(storage.ReadQueue<double>(subLocal).empty());

  CheckNetworkCounts(network, 1, 3, 1);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest SubExcludePub",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  auto subActive = storage.Subscribe(fooTopic, NT_DOUBLE, "double", {});
  auto subExclude = storage.Subscribe(fooTopic, NT_DOUBLE, "double",
                                      {.excludePublisher = pub});
  auto remoteTopic = storage.ServerAnnounce(
      "foo", 0, "double", wpi::util::json::object(), std::nullopt);

  // local set
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subActive), 1.0, 50));
  CHECK(storage.ReadQueue<double>(subExclude).empty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subActive), 2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subExclude), 2.0, 60));

  CheckNetworkCounts(network, 1, 2, 1);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest EntryExcludeSelf",
                 "[ntcore][local-storage]") {
  auto entry =
      storage.GetEntry(fooTopic, NT_DOUBLE, "double", {.excludeSelf = true});
  auto remoteTopic = storage.ServerAnnounce(
      "foo", 0, "double", wpi::util::json::object(), std::nullopt);

  // local set
  storage.SetEntryValue(entry, Value::MakeDouble(1.0, 50));
  CHECK(storage.ReadQueue<double>(entry).empty());

  // network set
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(entry), 2.0, 60));

  CheckNetworkCounts(network, 1, 1, 1);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest ReadQueueInitialLocal",
                 "[ntcore][local-storage]") {
  auto pub = storage.Publish(fooTopic, NT_DOUBLE, "double", {}, {});
  storage.SetEntryValue(pub, Value::MakeDouble(1.0, 50));

  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", DEFAULT_PUB_SUB_OPTIONS);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});

  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subBoth), 1.0, 50));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subLocal), 1.0, 50));
  CHECK(storage.ReadQueue<double>(subRemote).empty());

  CheckNetworkCounts(network, 1, 3, 1);
}

TEST_CASE_METHOD(LocalStorageTest, "LocalStorageTest ReadQueueInitialRemote",
                 "[ntcore][local-storage]") {
  auto remoteTopic = storage.ServerAnnounce(
      "foo", 0, "double", wpi::util::json::object(), std::nullopt);
  storage.ServerSetValue(remoteTopic, Value::MakeDouble(2.0, 60));

  auto subBoth =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", DEFAULT_PUB_SUB_OPTIONS);
  auto subLocal =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableRemote = true});
  auto subRemote =
      storage.Subscribe(fooTopic, NT_DOUBLE, "double", {.disableLocal = true});

  // network set
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subBoth), 2.0, 60));
  CHECK(SingleTimestampedEq(storage.ReadQueue<double>(subRemote), 2.0, 60));
  CHECK(storage.ReadQueue<double>(subLocal).empty());

  CheckNetworkCounts(network, 0, 3, 0);
}

}  // namespace wpi::nt
