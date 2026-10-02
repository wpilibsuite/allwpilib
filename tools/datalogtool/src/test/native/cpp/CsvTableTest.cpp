// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CsvTable.hpp"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/raw_ostream.hpp"

TEST_CASE("CsvTable merges through the inclusive fuzziness boundary",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 3, 1'000'000};

  table.Add(1'000'000'000, 0, "10");
  table.Add(1'000'500'000, 1, "20");
  table.Add(1'001'000'000, 2, "30");
  CHECK(csv.empty());
  table.Add(1'001'001'000, 1, "40");
  table.Flush();

  CHECK(csv == "1,10,20,30\n1.001001,,40,\n");
}

TEST_CASE("CsvTable zero fuzziness merges only identical timestamps",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 2, 0};

  table.Add(0, 0, "10");
  table.Add(0, 1, "20");
  table.Add(1'000, 0, "30");
  table.Flush();

  CHECK(csv == "0,10,20\n1e-06,30,\n");
}

TEST_CASE("CsvTable does not chain nearby timestamps beyond the limit",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 3, 1'000'000};

  table.Add(1'000'000'000, 0, "10");
  table.Add(1'000'750'000, 1, "20");
  table.Add(1'001'500'000, 2, "30");
  table.Flush();

  CHECK(csv == "1,10,20,\n1.0015,,,30\n");
}

TEST_CASE("CsvTable uses the last value added to each column",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 2, 1'000'000};

  table.Add(1'000'000'000, 0, "10");
  table.Add(1'000'500'000, 1, "20");
  table.Add(1'001'000'000, 0, "30");
  table.Flush();
  table.Flush();

  CHECK(csv == "1,30,20\n");
}

TEST_CASE("CsvTable bounds the span of out of order timestamps",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 3, 1'000'000};

  table.Add(1'000'500'000, 0, "10");
  table.Add(1'000'000'000, 1, "20");
  table.Add(1'001'000'000, 2, "30");
  table.Add(999'999'000, 0, "40");
  table.Flush();

  CHECK(csv == "1,10,20,30\n0.999999,40,,\n");
}

TEST_CASE("CsvTable preserves escaped values and empty columns",
          "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 4, 1'000'000};

  table.Add(1'000'000'000, 1, "\"a,\"\"b\"\"\ntext\"");
  table.Add(1'000'500'000, 2, "\"\"");
  table.Flush();
  table.Add(2'000'000'000, 3, "42");
  table.Flush();

  CHECK(csv == "1,,\"a,\"\"b\"\"\ntext\",\"\",\n2,,,,42\n");
}

TEST_CASE("CsvTable does not write empty rows", "[datalogtool]") {
  std::string csv;
  wpi::util::raw_string_ostream os{csv};
  CsvTable table{os, 0, 1'000'000};

  table.Flush();

  CHECK(csv.empty());
}
