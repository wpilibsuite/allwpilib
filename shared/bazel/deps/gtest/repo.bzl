load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def third_party_gtest():
    http_archive(
        name = "gtest",
        strip_prefix = "googletest-703bd9caab50b139428cea1aaff9974ebee5742e",
        sha256 = "2db427be8b258ad401177c411c2a7c2f6bc78548a04f1a23576cc62616d9cd38",
        urls = ["https://github.com/google/googletest/archive/703bd9caab50b139428cea1aaff9974ebee5742e.zip"],
    )
