#!/usr/bin/env python3

import os
import shutil
from pathlib import Path

from upstream_utils import Lib, walk_cwd_and_copy_if

protobuf_lite_sources = {
    Path("any_lite.cc"),
    Path("arena.cc"),
    Path("arenastring.cc"),
    Path("arenaz_sampler.cc"),
    Path("extension_set.cc"),
    Path("generated_enum_util.cc"),
    Path("generated_message_tctable_lite.cc"),
    Path("generated_message_util.cc"),
    Path("implicit_weak_message.cc"),
    Path("inlined_string_field.cc"),
    Path("io/coded_stream.cc"),
    Path("io/io_win32.cc"),
    Path("io/strtod.cc"),
    Path("io/zero_copy_stream.cc"),
    Path("io/zero_copy_stream_impl.cc"),
    Path("io/zero_copy_stream_impl_lite.cc"),
    Path("map.cc"),
    Path("message_lite.cc"),
    Path("parse_context.cc"),
    Path("repeated_field.cc"),
    Path("repeated_ptr_field.cc"),
    Path("stubs/bytestream.cc"),
    Path("stubs/common.cc"),
    Path("stubs/int128.cc"),
    Path("stubs/status.cc"),
    Path("stubs/statusor.cc"),
    Path("stubs/stringpiece.cc"),
    Path("stubs/stringprintf.cc"),
    Path("stubs/structurally_valid.cc"),
    Path("stubs/strutil.cc"),
    Path("stubs/time.cc"),
    Path("wire_format_lite.cc"),
}

protobuf_lite_includes = {
    Path("google/protobuf/any.h"),
    Path("google/protobuf/arena.h"),
    Path("google/protobuf/arena_impl.h"),
    Path("google/protobuf/arenastring.h"),
    Path("google/protobuf/arenaz_sampler.h"),
    Path("google/protobuf/endian.h"),
    Path("google/protobuf/explicitly_constructed.h"),
    Path("google/protobuf/extension_set.h"),
    Path("google/protobuf/extension_set_inl.h"),
    Path("google/protobuf/generated_enum_util.h"),
    Path("google/protobuf/generated_message_tctable_decl.h"),
    Path("google/protobuf/generated_message_tctable_impl.h"),
    Path("google/protobuf/generated_message_util.h"),
    Path("google/protobuf/has_bits.h"),
    Path("google/protobuf/implicit_weak_message.h"),
    Path("google/protobuf/inlined_string_field.h"),
    Path("google/protobuf/io/coded_stream.h"),
    Path("google/protobuf/io/io_win32.h"),
    Path("google/protobuf/io/strtod.h"),
    Path("google/protobuf/io/zero_copy_stream.h"),
    Path("google/protobuf/io/zero_copy_stream_impl.h"),
    Path("google/protobuf/io/zero_copy_stream_impl_lite.h"),
    Path("google/protobuf/map.h"),
    Path("google/protobuf/map_entry_lite.h"),
    Path("google/protobuf/map_field_lite.h"),
    Path("google/protobuf/map_type_handler.h"),
    Path("google/protobuf/message_lite.h"),
    Path("google/protobuf/metadata_lite.h"),
    Path("google/protobuf/parse_context.h"),
    Path("google/protobuf/port.h"),
    Path("google/protobuf/repeated_field.h"),
    Path("google/protobuf/repeated_ptr_field.h"),
    Path("google/protobuf/stubs/bytestream.h"),
    Path("google/protobuf/stubs/callback.h"),
    Path("google/protobuf/stubs/casts.h"),
    Path("google/protobuf/stubs/common.h"),
    Path("google/protobuf/stubs/hash.h"),
    Path("google/protobuf/stubs/logging.h"),
    Path("google/protobuf/stubs/macros.h"),
    Path("google/protobuf/stubs/map_util.h"),
    Path("google/protobuf/stubs/mutex.h"),
    Path("google/protobuf/stubs/once.h"),
    Path("google/protobuf/stubs/platform_macros.h"),
    Path("google/protobuf/stubs/port.h"),
    Path("google/protobuf/stubs/status.h"),
    Path("google/protobuf/stubs/stl_util.h"),
    Path("google/protobuf/stubs/stringpiece.h"),
    Path("google/protobuf/stubs/strutil.h"),
    Path("google/protobuf/stubs/template_util.h"),
    Path("google/protobuf/wire_format_lite.h"),
}
protobuf_sources = {
    Path("any.cc"),
    Path("any.pb.cc"),
    Path("api.pb.cc"),
    Path("compiler/importer.cc"),
    Path("compiler/parser.cc"),
    Path("descriptor.cc"),
    Path("descriptor.pb.cc"),
    Path("descriptor_database.cc"),
    Path("duration.pb.cc"),
    Path("dynamic_message.cc"),
    Path("empty.pb.cc"),
    Path("extension_set_heavy.cc"),
    Path("field_mask.pb.cc"),
    Path("generated_message_bases.cc"),
    Path("generated_message_reflection.cc"),
    Path("generated_message_tctable_full.cc"),
    Path("io/gzip_stream.cc"),
    Path("io/printer.cc"),
    Path("io/tokenizer.cc"),
    Path("map_field.cc"),
    Path("message.cc"),
    Path("reflection_ops.cc"),
    Path("service.cc"),
    Path("source_context.pb.cc"),
    Path("struct.pb.cc"),
    Path("stubs/substitute.cc"),
    Path("text_format.cc"),
    Path("timestamp.pb.cc"),
    Path("type.pb.cc"),
    Path("unknown_field_set.cc"),
    Path("util/delimited_message_util.cc"),
    Path("util/field_comparator.cc"),
    Path("util/field_mask_util.cc"),
    Path("util/internal/datapiece.cc"),
    Path("util/internal/default_value_objectwriter.cc"),
    Path("util/internal/error_listener.cc"),
    Path("util/internal/field_mask_utility.cc"),
    Path("util/internal/json_escaping.cc"),
    Path("util/internal/json_objectwriter.cc"),
    Path("util/internal/json_stream_parser.cc"),
    Path("util/internal/object_writer.cc"),
    Path("util/internal/proto_writer.cc"),
    Path("util/internal/protostream_objectsource.cc"),
    Path("util/internal/protostream_objectwriter.cc"),
    Path("util/internal/type_info.cc"),
    Path("util/internal/utility.cc"),
    Path("util/json_util.cc"),
    Path("util/message_differencer.cc"),
    Path("util/time_util.cc"),
    Path("util/type_resolver_util.cc"),
    Path("wire_format.cc"),
    Path("wrappers.pb.cc"),
}

protobuf_includes = {
    Path("google/protobuf/any.pb.h"),
    Path("google/protobuf/api.pb.h"),
    Path("google/protobuf/compiler/importer.h"),
    Path("google/protobuf/compiler/parser.h"),
    Path("google/protobuf/descriptor.h"),
    Path("google/protobuf/descriptor.pb.h"),
    Path("google/protobuf/descriptor_database.h"),
    Path("google/protobuf/duration.pb.h"),
    Path("google/protobuf/dynamic_message.h"),
    Path("google/protobuf/empty.pb.h"),
    Path("google/protobuf/field_access_listener.h"),
    Path("google/protobuf/field_mask.pb.h"),
    Path("google/protobuf/generated_enum_reflection.h"),
    Path("google/protobuf/generated_message_bases.h"),
    Path("google/protobuf/generated_message_reflection.h"),
    Path("google/protobuf/io/gzip_stream.h"),
    Path("google/protobuf/io/printer.h"),
    Path("google/protobuf/io/tokenizer.h"),
    Path("google/protobuf/map_entry.h"),
    Path("google/protobuf/map_field.h"),
    Path("google/protobuf/map_field_inl.h"),
    Path("google/protobuf/message.h"),
    Path("google/protobuf/metadata.h"),
    Path("google/protobuf/reflection.h"),
    Path("google/protobuf/reflection_internal.h"),
    Path("google/protobuf/reflection_ops.h"),
    Path("google/protobuf/service.h"),
    Path("google/protobuf/source_context.pb.h"),
    Path("google/protobuf/struct.pb.h"),
    Path("google/protobuf/text_format.h"),
    Path("google/protobuf/timestamp.pb.h"),
    Path("google/protobuf/type.pb.h"),
    Path("google/protobuf/unknown_field_set.h"),
    Path("google/protobuf/util/delimited_message_util.h"),
    Path("google/protobuf/util/field_comparator.h"),
    Path("google/protobuf/util/field_mask_util.h"),
    Path("google/protobuf/util/json_util.h"),
    Path("google/protobuf/util/message_differencer.h"),
    Path("google/protobuf/util/time_util.h"),
    Path("google/protobuf/util/type_resolver.h"),
    Path("google/protobuf/util/type_resolver_util.h"),
    Path("google/protobuf/wire_format.h"),
    Path("google/protobuf/wrappers.pb.h"),
}

protobuf_internal_includes = {
    Path("google/protobuf/port_def.inc"),
    Path("google/protobuf/port_undef.inc"),
    Path("google/protobuf/stubs/int128.h"),
    Path("google/protobuf/stubs/mathutil.h"),
    Path("google/protobuf/stubs/statusor.h"),
    Path("google/protobuf/stubs/status_macros.h"),
    Path("google/protobuf/stubs/stringprintf.h"),
    Path("google/protobuf/stubs/substitute.h"),
    Path("google/protobuf/stubs/time.h"),
    Path("google/protobuf/util/internal/constants.h"),
    Path("google/protobuf/util/internal/datapiece.h"),
    Path("google/protobuf/util/internal/default_value_objectwriter.h"),
    Path("google/protobuf/util/internal/error_listener.h"),
    Path("google/protobuf/util/internal/field_mask_utility.h"),
    Path("google/protobuf/util/internal/json_escaping.h"),
    Path("google/protobuf/util/internal/json_objectwriter.h"),
    Path("google/protobuf/util/internal/json_stream_parser.h"),
    Path("google/protobuf/util/internal/location_tracker.h"),
    Path("google/protobuf/util/internal/object_location_tracker.h"),
    Path("google/protobuf/util/internal/object_source.h"),
    Path("google/protobuf/util/internal/object_writer.h"),
    Path("google/protobuf/util/internal/proto_writer.h"),
    Path("google/protobuf/util/internal/protostream_objectsource.h"),
    Path("google/protobuf/util/internal/protostream_objectwriter.h"),
    Path("google/protobuf/util/internal/structured_objectwriter.h"),
    Path("google/protobuf/util/internal/type_info.h"),
    Path("google/protobuf/util/internal/utility.h"),
}

use_src_files = protobuf_lite_sources | protobuf_sources
use_include_files = (
    protobuf_lite_includes | protobuf_includes | protobuf_internal_includes
)


def copy_upstream_src(wpilib_root: Path):
    upstream_root = Path(".").absolute()
    wpiutil = wpilib_root / "wpiutil"

    # Delete old install
    for d in [
        "src/main/native/thirdparty/protobuf/src",
        "src/main/native/thirdparty/protobuf/include",
    ]:
        shutil.rmtree(wpiutil / d, ignore_errors=True)

    # Copy protobuf source files into allwpilib
    os.chdir(upstream_root / "src/google/protobuf")
    walk_cwd_and_copy_if(
        lambda dp, f: dp / f in use_src_files,
        wpiutil / "src/main/native/thirdparty/protobuf/src",
    )

    # Copy protobuf header files into allwpilib
    os.chdir(upstream_root / "src")
    walk_cwd_and_copy_if(
        lambda dp, f: dp / f in use_include_files,
        wpiutil / "src/main/native/thirdparty/protobuf/include",
    )


def main():
    name = "protobuf"
    url = "https://github.com/protocolbuffers/protobuf"
    tag = "v3.21.12"

    protobuf = Lib(name, url, tag, copy_upstream_src)
    protobuf.main()


if __name__ == "__main__":
    main()
