// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NameTree.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "NameTreeSupport.hpp"

namespace {

using wpi::filterdesigner::BuildNameTree;
using wpi::filterdesigner::NameTreeItem;
using wpi::filterdesigner::NameTreeNode;
using wpi::filterdesigner::NameTreeNodeMatchesSearch;

TEST_CASE("NameTreeTest LeadingSlashDoesNotMakeAnEmptyRootChild",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/accel/x", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  REQUIRE(root.children.size() == 1);
  const NameTreeNode& accel = root.children[0];
  CHECK(accel.name == "accel");
  CHECK(accel.fullPath.empty());
  REQUIRE(accel.children.size() == 1);
  CHECK(accel.children[0].name == "x");
  CHECK(accel.children[0].fullPath == "/accel/x");
  CHECK(accel.children[0].type == "double");
  CHECK(accel.children[0].selectable);
}

TEST_CASE("NameTreeTest SplitsOnTheFirstColonSoTheNtPrefixIsItsOwnLevel",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{
      {"NT:/Spindexer/VelocityRPM", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* nt = FindChild(root, "NT");
  REQUIRE(nt != nullptr);
  const NameTreeNode* spindexer = FindChild(*nt, "Spindexer");
  REQUIRE(spindexer != nullptr);
  const NameTreeNode* leaf = FindChild(*spindexer, "VelocityRPM");
  REQUIRE(leaf != nullptr);
  // The leaf still names the entry as the log spells it — the split is a
  // display concern and the selection has to round-trip.
  CHECK(leaf->fullPath == "NT:/Spindexer/VelocityRPM");
}

TEST_CASE("NameTreeTest DoesNotSplitOnAColonThatFollowsASlash",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/Shooter/ratio:1", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* shooter = FindChild(root, "Shooter");
  REQUIRE(shooter != nullptr);
  REQUIRE(shooter->children.size() == 1);
  CHECK(shooter->children[0].name == "ratio:1");
}

TEST_CASE("NameTreeTest EntriesSharingAPrefixShareOneBranch",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/drive/left", "double", true},
                                  {"/drive/right", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  REQUIRE(root.children.size() == 1);
  CHECK(root.children[0].name == "drive");
  CHECK(root.children[0].children.size() == 2);
}

TEST_CASE("NameTreeTest ANodeCanBeBothAnEntryAndABranch", "[filterdesigner]") {
  // NT allows a topic at /foo alongside /foo/bar, and a wpilog can carry the
  // same shape. Such a node has to render its own selectable row *and* its
  // subtree.
  std::vector<NameTreeItem> items{{"/foo", "double", true},
                                  {"/foo/bar", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* foo = FindChild(root, "foo");
  REQUIRE(foo != nullptr);
  CHECK(foo->fullPath == "/foo");
  REQUIRE(foo->children.size() == 1);
  CHECK(foo->children[0].fullPath == "/foo/bar");
}

TEST_CASE("NameTreeTest NamesThatSplitAlikeKeepSeparateRows",
          "[filterdesigner]") {
  // "/foo" and "foo" both split to {"foo"}. Each entry must keep a row of
  // its own, or the later one hides the earlier from the picker for good.
  std::vector<NameTreeItem> items{{"/foo", "double", true},
                                  {"foo", "int64", true}};
  NameTreeNode root = BuildNameTree(items);
  REQUIRE(root.children.size() == 2u);
  CHECK(root.children[0].name == "foo");
  CHECK(root.children[1].name == "foo");
  std::vector<std::string> paths{root.children[0].fullPath,
                                 root.children[1].fullPath};
  std::ranges::sort(paths);
  CHECK(paths == std::vector<std::string>{"/foo", "foo"});
}

TEST_CASE("NameTreeTest ABranchStillTakesTheLeafOfItsOwnName",
          "[filterdesigner]") {
  // The sibling rule must not split "/foo" from the branch "/foo/bar" made.
  std::vector<NameTreeItem> items{{"/foo/bar", "double", true},
                                  {"/foo", "double", true}};
  NameTreeNode root = BuildNameTree(items);
  REQUIRE(root.children.size() == 1u);
  CHECK(root.children[0].fullPath == "/foo");
  REQUIRE(root.children[0].children.size() == 1u);
  CHECK(root.children[0].children[0].fullPath == "/foo/bar");
}

TEST_CASE("NameTreeTest IgnoresNamesThatSplitToNothing", "[filterdesigner]") {
  std::vector<NameTreeItem> items{
      {"/", "double", true}, {"", "double", true}, {"//", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  CHECK(root.children.empty());
}

TEST_CASE("NameTreeTest ChildrenAreSortedRegardlessOfInputOrder",
          "[filterdesigner]") {
  // wpilog entries arrive in log order, not name order.
  std::vector<NameTreeItem> items{{"/b/z", "double", true},
                                  {"/a/y", "double", true},
                                  {"/b/a", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  REQUIRE(root.children.size() == 2);
  CHECK(root.children[0].name == "a");
  CHECK(root.children[1].name == "b");
  REQUIRE(root.children[1].children.size() == 2);
  CHECK(root.children[1].children[0].name == "a");
  CHECK(root.children[1].children[1].name == "z");
}

TEST_CASE("NameTreeTest CarriesTheSelectableFlagToTheLeaf",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/a/text", "string", false},
                                  {"/a/value", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* a = FindChild(root, "a");
  REQUIRE(a != nullptr);
  const NameTreeNode* text = FindChild(*a, "text");
  REQUIRE(text != nullptr);
  CHECK_FALSE(text->selectable);
  CHECK(text->type == "string");
  const NameTreeNode* value = FindChild(*a, "value");
  REQUIRE(value != nullptr);
  CHECK(value->selectable);
}

TEST_CASE("NameTreeTest LeavesCacheTheirRenderedRow", "[filterdesigner]") {
  // Built once with the tree, not per frame: the picker draws every surviving
  // row of a 543-entry log each frame the popup is open.
  std::vector<NameTreeItem> items{{"/accel/x", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* accel = FindChild(root, "accel");
  REQUIRE(accel != nullptr);
  UNSCOPED_INFO("a pure branch renders its name, so it needs no label");
  CHECK(accel->label.empty());
  REQUIRE(accel->children.size() == 1);
  CHECK(accel->children[0].label == "x  [double]##/accel/x");
}

TEST_CASE("NameTreeTest RowsThatSplitAlikeGetDistinctIds", "[filterdesigner]") {
  // "/foo" and "foo" land as siblings named "foo" with the same type, so
  // their visible text is identical. ImGui takes a Selectable's id from its
  // label, and two rows sharing an id share their click and focus state.
  std::vector<NameTreeItem> items{{"/foo", "double", true},
                                  {"foo", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  REQUIRE(root.children.size() == 2u);
  CHECK(root.children[0].name == "foo");
  CHECK(root.children[1].name == "foo");
  CHECK(root.children[0].label != root.children[1].label);
  UNSCOPED_INFO("everything before the ## is what the row shows");
  for (const auto& child : root.children) {
    CHECK(child.label.starts_with("foo  [double]##"));
  }
}

TEST_CASE("NameTreeTest EmptySearchMatchesEverything", "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/a/b", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  CHECK(NameTreeNodeMatchesSearch(root.children[0], ""));
}

TEST_CASE("NameTreeTest SearchIsCaseInsensitive", "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/Spindexer/VelocityRPM", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  CHECK(NameTreeNodeMatchesSearch(root.children[0], "velocity"));
  CHECK(NameTreeNodeMatchesSearch(root.children[0], "VELOCITY"));
}

TEST_CASE("NameTreeTest SearchMatchesTheFullPathNotJustTheLeafName",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/Spindexer/VelocityRPM", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* leaf = &root.children[0].children[0];
  CHECK(leaf->name == "VelocityRPM");
  // "spindexer" appears nowhere in the leaf's own name.
  CHECK(NameTreeNodeMatchesSearch(*leaf, "spindexer"));
}

TEST_CASE("NameTreeTest ABranchSurvivesIfAnyDescendantMatches",
          "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/drive/left/current", "double", true},
                                  {"/drive/right/current", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  const NameTreeNode* drive = FindChild(root, "drive");
  REQUIRE(drive != nullptr);
  CHECK(NameTreeNodeMatchesSearch(*drive, "left"));
  const NameTreeNode* right = FindChild(*drive, "right");
  REQUIRE(right != nullptr);
  CHECK_FALSE(NameTreeNodeMatchesSearch(*right, "left"));
}

TEST_CASE("NameTreeTest ANonMatchingSubtreeIsFilteredOut", "[filterdesigner]") {
  std::vector<NameTreeItem> items{{"/drive/left", "double", true}};
  NameTreeNode root = BuildNameTree(items);

  CHECK_FALSE(NameTreeNodeMatchesSearch(root.children[0], "shooter"));
}

TEST_CASE("NameTreeTest DisabledLeavesAreStillSearchable", "[filterdesigner]") {
  // Non-numeric entries stay in the tree, greyed, so the user can see the
  // entry exists rather than wondering where it went.
  std::vector<NameTreeItem> items{{"/a/message", "string", false}};
  NameTreeNode root = BuildNameTree(items);

  CHECK(NameTreeNodeMatchesSearch(root.children[0], "message"));
}

}  // namespace
