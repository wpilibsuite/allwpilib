load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def third_party_gtest():
    http_archive(
        name = "gtest",
        sha256 = "8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7",
        strip_prefix = "googletest-1.14.0",
        urls = ["https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz"],
    )
