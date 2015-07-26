/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"

#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace nt {

class StorageTest : public ::testing::Test {
 public:
  StorageTest() {
    storage.EnableUpdates();
  }
  Storage::EntriesMap& entries() { return storage.m_entries; }
  Storage::UpdateQueue& updates() { return storage.updates(); }
  std::shared_ptr<StorageEntry> GetEntry(StringRef name) {
    auto& entry = storage.m_entries[name];
    if (!entry) entry = std::make_shared<StorageEntry>();
    return entry;
  }
  Storage storage;
};

class StorageTestPopulateOne : public StorageTest {
 public:
  StorageTestPopulateOne() {
    storage.SetEntryTypeValue("foo", Value::MakeBoolean(true));
    while (!updates().empty()) updates().pop();
  }
};

class StorageTestPopulated : public StorageTest {
 public:
  StorageTestPopulated() {
    storage.SetEntryTypeValue("foo", Value::MakeBoolean(true));
    storage.SetEntryTypeValue("foo2", Value::MakeDouble(0.0));
    storage.SetEntryTypeValue("bar", Value::MakeDouble(1.0));
    storage.SetEntryTypeValue("bar2", Value::MakeBoolean(false));
    while (!updates().empty()) updates().pop();
  }
};

class StorageTestPersistent : public StorageTest {
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
    while (!updates().empty()) updates().pop();
  }
};

class MockLoadWarn {
 public:
  MOCK_METHOD2(Warn, void(std::size_t line, const char* msg));
};

TEST_F(StorageTest, Construct) {
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, FindEntryNotExist) {
  ASSERT_FALSE(storage.FindEntry("foo"));
  ASSERT_TRUE(entries().empty());
}

TEST_F(StorageTest, FindEntryExist) {
  auto entry1 = GetEntry("foo");
  auto entry = storage.FindEntry("foo");
  ASSERT_TRUE(bool(entry));
  ASSERT_EQ(entry1, entry);
  ASSERT_FALSE(storage.FindEntry("bar"));
}

TEST_F(StorageTest, StorageEntryInit) {
  auto entry = GetEntry("foo");
  ASSERT_TRUE(bool(entry));
  ASSERT_EQ(1u, entries().size());

  EXPECT_FALSE(entry->value());
  EXPECT_EQ(0u, entry->flags());
  EXPECT_EQ(0xffffu, entry->id());
  EXPECT_EQ(SequenceNumber(), entry->seq_num());
}

TEST_F(StorageTest, GetEntryValueNotExist) {
  ASSERT_FALSE(storage.GetEntryValue("foo"));
  ASSERT_TRUE(entries().empty());
}

TEST_F(StorageTest, GetEntryValueExist) {
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  ASSERT_EQ(1u, updates().size());
  updates().pop();
  ASSERT_EQ(value, storage.GetEntryValue("foo"));
}

TEST_F(StorageTest, SetEntryTypeValueAssignNew) {
  // brand new entry
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTestPopulateOne, SetEntryTypeValueAssignTypeChange) {
  // update with different type results in assignment message
  auto value = Value::MakeDouble(0.0);
  storage.SetEntryTypeValue("foo", value);
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTestPopulateOne, SetEntryTypeValueEqualValue) {
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, SetEntryTypeValueDifferentValue) {
  // update with same type and different value results in value update message
  auto value = Value::MakeBoolean(false);
  storage.SetEntryTypeValue("foo", value);
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kValueUpdate, update.kind);
}

TEST_F(StorageTest, SetEntryTypeValueEmptyName) {
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("", value);
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryTypeValueEmptyValue) {
  storage.SetEntryTypeValue("foo", nullptr);
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryValueAssignNew) {
  // brand new entry
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTestPopulateOne, SetEntryValueAssignTypeChange) {
  // update with different type results in error and no message
  auto value = Value::MakeDouble(0.0);
  EXPECT_FALSE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_NE(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, SetEntryValueEqualValue) {
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, SetEntryValueDifferentValue) {
  // update with same type and different value results in value update message
  auto value = Value::MakeBoolean(false);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kValueUpdate, update.kind);
}

TEST_F(StorageTest, SetEntryValueEmptyName) {
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("", value));
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryValueEmptyValue) {
  EXPECT_TRUE(storage.SetEntryValue("foo", nullptr));
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryFlagsNew) {
  // flags setting doesn't create an entry
  storage.SetEntryFlags("foo", 0u);
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, SetEntryFlagsEqualValue) {
  // update with same value: no update message is issued (minimizing bandwidth
  // usage)
  storage.SetEntryFlags("foo", 0u);
  auto entry = GetEntry("foo");
  EXPECT_EQ(0u, entry->flags());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, SetEntryFlagsDifferentValue) {
  // update with different value results in flags update message
  storage.SetEntryFlags("foo", 1u);
  auto entry = GetEntry("foo");
  EXPECT_EQ(1u, entry->flags());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kFlagsUpdate, update.kind);
}

TEST_F(StorageTest, SetEntryFlagsEmptyName) {
  storage.SetEntryFlags("", 0u);
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, GetEntryFlagsNotExist) {
  ASSERT_EQ(0u, storage.GetEntryFlags("foo"));
  ASSERT_TRUE(entries().empty());
}

TEST_F(StorageTestPopulateOne, GetEntryFlagsExist) {
  storage.SetEntryFlags("foo", 1u);
  while (!updates().empty()) updates().pop();
  ASSERT_EQ(1u, storage.GetEntryFlags("foo"));
}

TEST_F(StorageTest, DeleteEntryNotExist) {
  storage.DeleteEntry("foo");
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulateOne, DeleteEntryExist) {
  auto entry = GetEntry("foo");
  storage.DeleteEntry("foo");
  ASSERT_TRUE(entries().empty());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kDelete, update.kind);
}

TEST_F(StorageTest, DeleteAllEntriesEmpty) {
  storage.DeleteAllEntries();
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTestPopulated, DeleteAllEntries) {
  storage.DeleteAllEntries();
  ASSERT_TRUE(entries().empty());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(nullptr, update.entry);
  EXPECT_EQ(Storage::Update::kDeleteAll, update.kind);
}

TEST_F(StorageTestPopulated, GetEntryInfoAll) {
  auto info = storage.GetEntryInfo("", 0u);
  ASSERT_EQ(4u, info.size());
}

TEST_F(StorageTestPopulated, GetEntryInfoPrefix) {
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

TEST_F(StorageTestPopulated, GetEntryInfoTypes) {
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

TEST_F(StorageTestPopulated, GetEntryInfoPrefixTypes) {
  auto info = storage.GetEntryInfo("bar", NT_BOOLEAN);
  ASSERT_EQ(1u, info.size());
  EXPECT_EQ("bar2", info[0].name);
  EXPECT_EQ(NT_BOOLEAN, info[0].type);
}

TEST_F(StorageTestPersistent, SavePersistentEmpty) {
  std::ostringstream oss;
  storage.SavePersistent(oss);
  ASSERT_EQ("[NetworkTables Storage 3.0]\n", oss.str());
}

TEST_F(StorageTestPersistent, SavePersistent) {
  for (auto& i : entries()) i.getValue()->set_flags(NT_PERSISTENT);
  std::ostringstream oss;
  storage.SavePersistent(oss);
  std::string out = oss.str();
  //fputs(out.c_str(), stderr);
  llvm::StringRef line, rem = out;
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("[NetworkTables Storage 3.0]", line);
  std::tie(line, rem) = rem.split('\n');
  ASSERT_EQ("boolean \"\\x00\\x03\\x05\\n\"=true", line);
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
  ASSERT_EQ("double \"double/big\"=1.3e+08", line);
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

TEST_F(StorageTest, LoadPersistentBadHeader) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss("");
  EXPECT_CALL(warn, Warn(1, "header line mismatch, ignoring rest of file"));
  EXPECT_FALSE(storage.LoadPersistent(iss, warn_func));

  std::istringstream iss2("[NetworkTables");
  EXPECT_CALL(warn, Warn(1, "header line mismatch, ignoring rest of file"));
  EXPECT_FALSE(storage.LoadPersistent(iss2, warn_func));
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, LoadPersistentCommentHeader) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "\n; comment\n# comment\n[NetworkTables Storage 3.0]\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, LoadPersistentEmptyName) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"\"=true\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, LoadPersistentAssign) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=true\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo");
  EXPECT_EQ(*Value::MakeBoolean(true), *entry->value());
  EXPECT_EQ(NT_PERSISTENT, entry->flags());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTestPopulateOne, LoadPersistentUpdateFlags) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=true\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo");
  EXPECT_EQ(*Value::MakeBoolean(true), *entry->value());
  EXPECT_EQ(NT_PERSISTENT, entry->flags());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kFlagsUpdate, update.kind);
}

TEST_F(StorageTestPopulateOne, LoadPersistentUpdateValue) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  GetEntry("foo")->set_flags(NT_PERSISTENT);
  while (!updates().empty()) updates().pop();

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=false\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo");
  EXPECT_EQ(*Value::MakeBoolean(false), *entry->value());
  EXPECT_EQ(NT_PERSISTENT, entry->flags());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kValueUpdate, update.kind);
}

TEST_F(StorageTestPopulateOne, LoadPersistentUpdateValueFlags) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=false\n");
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));
  auto entry = GetEntry("foo");
  EXPECT_EQ(*Value::MakeBoolean(false), *entry->value());
  EXPECT_EQ(NT_PERSISTENT, entry->flags());
  ASSERT_EQ(2u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kValueUpdate, update.kind);
  update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kFlagsUpdate, update.kind);
}

TEST_F(StorageTest, LoadPersistent) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::string in = "[NetworkTables Storage 3.0]\n";
  in += "boolean \"\\x00\\x03\\x05\\n\"=true\n";
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
  ASSERT_EQ(21u, entries().size());
  ASSERT_EQ(21u, updates().size());

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
}

TEST_F(StorageTest, LoadPersistentWarn) {
  MockLoadWarn warn;
  auto warn_func =
      [&](std::size_t line, const char* msg) { warn.Warn(line, msg); };

  std::istringstream iss(
      "[NetworkTables Storage 3.0]\nboolean \"foo\"=foo\n");
  EXPECT_CALL(warn,
              Warn(2, "unrecognized boolean value, not 'true' or 'false'"));
  EXPECT_TRUE(storage.LoadPersistent(iss, warn_func));

  ASSERT_TRUE(entries().empty());
  ASSERT_TRUE(updates().empty());
}

}  // namespace nt
