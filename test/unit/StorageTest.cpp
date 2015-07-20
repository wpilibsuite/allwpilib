/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"

#include "gtest/gtest.h"

namespace nt {

class StorageTest : public ::testing::Test {
 public:
  void PopulateTrue() {
    storage.SetEntryTypeValue("foo", Value::MakeBoolean(true));
    ASSERT_EQ(1u, updates().size());
    updates().pop();
  }

  Storage::EntriesMap& entries() { return storage.m_entries; }
  Storage::UpdateQueue& updates() { return storage.updates(); }
  Storage storage;
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
  auto entry1 = storage.GetEntry("foo");
  auto entry = storage.FindEntry("foo");
  ASSERT_TRUE(bool(entry));
  ASSERT_EQ(entry1, entry);
  ASSERT_FALSE(storage.FindEntry("bar"));
}

TEST_F(StorageTest, GetEntryNotExist) {
  auto entry = storage.GetEntry("foo");
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
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTest, SetEntryTypeValueAssignTypeChange) {
  PopulateTrue();
  // update with different type results in assignment message
  auto value = Value::MakeDouble(0.0);
  storage.SetEntryTypeValue("foo", value);
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTest, SetEntryTypeValueEqualValue) {
  PopulateTrue();
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  storage.SetEntryTypeValue("foo", value);
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryTypeValueDifferentValue) {
  PopulateTrue();
  // update with same type and different value results in value update message
  auto value = Value::MakeBoolean(false);
  storage.SetEntryTypeValue("foo", value);
  auto entry = storage.GetEntry("foo");
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
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(entry, update.entry);
  EXPECT_EQ(Storage::Update::kAssign, update.kind);
}

TEST_F(StorageTest, SetEntryValueAssignTypeChange) {
  PopulateTrue();
  // update with different type results in error and no message
  auto value = Value::MakeDouble(0.0);
  EXPECT_FALSE(storage.SetEntryValue("foo", value));
  auto entry = storage.GetEntry("foo");
  EXPECT_NE(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryValueEqualValue) {
  PopulateTrue();
  // update with same type and same value: change value contents but no update
  // message is issued (minimizing bandwidth usage)
  auto value = Value::MakeBoolean(true);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(value, entry->value());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryValueDifferentValue) {
  PopulateTrue();
  // update with same type and different value results in value update message
  auto value = Value::MakeBoolean(false);
  EXPECT_TRUE(storage.SetEntryValue("foo", value));
  auto entry = storage.GetEntry("foo");
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

TEST_F(StorageTest, SetEntryFlagsEqualValue) {
  PopulateTrue();
  // update with same value: no update message is issued (minimizing bandwidth
  // usage)
  storage.SetEntryFlags("foo", 0u);
  auto entry = storage.GetEntry("foo");
  EXPECT_EQ(0u, entry->flags());
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, SetEntryFlagsDifferentValue) {
  PopulateTrue();
  // update with different value results in flags update message
  storage.SetEntryFlags("foo", 1u);
  auto entry = storage.GetEntry("foo");
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

TEST_F(StorageTest, GetEntryFlagsExist) {
  PopulateTrue();
  storage.SetEntryFlags("foo", 1u);
  while (!updates().empty()) updates().pop();
  ASSERT_EQ(1u, storage.GetEntryFlags("foo"));
}

TEST_F(StorageTest, DeleteEntryNotExist) {
  storage.DeleteEntry("foo");
  ASSERT_TRUE(updates().empty());
}

TEST_F(StorageTest, DeleteEntryExist) {
  PopulateTrue();
  auto entry = storage.GetEntry("foo");
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

TEST_F(StorageTest, DeleteAllEntries) {
  PopulateTrue();
  storage.DeleteAllEntries();
  ASSERT_TRUE(entries().empty());
  ASSERT_EQ(1u, updates().size());
  auto update = updates().pop();
  EXPECT_EQ(nullptr, update.entry);
  EXPECT_EQ(Storage::Update::kDeleteAll, update.kind);
}

}  // namespace nt
