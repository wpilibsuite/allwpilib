load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def third_party_gtest():
    http_archive(
        name = "gtest",
        strip_prefix = "googletest-58d77fa8070e8cec2dc1ed015d66b454c8d78850",
        sha256 = "ab78fa3f912d44d38b785ec011a25f26512aaedc5291f51f3807c592b506d33a",
        urls = ["https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip"],
    )
