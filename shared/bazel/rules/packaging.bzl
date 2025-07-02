load("@rules_pkg//:mappings.bzl", "pkg_filegroup", "pkg_files")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")

def pkg_java_src_files(name):
    pkg_files(
        name = name + "-java-srcs",
        srcs = native.glob(["src/main/java/**"]),
        strip_prefix = "src/main/java",
    )

    pkg_files(
        name = name + "-generated-java-srcs",
        srcs = native.glob(["src/generated/main/java/**/*.java"], allow_empty = True),
        strip_prefix = "src/generated/main/java",
    )

    pkg_files(
        name = name + "-proto-srcs",
        srcs = native.glob(["src/main/proto/**"], allow_empty = True),
        strip_prefix = "src/main/proto",
    )

    pkg_filegroup(
        name = name,
        srcs = [
            name + "-java-srcs",
            name + "-proto-srcs",
            name + "-generated-java-srcs",
            "//shared/bazel/rules:src_jar_dummy_manifest",
        ],
    )


def zip_java_srcs(name, extra_pkgs = []):
    pkg_java_src_files("{}-java-srcs".format(name))

    pkg_zip(
        name = "lib{}-java-sources".format(name),
        srcs = [":{}-java-srcs".format(name)] + extra_pkgs,
        out = "lib{}-java-sources.jar".format(name),
    )