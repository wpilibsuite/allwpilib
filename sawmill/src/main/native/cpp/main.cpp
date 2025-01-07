// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/argparse.h>
#include <wpi/fs.h>

#include "DataLogCSVWriter.h"
#include "DataLogJSONWriter.h"
#include "LogLoader.h"

void export_json(fs::path log_path, fs::path output_path) {
  datalogcli::LogLoader loader{};
  loader.Load(log_path.string());
  std::vector<wpi::log::DataLogRecord> records = loader.GetAllRecords();
  datalogcli::DataLogJSONWriter writer{};
  writer.ExportJSON(output_path, records);
}

void export_csv(fs::path log_path, fs::path output_path) {}

void write_json(std::vector<wpi::log::DataLogRecord> records,
                fs::path output_path) {}

void write_csv(std::vector<wpi::log::DataLogRecord> records,
               fs::path output_path) {
  datalogcli::DataLogCSVWriter writer{};
  writer.ExportCsv(output_path.string(), 0);
}

void extract_entry(std::string_view entry_name, fs::path log_path,
                   fs::path output_path, bool use_json) {
  // represent entry as a list of records
  datalogcli::LogLoader loader{};
  loader.Load(log_path.string());
  std::vector<wpi::log::DataLogRecord> records = loader.GetRecords(entry_name);
}

int main(int argc, char* argv[]) {
  wpi::ArgumentParser cli{"datalogcli"};
  std::string jsonMode{};

  wpi::ArgumentParser export_json_command{"json"};
  export_json_command.add_description(
      "Export a JSON representation of a DataLog file");
  export_json_command.add_argument("log_file")
      .help("Path to the DataLog file to export");
  export_json_command.add_argument("export_path")
      .help(
          "Path of the JSON file to create with the exported data. If it "
          "exists, it will be overwritten.");
  export_json_command.add_argument("-m", "--mode")
      .help(
          "How to format the exported JSON. 'direct' means that each record "
          "will be directly converted to a JSON dictionary and exported, "
          "including control records. 'direct_data' is the same as direct, but "
          "control records will be ommitted. The default is 'direct_data'.")
      .default_value("direct_data")
      .store_into(jsonMode);

  wpi::ArgumentParser export_csv_command{"csv"};
  export_csv_command.add_description(
      "Export a CSV representation of a DataLog file");
  export_csv_command.add_argument("log-file")
      .help("The DataLog file to export");
  export_csv_command.add_argument("output-file")
      .help(
          "The CSV file to create with the exported data. If it "
          "exists, it will be overwritten.");

  wpi::ArgumentParser extract_entry_command{"extract"};
  extract_entry_command.add_description(
      "Extract the history of one entry from a DataLog file and store it in a "
      "JSON or CSV file");
  extract_entry_command.add_argument("-e", "--entry")
      .required()
      .help("The entry to extract from the Log");
  extract_entry_command.add_argument("-l", "--log-file")
      .required()
      .help("The DataLog file to extract from");
  extract_entry_command.add_argument("--time-start")
      .help("The timestamp to start extracting at");
  extract_entry_command.add_argument("-o", "--output")
      .required()
      .help(
          "The file to export the field and data to. It will be created or "
          "overwritten if it already exists");

  cli.add_subparser(export_json_command);
  cli.add_subparser(export_csv_command);
  cli.add_subparser(extract_entry_command);

  try {
    cli.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << cli;
    return 1;
  }

  // see which one was called
  if (export_json_command) {
    // validate paths
    fs::path logPath{export_json_command.get("--log-file")};
    if (logPath.extension() != ".wpilog") {
      std::cerr << "Please use a valid DataLog (.wpilog) file." << std::endl;
      return 1;
    }

    fs::path outputPath{export_json_command.get("--output")};
    if (outputPath.extension() != ".csv" || outputPath.extension() != ".json") {
      std::cerr
          << "Only JSON and CSV are currently supported as output formats."
          << std::endl;
      return 1;
    }

    export_json(logPath, outputPath);
  } else if (export_csv_command) {
    // validate paths
    fs::path logPath{export_csv_command.get("--log-file")};
    if (logPath.extension() != ".wpilog") {
      std::cerr << "Please use a valid DataLog (.wpilog) file." << std::endl;
      return 1;
    }

    fs::path outputPath{export_csv_command.get("--output")};
    if (outputPath.extension() != ".csv" || outputPath.extension() != ".json") {
      std::cerr
          << "Only JSON and CSV are currently supported as output formats."
          << std::endl;
      return 1;
    }

    export_csv(logPath, outputPath);
  } else if (extract_entry_command) {
    // validate paths
    fs::path logPath{extract_entry_command.get("--log-file")};
    if (logPath.extension() != ".wpilog") {
      std::cerr << "Please use a valid DataLog (.wpilog) file." << std::endl;
      return 1;
    }

    fs::path outputPath{extract_entry_command.get("--output")};
    if (outputPath.extension() != ".csv" || outputPath.extension() != ".json") {
      std::cerr
          << "Only JSON and CSV are currently supported as output formats."
          << std::endl;
      return 1;
    }

    bool json{outputPath.extension() == ".json" ? true : false};

    extract_entry(extract_entry_command.get("--entry"), logPath, outputPath,
                  json);
  }
}
