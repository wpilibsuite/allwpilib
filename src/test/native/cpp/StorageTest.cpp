/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"
#include "StorageTest.h"

#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace nt {

class StorageTestEmpty : public StorageTest,
                         public ::testing::TestWithParam<bool> {
 public:
  StorageTestEmpty() { HookOutgoing(GetParam()); }
};

class StorageTestPopulateOne : public StorageTestEmpty {
 public:
  StorageTestPopulateOne() {
    storage.SetEntryTypeValue("foo", Value::MakeBoolean(true));
    outgoing.clear();
  }
};

class StorageTestPopulated : public StorageTestEmpty {
 public:
  StorageTestPopulated() {
    storage.SetEntryTypeValue("foo", Value::MakeBoolean(true));
    storage.SetEntryTypeValue("foo2", Value::MakeDouble(0.0));
    storage.SetEntryTypeValue("bar", Value::MakeDouble(1.0));
    storage.SetEntryTypeValue("bar2", Value::MakeBoolean(false));
    outgoing.clear();
  }
};

class StorageTestPersistent : public StorageTestEmpty {
 public:
  StorageTestPersistent() {
    storage.SetEntryTypeValue("boolean/true", Value::MakeBoolean(true));
    storage.SetEntryTypeValue("boolean/false", Value::MakeBoolean(false));
    storage.SetEntryTypeValue("double/neg", Value::MakeDouble(-1.5));
    storage.SetEntryTypeValue("double/zero", Value::MakeDouble(0.0));
    storage.SetEntryTypeValue("double/big", Value::MakeDouble(1.3e8));
    storage.SetEntryTypeValue("string/empty", Value::MakeString(""));
    storage.SetEntryTypeValue("string/normal", Value::MakeString("hello"));
    storage.SetEntryTypeValue("string/special",
                              Value::MakeString(StringRef("\0\3\5\n", 4)));
    storage.SetEntryTypeValue("raw/empty", Value::MakeRaw(""));
    storage.SetEntryTypeValue("raw/normal", Value::MakeRaw("hello"));
    storage.SetEntryTypeValue("raw/special",
                              Value::MakeRaw(StringRef("\0\3\5\n", 4)));
    storage.SetEntryTypeValue("booleanarr/empty",
                              Value::MakeBooleanArray(std::vector<int>{}));
    storage.SetEntryTypeValue("booleanarr/one",
                              Value::MakeBooleanArray(std::vector<int>{1}));
    storage.SetEntryTypeValue("booleanarr/two",
                              Value::MakeBooleanArray(std::vector<int>{1, 0}));
    storage.SetEntryTypeValue("doublearr/empty",
                              Value::MakeDoubleArray(std::vector<double>{}));
    storage.SetEntryTypeValue("doublearr/one",
                              Value::MakeDoubleArray(std::vector<double>{0.5}));
    storage.SetEntryTypeValue(
        "doublearr/two",
        Value::MakeDoubleArray(std::vector<double>{0.5, -0.25}));
    storage.SetEntryTypeValue(
        "stringarr/empty", Value::MakeStringArray(std::vector<std::string>{}));
    storage.SetEntryTypeValue(
        "stringarr/one",
        Value::MakeStringArray(std::vector<std::string>{"hello"}));
    storage.SetEntryTypeValue(
        "stringarr/two",
        Value::MakeStringArray(std::vector<std::string>{"hello", "world\n"}));
    storage.SetEntryTypeValue(StringRef("\0\3\5\n", 4),
                              Value::MakeBoolean(true));
    storage.SetEntryTypeValue("=", Value::MakeBoolean(true));
    outgoing.clear();
  }
};

class MockLoadWarn {
 public:
  MOCK_METHOD2(Warn, void(std::size_t line, llvm::StringRef msg));
};

TEST_P(StorageTestEmpty, Construct) {
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
}

TEST_P(StorageTestEmpty, StorageEntryInit) {
  auto entry = GetEntry("foo");
  EXPECT_FALSE(entry->value);
  EXPECT_EQ(0u, entry->flags);
  EXPECT_EQ("foobar", entry->name);  // since GetEntry uses the tmp_entry.
  EXPECT_EQ(0xffffu, entry->id);
  EXPECT_EQ(SequenceNumber(), entry->seq_num);
}

TEST_P(StorageTestEmpty, GetEntryValueNotExist) {
  EXPECT_FALSE(storage.GetEntryValue("foo"));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, GetEntryValueExist) {
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  outgoing.clear();
  EXPECT_EQ(value, storage.GetEntryValue("foo"));
}

TEST_P(StorageTestEmpty, SetEntryTypeValueAssignNew) {
  // brand new entry
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  EXPECT_EQ(value, GetEntry("foo")->value);
  if (GetParam()) {
    ASSERT_EQ(1u, idmap().size());
    EXPECT_EQ(value, idmap()[0]->value);
  } else {
    EXPECT_TRUE(idmap().empty());
  }

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kEntryAssign, msg->type());
  EXPECT_EQ("foo", msg->str());
  if (GetParam())
    EXPECT_EQ(0u, msg->id());  // assigned as server
  else
    EXPECT_EQ(0xffffu, msg->id());  // not assigned as client
  EXPECT_EQ(1u, msg->seq_num_uid());
  EXPECT_EQ(value, msg->value());
  EXPECT_EQ(0u, msg->flags());
}

TEST_P(StorageTestPopulateOne, SetEntryTypeValueAssignTypeChange) {
  // update with different type results in assignment message
  auto value = Value::MakeDouble(0.0);
  storage.SetEntryTypeValue("foo", value);
  EXPECT_EQ(value, GetEntry("foo")->value);

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kEntryAssign, msg->type());
  EXPECT_EQ("foo", msg->str());
  if (GetParam())
    EXPECT_EQ(0u, msg->id());  // assigned as server
  else
    EXPECT_EQ(0xffffu, msg->id());  // not assigned as client
  EXPECT_EQ(2u, msg->seq_num_uid());  // incremented
  EXPECT_EQ(value, msg->value());
  EXPECT_EQ(0u, msg->flags());
}

TEST_P(StorageTestPopulateOne, SetEntryTypeValueEqualValue) {
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  EXPECT_EQ(value, GetEntry("foo")->value);
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, SetEntryTypeValueDifferentValue) {
  // update with same type and different value results in value update message
  auto value = Value::MakeDouble(1.0);
  storage.SetEntryTypeValue("foo2", value);
  EXPECT_EQ(value, GetEntry("foo2")->value);

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kEntryUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(2u, msg->seq_num_uid());  // incremented
    EXPECT_EQ(value, msg->value());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
    EXPECT_EQ(2u, GetEntry("foo2")->seq_num.value());  // still should be incremented
  }
}

TEST_P(StorageTestEmpty, SetEntryTypeValueEmptyName) {
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("", value);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetEntryTypeValueEmptyValue) {
  storage.SetEntryTypeValue("foo", nullptr);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetEntryValueAssignNew) {
  // brand new entry
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  EXPECT_EQ(value, GetEntry("foo")->value);

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kEntryAssign, msg->type());
  EXPECT_EQ("foo", msg->str());
  if (GetParam())
    EXPECT_EQ(0u, msg->id());  // assigned as server
  else
    EXPECT_EQ(0xffffu, msg->id());  // not assigned as client
  EXPECT_EQ(0u, msg->seq_num_uid());
  EXPECT_EQ(value, msg->value());
  EXPECT_EQ(0u, msg->flags());
}

TEST_P(StorageTestPopulateOne, SetEntryValueAssignTypeChange) {
  // update with different type results in error and no message
  auto value = Value::MakeDouble(0.0);
  EXPECT_FALSE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_NE(value, entry->value);
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulateOne, SetEntryValueEqualValue) {
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value);
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, SetEntryValueDifferentValue) {
  // update with same type and different value results in value update message
  auto value = Value::MakeDouble(1.0);
  EXPECT_TRUE(storage.SetEntryValue("foo2", value));
  auto entry = GetEntry("foo2");
  EXPECT_EQ(value, entry->value);

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kEntryUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(2u, msg->seq_num_uid());  // incremented
    EXPECT_EQ(value, msg->value());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
    EXPECT_EQ(2u, GetEntry("foo2")->seq_num.value());  // still should be incremented
  }
}

TEST_P(StorageTestEmpty, SetEntryValueEmptyName) {
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("", value));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetEntryValueEmptyValue) {
  EXPECT_TRUE(storage.SetEntryValue("foo", nullptr));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetDefaultEntryAssignNew) {
  // brand new entry
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("foo", value);
  EXPECT_TRUE(ret_val);
  EXPECT_EQ(value, GetEntry("foo")->value);

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kEntryAssign, msg->type());
  EXPECT_EQ("foo", msg->str());
  if (GetParam())
    EXPECT_EQ(0u, msg->id());  // assigned as server
  else
    EXPECT_EQ(0xffffu, msg->id());  // not assigned as client
  EXPECT_EQ(0u, msg->seq_num_uid());
  EXPECT_EQ(value, msg->value());
  EXPECT_EQ(0u, msg->flags());
}

TEST_P(StorageTestPopulateOne, SetDefaultEntryExistsSameType) {
  // existing entry
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("foo", value);
  EXPECT_TRUE(ret_val);
  EXPECT_NE(value, GetEntry("foo")->value);
  
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulateOne, SetDefaultEntryExistsDifferentType) {
  // existing entry is boolean
  auto value = Value::MakeDouble(2.0);
  auto ret_val = storage.SetDefaultEntryValue("foo", value);
  EXPECT_FALSE(ret_val);
  // should not have updated value in table if it already existed.
  EXPECT_NE(value, GetEntry("foo")->value);
  
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetDefaultEntryEmptyName) {
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("", value);
  EXPECT_FALSE(ret_val);
  auto entry = GetEntry("foo");
  EXPECT_FALSE(entry->value);
  EXPECT_EQ(0u, entry->flags);
  EXPECT_EQ("foobar", entry->name);  // since GetEntry uses the tmp_entry.
  EXPECT_EQ(0xffffu, entry->id);
  EXPECT_EQ(SequenceNumber(), entry->seq_num);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, SetDefaultEntryEmptyValue) {
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("", nullptr);
  EXPECT_FALSE(ret_val);
  auto entry = GetEntry("foo");
  EXPECT_FALSE(entry->value);
  EXPECT_EQ(0u, entry->flags);
  EXPECT_EQ("foobar", entry->name);  // since GetEntry uses the tmp_entry.
  EXPECT_EQ(0xffffu, entry->id);
  EXPECT_EQ(SequenceNumber(), entry->seq_num);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, SetDefaultEntryEmptyName) {
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("", value);
  EXPECT_FALSE(ret_val);
  // assert that no entries get added
  EXPECT_EQ(4u, entries().size());
  if (GetParam())
    EXPECT_EQ(4u, idmap().size());
  else
    EXPECT_EQ(0u, idmap().size());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, SetDefaultEntryEmptyValue) { 
  auto value = Value::MakeBoolean(true);
  auto ret_val = storage.SetDefaultEntryValue("", nullptr);
  EXPECT_FALSE(ret_val);
  // assert that no entries get added
  EXPECT_EQ(4u, entries().size());
  if (GetParam())
    EXPECT_EQ(4u, idmap().size());
  else
    EXPECT_EQ(0u, idmap().size());
  EXPECT_TRUE(outgoing.empty());
}


TEST_P(StorageTestEmpty, SetEntryFlagsNew) {
  // flags setting doesn't create an entry
  storage.SetEntryFlags("foo", 0u);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulateOne, SetEntryFlagsEqualValue) {
  // update with same value: no update message is issued (minimizing bandwidth
  // usage)
  storage.SetEntryFlags("foo", 0u);
  auto entry = GetEntry("foo");
  EXPECT_EQ(0u, entry->flags);
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, SetEntryFlagsDifferentValue) {
  // update with different value results in flags update message
  storage.SetEntryFlags("foo2", 1u);
  EXPECT_EQ(1u, GetEntry("foo2")->flags);

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kFlagsUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(1u, msg->flags());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
  }
}

TEST_P(StorageTestEmpty, SetEntryFlagsEmptyName) {
  storage.SetEntryFlags("", 0u);
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, GetEntryFlagsNotExist) {
  EXPECT_EQ(0u, storage.GetEntryFlags("foo"));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulateOne, GetEntryFlagsExist) {
  storage.SetEntryFlags("foo", 1u);
  outgoing.clear();
  EXPECT_EQ(1u, storage.GetEntryFlags("foo"));
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, DeleteEntryNotExist) {
  storage.DeleteEntry("foo");
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, DeleteEntryExist) {
  storage.DeleteEntry("foo2");
  EXPECT_TRUE(entries().count("foo2") == 0);
  if (GetParam()) {
    ASSERT_TRUE(idmap().size() >= 2);
    EXPECT_FALSE(idmap()[1]);
  }

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kEntryDelete, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
  }
}

TEST_P(StorageTestEmpty, DeleteAllEntriesEmpty) {
  storage.DeleteAllEntries();
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestPopulated, DeleteAllEntries) {
  storage.DeleteAllEntries();
  ASSERT_TRUE(entries().empty());

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kClearEntries, msg->type());
}

TEST_P(StorageTestPopulated, DeleteAllEntriesPersistent) {
  GetEntry("foo2")->flags = NT_PERSISTENT;
  storage.DeleteAllEntries();
  ASSERT_EQ(1u, entries().size());
  EXPECT_EQ(1u, entries().count("foo2"));

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kClearEntries, msg->type());
}

TEST_P(StorageTestPopulated, GetEntryInfoAll) {
  auto info = storage.GetEntryInfo("", 0u);
  ASSERT_EQ(4u, info.size());
}

TEST_P(StorageTestPopulated, GetEntryInfoPrefix) {
  auto info = storage.GetEntryInfo("foo", 0u);
  ASSERT_EQ(2u, info.size());
  if (info[0].name == "foo") {
    EXPECT_EQ("foo", info[0].name);
    EXPECT_EQ(NT_BOOLEAN, info[0].type);
    EXPECT_EQ("foo2", info[1].name);
    EXPECT_EQ(NT_DOUBLE, info[1].type);
  } else {
    EXPECT_EQ("foo2", info[0].name);
    EXPECT_EQ(NT_DOUBLE, info[0].type);
    EXPECT_EQ("foo", info[1].name);
    EXPECT_EQ(NT_BOOLEAN, info[1].type);
  }
}

TEST_P(StorageTestPopulated, GetEntryInfoTypes) {
  auto info = storage.GetEntryInfo("", NT_DOUBLE);
  ASSERT_EQ(2u, info.size());
  EXPECT_EQ(NT_DOUBLE, info[0].type);
  EXPECT_EQ(NT_DOUBLE, info[1].type);
  if (info[0].name == "foo2") {
    EXPECT_EQ("foo2", info[0].name);
    EXPECT_EQ("bar", info[1].name);
  } else {
    EXPECT_EQ("bar", info[0].name);
    EXPECT_EQ("foo2", info[1].name);
  }
}

TEST_P(StorageTestPopulated, GetEntryInfoPrefixTypes) {
  auto info = storage.GetEntryInfo("bar", NT_BOOLEAN);
  ASSERT_EQ(1u, info.size());
  EXPECT_EQ("bar2", info[0].name);
  EXPECT_EQ(NT_BOOLEAN, info[0].type);
}

TEST_P(StorageTestPersistent, SavePersistentEmpty) {
  std::ostringstream oss;
  storage.SavePersistent(oss, false);
  ASSERT_EQ("[NetworkTables Storage 3.0]\n", oss.str());
}

TEST_P(StorageTestPersistent, SavePersistent) {
  for (auto& i : entries()) i.getValue()->flags = NT_PERSISTENT;
  std::ostringstream oss;
  storage.SavePersistent(oss, false);
  std::string out = oss.str();
  //fputs(out.c_str(), stderr);
  llvm::StringRef line, rem = out;
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("[NetworkTables Storage 3.0]", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("boolean \"\\x00\\x03\\x05\\n\"=true", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("boolean \"\\x3D\"=true", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("boolean \"boolean/false\"=false", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("boolean \"boolean/true\"=true", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array boolean \"booleanarr/empty\"=", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array boolean \"booleanarr/one\"=true", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array boolean \"booleanarr/two\"=true,false", line);
  std::tie(line, rem) = rem.split('\n');
#if defined(_MSC_VER) && _MSC_VER < 1900
  ASSERT_EQ("double \"double/big\"=1.3e+008", line);
#else
  ASSERT_EQ("double \"double/big\"=1.3e+08", line);
#endif
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("double \"double/neg\"=-1.5", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("double \"double/zero\"=0", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array double \"doublearr/empty\"=", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array double \"doublearr/one\"=0.5", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array double \"doublearr/two\"=0.5,-0.25", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("raw \"raw/empty\"=", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("raw \"raw/normal\"=aGVsbG8=", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("raw \"raw/special\"=AAMFCg==", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("string \"string/empty\"=\"\"", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("string \"string/normal\"=\"hello\"", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("string \"string/special\"=\"\\x00\\x03\\x05\\n\"", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array string \"stringarr/empty\"=", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array string \"stringarr/one\"=\"hello\"", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("array string \"stringarr/two\"=\"hello\",\"world\\n\"", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("", line);
}

TEST_P(StorageTestEmpty, LoadPersistentBadHeader) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss("");
  EXPECT_CALL(warn, Warn(1, llvm::StringRef("header line mismatch, ignoring rest of file")));
  EXPECT_FALSE(storage.LoadPersistent(iss, warn_func));

  std::istringstream iss2("[NetworkTables");
  EXPECT_CALL(warn, Warn(1, llvm::StringRef("header line mismatch, ignoring rest of file")));
  EXPECT_FALSE(storage.LoadPersistent(iss2, warn_func));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, LoadPersistentCommentHeader) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "\n; comment\n# comment\n[NetworkTables Storage 3.0]\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, LoadPersistentEmptyName) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"\"=true\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

TEST_P(StorageTestEmpty, LoadPersistentAssign) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=true\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo");
  EXPECT_EQ(*Value::MakeBoolean(true), *entry->value);
  EXPECT_EQ(NT_PERSISTENT, entry->flags);

  ASSERT_EQ(1u, outgoing.size());
  EXPECT_FALSE(outgoing[0].only);
  EXPECT_FALSE(outgoing[0].except);
  auto msg = outgoing[0].msg;
  EXPECT_EQ(Message::kEntryAssign, msg->type());
  EXPECT_EQ("foo", msg->str());
  if (GetParam())
    EXPECT_EQ(0u, msg->id());  // assigned as server
  else
    EXPECT_EQ(0xffffu, msg->id());  // not assigned as client
  EXPECT_EQ(1u, msg->seq_num_uid());
  EXPECT_EQ(*Value::MakeBoolean(true), *msg->value());
  EXPECT_EQ(NT_PERSISTENT, msg->flags());
}

TEST_P(StorageTestPopulated, LoadPersistentUpdateFlags) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\ndouble \"foo2\"=0.0\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo2");
  EXPECT_EQ(*Value::MakeDouble(0.0), *entry->value);
  EXPECT_EQ(NT_PERSISTENT, entry->flags);

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kFlagsUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(NT_PERSISTENT, msg->flags());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
  }
}

TEST_P(StorageTestPopulated, LoadPersistentUpdateValue) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  GetEntry("foo2")->flags = NT_PERSISTENT;

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\ndouble \"foo2\"=1.0\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo2");
  EXPECT_EQ(*Value::MakeDouble(1.0), *entry->value);
  EXPECT_EQ(NT_PERSISTENT, entry->flags);

  if (GetParam()) {
    ASSERT_EQ(1u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kEntryUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(2u, msg->seq_num_uid());  // incremented
    EXPECT_EQ(*Value::MakeDouble(1.0), *msg->value());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
    EXPECT_EQ(2u, GetEntry("foo2")->seq_num.value());  // still should be incremented
  }
}

TEST_P(StorageTestPopulated, LoadPersistentUpdateValueFlags) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\ndouble \"foo2\"=1.0\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo2");
  EXPECT_EQ(*Value::MakeDouble(1.0), *entry->value);
  EXPECT_EQ(NT_PERSISTENT, entry->flags);

  if (GetParam()) {
    ASSERT_EQ(2u, outgoing.size());
    EXPECT_FALSE(outgoing[0].only);
    EXPECT_FALSE(outgoing[0].except);
    auto msg = outgoing[0].msg;
    EXPECT_EQ(Message::kEntryUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(2u, msg->seq_num_uid());  // incremented
    EXPECT_EQ(*Value::MakeDouble(1.0), *msg->value());

    EXPECT_FALSE(outgoing[1].only);
    EXPECT_FALSE(outgoing[1].except);
    msg = outgoing[1].msg;
    EXPECT_EQ(Message::kFlagsUpdate, msg->type());
    EXPECT_EQ(1u, msg->id());  // assigned as server
    EXPECT_EQ(NT_PERSISTENT, msg->flags());
  } else {
    // shouldn't send an update id not assigned yet (happens on client only)
    EXPECT_TRUE(outgoing.empty());
    EXPECT_EQ(2u, GetEntry("foo2")->seq_num.value());  // still should be incremented
  }
}

TEST_P(StorageTestEmpty, LoadPersistent) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::string in = "[NetworkTables Storage 3.0]\n";
  in += "boolean \"\\x00\\x03\\x05\\n\"=true\n";
  in += "boolean \"\\x3D\"=true\n";
  in += "boolean \"boolean/false\"=false\n";
  in += "boolean \"boolean/true\"=true\n";
  in += "array boolean \"booleanarr/empty\"=\n";
  in += "array boolean \"booleanarr/one\"=true\n";
  in += "array boolean \"booleanarr/two\"=true,false\n";
  in += "double \"double/big\"=1.3e+08\n";
  in += "double \"double/neg\"=-1.5\n";
  in += "double \"double/zero\"=0\n";
  in += "array double \"doublearr/empty\"=\n";
  in += "array double \"doublearr/one\"=0.5\n";
  in += "array double \"doublearr/two\"=0.5,-0.25\n";
  in += "raw \"raw/empty\"=\n";
  in += "raw \"raw/normal\"=aGVsbG8=\n";
  in += "raw \"raw/special\"=AAMFCg==\n";
  in += "string \"string/empty\"=\"\"\n";
  in += "string \"string/normal\"=\"hello\"\n";
  in += "string \"string/special\"=\"\\x00\\x03\\x05\\n\"\n";
  in += "array string \"stringarr/empty\"=\n";
  in += "array string \"stringarr/one\"=\"hello\"\n";
  in += "array string \"stringarr/two\"=\"hello\",\"world\\n\"\n";

  std::istringstream iss(in);
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  ASSERT_EQ(22u, entries().size());
  EXPECT_EQ(22u, outgoing.size());

  EXPECT_EQ(*Value::MakeBoolean(true), *storage.GetEntryValue("boolean/true"));
  EXPECT_EQ(*Value::MakeBoolean(false),
            *storage.GetEntryValue("boolean/false"));
  EXPECT_EQ(*Value::MakeDouble(-1.5), *storage.GetEntryValue("double/neg"));
  EXPECT_EQ(*Value::MakeDouble(0.0), *storage.GetEntryValue("double/zero"));
  EXPECT_EQ(*Value::MakeDouble(1.3e8), *storage.GetEntryValue("double/big"));
  EXPECT_EQ(*Value::MakeString(""), *storage.GetEntryValue("string/empty"));
  EXPECT_EQ(*Value::MakeString("hello"),
            *storage.GetEntryValue("string/normal"));
  EXPECT_EQ(*Value::MakeString(StringRef("\0\3\5\n", 4)),
            *storage.GetEntryValue("string/special"));
  EXPECT_EQ(*Value::MakeRaw(""),
            *storage.GetEntryValue("raw/empty"));
  EXPECT_EQ(*Value::MakeRaw("hello"),
            *storage.GetEntryValue("raw/normal"));
  EXPECT_EQ(*Value::MakeRaw(StringRef("\0\3\5\n", 4)),
            *storage.GetEntryValue("raw/special"));
  EXPECT_EQ(*Value::MakeBooleanArray(std::vector<int>{}),
            *storage.GetEntryValue("booleanarr/empty"));
  EXPECT_EQ(*Value::MakeBooleanArray(std::vector<int>{1}),
            *storage.GetEntryValue("booleanarr/one"));
  EXPECT_EQ(*Value::MakeBooleanArray(std::vector<int>{1, 0}),
            *storage.GetEntryValue("booleanarr/two"));
  EXPECT_EQ(*Value::MakeDoubleArray(std::vector<double>{}),
            *storage.GetEntryValue("doublearr/empty"));
  EXPECT_EQ(*Value::MakeDoubleArray(std::vector<double>{0.5}),
            *storage.GetEntryValue("doublearr/one"));
  EXPECT_EQ(*Value::MakeDoubleArray(std::vector<double>{0.5, -0.25}),
            *storage.GetEntryValue("doublearr/two"));
  EXPECT_EQ(*Value::MakeStringArray(std::vector<std::string>{}),
            *storage.GetEntryValue("stringarr/empty"));
  EXPECT_EQ(*Value::MakeStringArray(std::vector<std::string>{"hello"}),
            *storage.GetEntryValue("stringarr/one"));
  EXPECT_EQ(
      *Value::MakeStringArray(std::vector<std::string>{"hello", "world\n"}),
      *storage.GetEntryValue("stringarr/two"));
  EXPECT_EQ(*Value::MakeBoolean(true),
            *storage.GetEntryValue(StringRef("\0\3\5\n", 4)));
  EXPECT_EQ(*Value::MakeBoolean(true), *storage.GetEntryValue("="));
}

TEST_P(StorageTestEmpty, LoadPersistentWarn) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=foo\n");
  EXPECT_CALL(warn,
              Warn(2, llvm::StringRef("unrecognized boolean value, not 'true' or 'false'")));
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));

  EXPECT_TRUE(entries().empty());
  EXPECT_TRUE(idmap().empty());
  EXPECT_TRUE(outgoing.empty());
}

INSTANTIATE_TEST_CASE_P(StorageTestsEmpty, StorageTestEmpty, ::testing::Bool());
INSTANTIATE_TEST_CASE_P(StorageTestsPopulateOne, StorageTestPopulateOne,
                        ::testing::Bool());
INSTANTIATE_TEST_CASE_P(StorageTestsPopulated, StorageTestPopulated,
                        ::testing::Bool());
INSTANTIATE_TEST_CASE_P(StorageTestsPersistent, StorageTestPersistent,
                        ::testing::Bool());

}  // namespace nt
