load("@rules_cc//cc:defs.bzl", "cc_library")

def wpi_proto_library(
        name,
        proto_files,
        include_prefix):
    gen_srcs = []
    gen_hdrs = []

    for pf in proto_files:
        gen_srcs.append("generated_proto/" + pf[:-5] + "pb.cc")
        gen_hdrs.append("generated_proto/" + pf[:-5] + "pb.h")

    cmd = "$(locations //protoplugin:bazel_proto_generator) --include_prefix={} --proto_files $(SRCS) --output_files $(OUTS)".format(include_prefix)
    native.genrule(
        name = "generate_proto",
        srcs = proto_files,
        outs = gen_srcs + gen_hdrs,
        cmd = cmd,
        tools = [
            "//protoplugin:bazel_proto_generator",
        ],
        visibility = ["//visibility:public"],
    )

    native.filegroup(
        name = name + ".srcs",
        srcs = gen_srcs,
    )

    native.filegroup(
        name = name + ".hdr_files",
        srcs = gen_hdrs,
    )

    cc_library(
        name = name + ".hdrs",
        hdrs = gen_hdrs,
        includes = ["generated_proto/src/main/proto"],
        visibility = ["//visibility:public"],
    )
