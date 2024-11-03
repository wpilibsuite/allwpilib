#!/usr/bin/env python3

import os
import shutil

from upstream_utils import Lib, copy_to, walk_if

protobuf_lite_sources = set(
    [
        "google/protobuf/any_lite.cc",
        "google/protobuf/arena.cc",
        "google/protobuf/arenastring.cc",
        "google/protobuf/arenaz_sampler.cc",
        "google/protobuf/extension_set.cc",
        "google/protobuf/generated_enum_util.cc",
        "google/protobuf/generated_message_tctable_lite.cc",
        "google/protobuf/generated_message_util.cc",
        "google/protobuf/implicit_weak_message.cc",
        "google/protobuf/inlined_string_field.cc",
        "google/protobuf/io/coded_stream.cc",
        "google/protobuf/io/io_win32.cc",
        "google/protobuf/io/strtod.cc",
        "google/protobuf/io/zero_copy_stream.cc",
        "google/protobuf/io/zero_copy_stream_impl.cc",
        "google/protobuf/io/zero_copy_stream_impl_lite.cc",
        "google/protobuf/map.cc",
        "google/protobuf/message_lite.cc",
        "google/protobuf/parse_context.cc",
        "google/protobuf/repeated_field.cc",
        "google/protobuf/repeated_ptr_field.cc",
        "google/protobuf/stubs/bytestream.cc",
        "google/protobuf/stubs/common.cc",
        "google/protobuf/stubs/int128.cc",
        "google/protobuf/stubs/status.cc",
        "google/protobuf/stubs/statusor.cc",
        "google/protobuf/stubs/stringpiece.cc",
        "google/protobuf/stubs/stringprintf.cc",
        "google/protobuf/stubs/structurally_valid.cc",
        "google/protobuf/stubs/strutil.cc",
        "google/protobuf/stubs/time.cc",
        "google/protobuf/wire_format_lite.cc",
    ]
)

protobuf_lite_includes = set(
    [
        "google/protobuf/any.h",
        "google/protobuf/arena.h",
        "google/protobuf/arena_impl.h",
        "google/protobuf/arenastring.h",
        "google/protobuf/arenaz_sampler.h",
        "google/protobuf/endian.h",
        "google/protobuf/explicitly_constructed.h",
        "google/protobuf/extension_set.h",
        "google/protobuf/extension_set_inl.h",
        "google/protobuf/generated_enum_util.h",
        "google/protobuf/generated_message_tctable_decl.h",
        "google/protobuf/generated_message_tctable_impl.h",
        "google/protobuf/generated_message_util.h",
        "google/protobuf/has_bits.h",
        "google/protobuf/implicit_weak_message.h",
        "google/protobuf/inlined_string_field.h",
        "google/protobuf/io/coded_stream.h",
        "google/protobuf/io/io_win32.h",
        "google/protobuf/io/strtod.h",
        "google/protobuf/io/zero_copy_stream.h",
        "google/protobuf/io/zero_copy_stream_impl.h",
        "google/protobuf/io/zero_copy_stream_impl_lite.h",
        "google/protobuf/map.h",
        "google/protobuf/map_entry_lite.h",
        "google/protobuf/map_field_lite.h",
        "google/protobuf/map_type_handler.h",
        "google/protobuf/message_lite.h",
        "google/protobuf/metadata_lite.h",
        "google/protobuf/parse_context.h",
        "google/protobuf/port.h",
        "google/protobuf/repeated_field.h",
        "google/protobuf/repeated_ptr_field.h",
        "google/protobuf/stubs/bytestream.h",
        "google/protobuf/stubs/callback.h",
        "google/protobuf/stubs/casts.h",
        "google/protobuf/stubs/common.h",
        "google/protobuf/stubs/hash.h",
        "google/protobuf/stubs/logging.h",
        "google/protobuf/stubs/macros.h",
        "google/protobuf/stubs/map_util.h",
        "google/protobuf/stubs/mutex.h",
        "google/protobuf/stubs/once.h",
        "google/protobuf/stubs/platform_macros.h",
        "google/protobuf/stubs/port.h",
        "google/protobuf/stubs/status.h",
        "google/protobuf/stubs/stl_util.h",
        "google/protobuf/stubs/stringpiece.h",
        "google/protobuf/stubs/strutil.h",
        "google/protobuf/stubs/template_util.h",
        "google/protobuf/wire_format_lite.h",
    ]
)

protobuf_sources = set(
    [
        "google/protobuf/any.cc",
        "google/protobuf/any.pb.cc",
        "google/protobuf/api.pb.cc",
        "google/protobuf/compiler/importer.cc",
        "google/protobuf/compiler/parser.cc",
        "google/protobuf/descriptor.cc",
        "google/protobuf/descriptor.pb.cc",
        "google/protobuf/descriptor_database.cc",
        "google/protobuf/duration.pb.cc",
        "google/protobuf/dynamic_message.cc",
        "google/protobuf/empty.pb.cc",
        "google/protobuf/extension_set_heavy.cc",
        "google/protobuf/field_mask.pb.cc",
        "google/protobuf/generated_message_bases.cc",
        "google/protobuf/generated_message_reflection.cc",
        "google/protobuf/generated_message_tctable_full.cc",
        "google/protobuf/io/gzip_stream.cc",
        "google/protobuf/io/printer.cc",
        "google/protobuf/io/tokenizer.cc",
        "google/protobuf/map_field.cc",
        "google/protobuf/message.cc",
        "google/protobuf/reflection_ops.cc",
        "google/protobuf/service.cc",
        "google/protobuf/source_context.pb.cc",
        "google/protobuf/struct.pb.cc",
        "google/protobuf/stubs/substitute.cc",
        "google/protobuf/text_format.cc",
        "google/protobuf/timestamp.pb.cc",
        "google/protobuf/type.pb.cc",
        "google/protobuf/unknown_field_set.cc",
        "google/protobuf/util/delimited_message_util.cc",
        "google/protobuf/util/field_comparator.cc",
        "google/protobuf/util/field_mask_util.cc",
        "google/protobuf/util/internal/datapiece.cc",
        "google/protobuf/util/internal/default_value_objectwriter.cc",
        "google/protobuf/util/internal/error_listener.cc",
        "google/protobuf/util/internal/field_mask_utility.cc",
        "google/protobuf/util/internal/json_escaping.cc",
        "google/protobuf/util/internal/json_objectwriter.cc",
        "google/protobuf/util/internal/json_stream_parser.cc",
        "google/protobuf/util/internal/object_writer.cc",
        "google/protobuf/util/internal/proto_writer.cc",
        "google/protobuf/util/internal/protostream_objectsource.cc",
        "google/protobuf/util/internal/protostream_objectwriter.cc",
        "google/protobuf/util/internal/type_info.cc",
        "google/protobuf/util/internal/utility.cc",
        "google/protobuf/util/json_util.cc",
        "google/protobuf/util/message_differencer.cc",
        "google/protobuf/util/time_util.cc",
        "google/protobuf/util/type_resolver_util.cc",
        "google/protobuf/wire_format.cc",
        "google/protobuf/wrappers.pb.cc",
    ]
)

protobuf_includes = set(
    [
        "google/protobuf/any.pb.h",
        "google/protobuf/api.pb.h",
        "google/protobuf/compiler/importer.h",
        "google/protobuf/compiler/parser.h",
        "google/protobuf/descriptor.h",
        "google/protobuf/descriptor.pb.h",
        "google/protobuf/descriptor_database.h",
        "google/protobuf/duration.pb.h",
        "google/protobuf/dynamic_message.h",
        "google/protobuf/empty.pb.h",
        "google/protobuf/field_access_listener.h",
        "google/protobuf/field_mask.pb.h",
        "google/protobuf/generated_enum_reflection.h",
        "google/protobuf/generated_message_bases.h",
        "google/protobuf/generated_message_reflection.h",
        "google/protobuf/io/gzip_stream.h",
        "google/protobuf/io/printer.h",
        "google/protobuf/io/tokenizer.h",
        "google/protobuf/map_entry.h",
        "google/protobuf/map_field.h",
        "google/protobuf/map_field_inl.h",
        "google/protobuf/message.h",
        "google/protobuf/metadata.h",
        "google/protobuf/reflection.h",
        "google/protobuf/reflection_internal.h",
        "google/protobuf/reflection_ops.h",
        "google/protobuf/service.h",
        "google/protobuf/source_context.pb.h",
        "google/protobuf/struct.pb.h",
        "google/protobuf/text_format.h",
        "google/protobuf/timestamp.pb.h",
        "google/protobuf/type.pb.h",
        "google/protobuf/unknown_field_set.h",
        "google/protobuf/util/delimited_message_util.h",
        "google/protobuf/util/field_comparator.h",
        "google/protobuf/util/field_mask_util.h",
        "google/protobuf/util/json_util.h",
        "google/protobuf/util/message_differencer.h",
        "google/protobuf/util/time_util.h",
        "google/protobuf/util/type_resolver.h",
        "google/protobuf/util/type_resolver_util.h",
        "google/protobuf/wire_format.h",
        "google/protobuf/wrappers.pb.h",
    ]
)

protobuf_internal_includes = set(
    [
        "google/protobuf/port_def.inc",
        "google/protobuf/port_undef.inc",
        "google/protobuf/stubs/int128.h",
        "google/protobuf/stubs/mathutil.h",
        "google/protobuf/stubs/statusor.h",
        "google/protobuf/stubs/status_macros.h",
        "google/protobuf/stubs/stringprintf.h",
        "google/protobuf/stubs/substitute.h",
        "google/protobuf/stubs/time.h",
        "google/protobuf/util/internal/constants.h",
        "google/protobuf/util/internal/datapiece.h",
        "google/protobuf/util/internal/default_value_objectwriter.h",
        "google/protobuf/util/internal/error_listener.h",
        "google/protobuf/util/internal/field_mask_utility.h",
        "google/protobuf/util/internal/json_escaping.h",
        "google/protobuf/util/internal/json_objectwriter.h",
        "google/protobuf/util/internal/json_stream_parser.h",
        "google/protobuf/util/internal/location_tracker.h",
        "google/protobuf/util/internal/object_location_tracker.h",
        "google/protobuf/util/internal/object_source.h",
        "google/protobuf/util/internal/object_writer.h",
        "google/protobuf/util/internal/proto_writer.h",
        "google/protobuf/util/internal/protostream_objectsource.h",
        "google/protobuf/util/internal/protostream_objectwriter.h",
        "google/protobuf/util/internal/structured_objectwriter.h",
        "google/protobuf/util/internal/type_info.h",
        "google/protobuf/util/internal/utility.h",
    ]
)

use_src_files = protobuf_lite_sources | protobuf_sources
use_include_files = (
    protobuf_lite_includes | protobuf_includes | protobuf_internal_includes
)


def matches(dp, f, files):
    if not dp.startswith(os.path.join(".", "src")):
        return False
    p = dp[6:] + "/" + f
    return p in files


def copy_upstream_src(wpilib_root):
    upstream_root = os.path.abspath(".")
    wpiutil = os.path.join(wpilib_root, "wpiutil")

    # Delete old install
    for d in [
        "src/main/native/thirdparty/protobuf/src",
        "src/main/native/thirdparty/protobuf/include",
    ]:
        shutil.rmtree(os.path.join(wpiutil, d), ignore_errors=True)

    # Copy protobuf source files into allwpilib
    src_files = walk_if(".", lambda dp, f: matches(dp, f, use_src_files))
    src_files = [f[22:] for f in src_files]
    os.chdir(os.path.join(upstream_root, "src/google/protobuf"))
    copy_to(src_files, os.path.join(wpiutil, "src/main/native/thirdparty/protobuf/src"))

    # Copy protobuf header files into allwpilib
    os.chdir(upstream_root)
    include_files = walk_if(".", lambda dp, f: matches(dp, f, use_include_files))
    include_files = [f[6:] for f in include_files]
    os.chdir(os.path.join(upstream_root, "src"))
    copy_to(
        include_files,
        os.path.join(wpiutil, "src/main/native/thirdparty/protobuf/include"),
    )


def main():
    name = "protobuf"
    url = "https://github.com/protocolbuffers/protobuf"
    tag = "v3.21.12"

    protobuf = Lib(name, url, tag, copy_upstream_src)
    protobuf.main()


if __name__ == "__main__":
    main()
