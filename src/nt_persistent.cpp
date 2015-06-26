/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <cstdio>

#include "llvm/StringExtras.h"
#include "nt_base64.h"
#include "nt_storage.h"

using namespace ntimpl;

static void write_string(FILE* f, llvm::StringRef str) {
  fputc('"', f);
  for (unsigned int i = 0, e = str.size(); i != e; ++i) {
    unsigned char c = str[i];

    switch (c) {
      case '\\':
        fputs("\\\\", f);
        break;
      case '\t':
        fputs("\\t", f);
        break;
      case '\n':
        fputs("\\n", f);
        break;
      case '"':
        fputs("\\\"", f);
        break;
      default:
        if (std::isprint(c)) {
          fputc(c, f);
          break;
        }

        // Write out the escaped representation.
        fputs("\\x", f);
        fputc(llvm::hexdigit((c >> 4 & 0xF)), f);
        fputc(llvm::hexdigit((c >> 0) & 0xF), f);
    }
  }
  fputc('"', f);
}

const char* NT_SavePersistent(const char* filename) {
  FILE* f = fopen(filename, "wt");
  if (!f) return "could not open file";

  fputs("[NetworkTables Storage 3.0]\n", f);

  Storage& storage = Storage::GetInstance();
  for (Storage::EntriesMap::const_iterator i = storage.entries.begin(),
                                           end = storage.entries.end();
       i != end; ++i) {
    const StorageEntry& entry = i->getValue();
    // only write persistent-flagged values
    if ((entry.flags & NT_PERSISTENT) == 0) continue;

    // type
    const NT_Value& v = entry.value;
    switch (v.type) {
      case NT_BOOLEAN:
        fputs("boolean ", f);
        break;
      case NT_DOUBLE:
        fputs("double ", f);
        break;
      case NT_STRING:
        fputs("string ", f);
        break;
      case NT_RAW:
        fputs("raw ", f);
        break;
      case NT_BOOLEAN_ARRAY:
        fputs("array boolean ", f);
        break;
      case NT_DOUBLE_ARRAY:
        fputs("array double ", f);
        break;
      case NT_STRING_ARRAY:
        fputs("array string ", f);
        break;
      default:
        continue;
    }

    // name
    write_string(f, i->getKey());

    // =
    fputc('=', f);

    // value
    switch (v.type) {
      case NT_BOOLEAN:
        fputs(v.data.v_boolean ? "true" : "false", f);
        break;
      case NT_DOUBLE:
        fprintf(f, "%g", v.data.v_double);
        break;
      case NT_STRING:
        write_string(f, make_StringRef(v.data.v_string));
        break;
      case NT_RAW: {
        char* buf = new char[base64encode_len(v.data.v_raw.len)];
        base64encode(buf,
                     reinterpret_cast<const unsigned char*>(v.data.v_raw.str),
                     v.data.v_raw.len);
        fputs(buf, f);
        delete[] buf;
        break;
      }
      case NT_BOOLEAN_ARRAY:
        for (size_t i = 0; i < v.data.arr_boolean.size; ++i) {
          fputs(v.data.arr_boolean.arr[i] ? "true" : "false", f);
          if (i != (v.data.arr_boolean.size - 1)) fputc(',', f);
        }
        break;
      case NT_DOUBLE_ARRAY:
        for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          fprintf(f, "%g", v.data.arr_double.arr[i]);
          if (i != (v.data.arr_double.size - 1)) fputc(',', f);
        }
        break;
      case NT_STRING_ARRAY:
        for (size_t i = 0; i < v.data.arr_double.size; ++i) {
          write_string(f, make_StringRef(v.data.arr_string.arr[i]));
          if (i != (v.data.arr_double.size - 1)) fputc(',', f);
        }
        break;
      default:
        break;
    }

    // eol
    fputc('\n', f);
  }

  fclose(f);
  return 0;
}

const char* NT_LoadPersistent(const char* filename) {
  FILE* f = fopen(filename, "rt");
  if (!f) return "could not open file";

  fclose(f);
  return 0;
}
