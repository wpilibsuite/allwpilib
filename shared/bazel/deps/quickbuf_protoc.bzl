load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

QUICKBUF_VERSION = "1.3.2"

def __setup_quickbuf_protoc(mctx):
    http_file(
        name = "quickbuffer_protoc_linux",
        url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-linux-x86_64.exe",
        sha256 = "f9a041bccaa7040db523666ef1b5fe9f6f94e70a82c88951f18f58aadd9c50b5",
        executable = True,
    )

    http_file(
        name = "quickbuffer_protoc_osx",
        url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-osx-x86_64.exe   ",
        sha256 = "ea307c2b69664ae7e7c69db4cddf5803187e5a34bceffd09a21652f0f16044f7",
        executable = True,
    )

    http_file(
        name = "quickbuffer_protoc_windows",
        url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-windows-x86_64.exe ",
        sha256 = "27dc1f29764a62b5e6a813a4bcd63e81bbdc3394da760a44acae1025b4a89f1d",
        executable = True,
    )

setup_quickbuf_protoc = module_extension(
    __setup_quickbuf_protoc,
)

def setup_non_bzlmod_quickbuf_protoc():
    __setup_quickbuf_protoc(None)
