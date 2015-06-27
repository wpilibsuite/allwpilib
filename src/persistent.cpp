/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <fstream>
#include <string>
#include <tuple>

#include "llvm/StringExtras.h"
#include "base64.h"
#include "Storage.h"

using namespace ntimpl;

static void WriteString(std::ostream& os, llvm::StringRef str) {
  os << '"';
  for (auto c : str) {
    switch (c) {
      case '\\':
        os << "\\\\";
        break;
      case '\t':
        os << "\\t";
        break;
      case '\n':
        os << "\\n";
        break;
      case '"':
        os << "\\\"";
        break;
      default:
        if (std::isprint(c)) {
          os << c;
          break;
        }

        // Write out the escaped representation.
        os << "\\x";
        os << llvm::hexdigit((c >> 4) & 0xF);
        os << llvm::hexdigit((c >> 0) & 0xF);
    }
  }
  os << '"';
}

const char* NT_SavePersistent(const char* filename) {
  std::ofstream os(filename);
  if (!os) return "could not open file";

  os << "[NetworkTables Storage 3.0]\n";

  const Storage& storage = Storage::GetInstance();
  for (auto i = storage.entries.begin(), end = storage.entries.end();
       i != end; ++i) {
    const StorageEntry& entry = i->getValue();
    // only write persistent-flagged values
    if ((entry.flags & NT_PERSISTENT) == 0) continue;

    // type
    const NT_Value& v = entry.value;
    switch (v.type) {
      case NT_BOOLEAN:
        os << "boolean ";
        break;
      case NT_DOUBLE:
        os << "double ";
        break;
      case NT_STRING:
        os << "string ";
        break;
      case NT_RAW:
        os << "raw ";
        break;
      case NT_BOOLEAN_ARRAY:
        os << "array boolean ";
        break;
      case NT_DOUBLE_ARRAY:
        os << "array double ";
        break;
      case NT_STRING_ARRAY:
        os << "array string ";
        break;
      default:
        continue;
    }

    // name
    WriteString(os, i->getKey());

    // =
    os << '=';

    // value
    switch (v.type) {
      case NT_BOOLEAN:
        os << (v.data.v_boolean ? "true" : "false");
        break;
      case NT_DOUBLE:
        os << v.data.v_double;
        break;
      case NT_STRING:
        WriteString(os, MakeStringRef(v.data.v_string));
        break;
      case NT_RAW: {
        char* buf = new char[Base64EncodeLen(v.data.v_raw.len)];
        Base64Encode(buf,
                     reinterpret_cast<const unsigned char*>(v.data.v_raw.str),
                     v.data.v_raw.len);
        os << buf;
        delete[] buf;
        break;
      }
      case NT_BOOLEAN_ARRAY:
        for (size_t i = 0; i < v.data.arr_boolean.size; ++i) {
          os << (v.data.arr_boolean.arr[i] ? "true" : "false");
          if (i != (v.data.arr_boolean.size - 1))
            os << ',';
        }
        break;
      case NT_DOUBLE_ARRAY:
        for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          os << v.data.arr_double.arr[i];
          if (i != (v.data.arr_double.size - 1))
            os << ',';
        }
        break;
      case NT_STRING_ARRAY:
        for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          WriteString(os, MakeStringRef(v.data.arr_string.arr[i]));
          if (i != (v.data.arr_double.size - 1))
            os << ',';
        }
        break;
      default:
        break;
    }

    // eol
    os << '\n';
  }

  os.close();
  return 0;
}

const char* NT_LoadPersistent(const char* filename,
                              void (*warn)(size_t line, const char* msg)) {
  std::ifstream f(filename);
  if (!f) return "could not open file";

  std::string line_str;
  std::size_t line_num = 0;
  while (std::getline(f, line_str)) {
    llvm::StringRef line(line_str);
    ++line_num;

    // type
    llvm::StringRef type_str;
    std::tie(type_str, line) = line.split(' ');
    NT_Type type = NT_UNASSIGNED;
    if (type_str == "boolean") type = NT_BOOLEAN;
    else if (type_str == "double") type = NT_DOUBLE;
    else if (type_str == "string") type = NT_STRING;
    else if (type_str == "raw") type = NT_RAW;
    else if (type_str == "array") {
      llvm::StringRef array_str;
      std::tie(array_str, line) = line.split(' ');
      if (array_str == "boolean") type = NT_BOOLEAN_ARRAY;
      else if (array_str == "double") type = NT_DOUBLE_ARRAY;
      else if (array_str == "string") type = NT_STRING_ARRAY;
    }
    if (type == NT_UNASSIGNED) {
      if (warn) warn(line_num, "unrecognized type");
      continue;
    }

    // name

  }

  f.close();
  return 0;
}
