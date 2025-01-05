#include <cstring>
#include <string>
#include <string_view>
#include "wpi/argparse.h"

void export_json(std::string_view output_path) {

}

void export_csv(std::string_view output_path) {

}

void extract_field(std::string_view field_name, std::string_view output_path, bool use_json) {

}

void open_log(std::string_view log_path) {
  // TODO: Add Log reader (base it on SysId?)
}

int main(int argc, char* argv[]) {
  wpi::ArgumentParser cli{"wpilog-cli"};

  wpi::ArgumentParser export_json_command{"json"};
  export_json_command.add_description(
      "Export a JSON representation of a WPILOG file");
  export_json_command.add_argument("log_file")
      .help("Path to the WPILOG file to export");
  export_json_command.add_argument("json_file")
      .help(
          "Path of the JSON file to create with the exported data. If it "
          "exists, it will be overwritten.");

  wpi::ArgumentParser export_csv_command{"csv"};
  export_csv_command.add_description(
      "Export a CSV representation of a WPILOG file");
  export_csv_command.add_argument("-l", "--log-file").help("The WPILOG file to export");
  export_csv_command.add_argument("-o", "--output-file")
      .required()
      .help(
          "The CSV file to create with the exported data. If it "
          "exists, it will be overwritten.");

  wpi::ArgumentParser extract_field_command{"extract"};
  extract_field_command.add_description(
      "Extract the histoy of one field from a WPILOG file and store it in a "
      "JSON or CSV file");
  extract_field_command.add_argument("-f", "--field")
      .required()
      .help("The field to extract from the WPILOG");
  extract_field_command.add_argument("-l", "--log")
      .required()
      .help("The WPILOG file to extract from");
  extract_field_command.add_argument("--time-start")
    .help("The timestamp to start extracting at");
  extract_field_command.add_argument("-o", "--output")
      .required()
      .help(
          "The file to export the field and data to. It will be created or "
          "overwritten if it already exists");
  
  cli.add_subparser(export_json_command);
  cli.add_subparser(export_csv_command);
  cli.add_subparser(extract_field_command);

  try {
    cli.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << cli;
    return 1;
  }

  // see which one was called
  if (export_json_command) {
    //export_json();
  } else if (export_csv_command) {
    export_csv(export_csv_command.get("--log-file"));
  } else if (extract_field_command) {
    //extract_field();
  }
}