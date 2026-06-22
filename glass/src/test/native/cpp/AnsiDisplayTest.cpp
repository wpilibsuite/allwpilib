// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/AnsiDisplay.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mrc/AnsiDisplayState.h"

namespace {
struct TestRun {
  size_t row = 0;
  size_t column = 0;
  size_t columns = 0;
  std::string text;
  mrc::AnsiDisplayStyle style;
};

class TestRunVisitor : public mrc::AnsiDisplayRunVisitor {
 public:
  void StartRun(size_t row, size_t column, size_t columns,
                const mrc::AnsiDisplayStyle& style) override {
    auto& run = runs.emplace_back();
    run.row = row;
    run.column = column;
    run.columns = columns;
    run.style = style;
  }

  void AppendRunText(std::string_view text) override {
    runs.back().text += text;
  }

  std::vector<TestRun> runs;
};
}  // namespace

TEST_CASE("AnsiDisplayTest SnapshotsPlainText", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("Hello\nWorld");

  CHECK(model.GetPlainText() == "Hello\nWorld");
}

TEST_CASE("AnsiDisplayTest ClearAnsiRemovesOldCells", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("old");
  model.Append("\x1b[2J\x1b[Hnew");

  CHECK(model.GetPlainText() == "new");
}

TEST_CASE("AnsiDisplayTest SnapshotTracksMaxColumns", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("short\n123456");

  auto snapshot = model.GetSnapshot();
  CHECK(snapshot->lines.size() == 2u);
  CHECK(snapshot->maxColumns == 6u);

  model.Append("\x1b[2J\x1b[Hok");

  auto nextSnapshot = model.GetSnapshot();
  auto* reusableSnapshotAddress = nextSnapshot.get();
  CHECK(nextSnapshot.get() != snapshot.get());
  CHECK(nextSnapshot->lines.size() == 1u);
  CHECK(nextSnapshot->maxColumns == 2u);
  CHECK(snapshot->lines.size() == 2u);
  CHECK(snapshot->maxColumns == 6u);

  nextSnapshot.reset();
  snapshot.reset();
  model.Append("\x1b[2J\x1b[Habc");

  auto reusedSnapshot = model.GetSnapshot();
  CHECK(reusedSnapshot.get() == reusableSnapshotAddress);
  CHECK(reusedSnapshot->lines.size() == 1u);
  CHECK(reusedSnapshot->maxColumns == 3u);
}

TEST_CASE("AnsiDisplayTest EraseDisplayDoesNotMoveCursor", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("abc\x1b[2Jd");

  CHECK(model.GetPlainText() == "   d");
}

TEST_CASE("AnsiDisplayTest CursorRewritesPreserveVisibleCells",
          "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("abc\rZ");

  CHECK(model.GetPlainText() == "Zbc");
}

TEST_CASE("AnsiDisplayTest Utf8TextAdvancesOneCell", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append(
      "\xc3\xa9"
      "A\rB");

  CHECK(model.GetPlainText() == "BA");
}

TEST_CASE("AnsiDisplayTest SgrStyledRunsAreSegmented", "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("\x1b[1;31;44mX\x1b[0mY");

  auto snapshot = model.GetSnapshot();
  const auto& lines = snapshot->lines;
  REQUIRE(lines.size() == 1u);
  REQUIRE(lines[0].segments.size() == 2u);
  CHECK(lines[0].segments[0].startColumn == 0u);
  CHECK(lines[0].segments[0].columns == 1u);
  CHECK(lines[0].segments[0].text == "X");
  CHECK(lines[0].segments[0].style.bold);
  CHECK(lines[0].segments[0].style.hasForeground);
  CHECK(lines[0].segments[0].style.foreground ==
        IM_COL32(0x80, 0x00, 0x00, 255));
  CHECK(lines[0].segments[0].style.hasBackground);
  CHECK(lines[0].segments[0].style.background ==
        IM_COL32(0x00, 0x00, 0x80, 255));
  CHECK(lines[0].segments[1].text == "Y");
  CHECK_FALSE(lines[0].segments[1].style.hasForeground);
  CHECK_FALSE(lines[0].segments[1].style.hasBackground);
}

TEST_CASE("AnsiDisplayTest SgrColorResetRestoresDefaultStyle",
          "[ansi-display]") {
  wpi::glass::AnsiDisplayModel model;

  model.Append("A\x1b[31;44mB\x1b[39;49mC");

  auto snapshot = model.GetSnapshot();
  const auto& lines = snapshot->lines;
  REQUIRE(lines.size() == 1u);
  REQUIRE(lines[0].segments.size() == 3u);
  CHECK(lines[0].segments[0].text == "A");
  CHECK_FALSE(lines[0].segments[0].style.hasForeground);
  CHECK_FALSE(lines[0].segments[0].style.hasBackground);
  CHECK(lines[0].segments[1].text == "B");
  CHECK(lines[0].segments[1].style.hasForeground);
  CHECK(lines[0].segments[1].style.hasBackground);
  CHECK(lines[0].segments[2].text == "C");
  CHECK(lines[0].segments[0].style == lines[0].segments[2].style);
}

TEST_CASE("AnsiDisplayStateTest VisitRunsExposesRendererNeutralStyledCells",
          "[ansi-display-state]") {
  mrc::AnsiDisplayState state;

  state.Apply("\x1b[1;38;5;208;48;2;1;2;3mX\x1b[0mY");

  TestRunVisitor visitor;
  state.VisitRuns(visitor);
  REQUIRE(visitor.runs.size() == 2u);
  CHECK(visitor.runs[0].row == 0u);
  CHECK(visitor.runs[0].column == 0u);
  CHECK(visitor.runs[0].columns == 1u);
  CHECK(visitor.runs[0].text == "X");
  CHECK(visitor.runs[0].style.Bold);
  CHECK(visitor.runs[0].style.Foreground.ColorKind ==
        mrc::AnsiDisplayColor::Kind::Indexed256);
  CHECK(visitor.runs[0].style.Foreground.Index == 208);
  CHECK(visitor.runs[0].style.Background.ColorKind ==
        mrc::AnsiDisplayColor::Kind::Rgb);
  CHECK(visitor.runs[0].style.Background.Red == 1);
  CHECK(visitor.runs[0].style.Background.Green == 2);
  CHECK(visitor.runs[0].style.Background.Blue == 3);

  CHECK(visitor.runs[1].column == 1u);
  CHECK(visitor.runs[1].text == "Y");
  CHECK(visitor.runs[1].style.Foreground.ColorKind ==
        mrc::AnsiDisplayColor::Kind::Default);
  CHECK(visitor.runs[1].style.Background.ColorKind ==
        mrc::AnsiDisplayColor::Kind::Default);
}

TEST_CASE("AnsiDisplayStateTest SnapshotReplayResetsReceiverStyle",
          "[ansi-display-state]") {
  mrc::AnsiDisplayState source;
  source.Apply("plain");

  mrc::AnsiDisplayState receiver;
  receiver.Apply("\x1b[1;31m");
  receiver.Apply(source.BuildSnapshot());

  TestRunVisitor visitor;
  receiver.VisitRuns(visitor);
  REQUIRE(visitor.runs.size() == 1u);
  CHECK(visitor.runs[0].text == "plain");
  CHECK(visitor.runs[0].style == mrc::AnsiDisplayStyle{});
}

TEST_CASE("AnsiDisplayStateTest Utf8SplitAcrossApplyCallsWritesOneCell",
          "[ansi-display-state]") {
  mrc::AnsiDisplayState state;

  state.Apply("\xc3");
  state.Apply("\xa9\rX");

  TestRunVisitor visitor;
  state.VisitRuns(visitor);
  REQUIRE(visitor.runs.size() == 1u);
  CHECK(visitor.runs[0].columns == 1u);
  CHECK(visitor.runs[0].text == "X");
}
