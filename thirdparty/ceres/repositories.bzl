""" Starlark file for ceres repository definitions """

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def ceres_repositories():
    """ Fetches the ceres solver libraries """
    year = "frc2026"
    version = "2.2-1"

    http_archive(
        name = "ceres_headers",
        build_file_content = """
load(\"@rules_cc//cc:defs.bzl\", \"cc_library\")

cc_library(
    name = \"headers\",
    #hdrs = glob(["**"], exclude = ["LICENSE.md", "signature_of_eigen3_matrix_library"]),
    hdrs = glob(["ceres/**", "glog/**", "suitesparse/**", "openblas/**"]),
    includes = ["."],
    deps = ["@//wpimath:eigen-headers"],
    visibility = [\"//visibility:public\"],
)
""",
        url = "https://frcmaven.wpi.edu/artifactory/development/edu/wpi/first/thirdparty/" + year + "/ceres/ceres-cpp/" + version + "/ceres-cpp-" + version + "-headers.zip",
        integrity = "sha256-ITP1hirOrcna3tyOUQyniUGPw5JeeC8Ffm5scno865w=",
    )

    _LIB_ARTIFACTS = {
        "linuxarm32static": ("linux", "**/*.a", "sha256-ptlO1AuEWz84nAZn0fdXDf6fRsfj1EQu+/FR7PQO8Pk="),
        "linuxarm32staticdebug": ("linux", "**/*.a", "sha256-1n5wFPsML0HYuOodbiDTT4taxo5k/IC7to7YBPQtM1Q="),
        "linuxarm64static": ("linux", "**/*.a", "sha256-sBoNq7nTyllKJnOvyNm+IAxnKi8wP3YymaMwK2m8qCw="),
        "linuxarm64staticdebug": ("linux", "**/*.a", "sha256-44RmHzCSx8rptaaP3DC2IbTy4p61oAHzfzvomCALk6I="),
        "linuxx86-64static": ("linux", "**/*.a", "sha256-ntuu0fS01f/vkL4rMaYEuUaDvuYqQwwqLKcQy6yJwd8="),
        "linuxx86-64staticdebug": ("linux", "**/*.a", "sha256-2CmV1Z+gM3AKq/+rC9WSn8Gkx/OXLnVMjHlGT1kmB/c="),
        "osxuniversalstatic": ("osx", "**/*.a", "sha256-ExnU2z+kGU0iaYRmOpcPCLWdwIDKhVpQnN2g6iJ/z/U="),
        "osxuniversalstaticdebug": ("osx", "**/*.a", "sha256-dnqxm5qgVBKD43ORCIkYH+mXi7oQQKbCTWDY1GdTNfQ="),
        "windowsarm64static": ("windows", "**/*.lib", "sha256-obWGoORklu5g//x7CmykDX4S4g7ixy9RECdui5zy28g="),
        "windowsarm64staticdebug": ("windows", "**/*.lib", "sha256-iL/rdZ2i+9+48IxQEK3Ty8/zQkp/1h9halDG9YEEXz0="),
        "windowsx86-64static": ("windows", "**/*.lib", "sha256-wPhWBdyEC3AK6KgUgbvJsxWODGPSUMbAZmdEE7zSJ9Y="),
        "windowsx86-64staticdebug": ("windows", "**/*.lib", "sha256-ibozSdLVUEYAuECwKtTKrqZB5pNFdajktwF2TEw+aDg="),
    }

    for artifact, (prefix, glob_pattern, integrity) in _LIB_ARTIFACTS.items():
        repo_name = "ceres_" + artifact
        build_file_content = """
filegroup(
    name = \"lib\",
    srcs = glob([\"%s\"]),
    visibility = [\"//visibility:public\"],
)
""" % glob_pattern

        url_fname = "ceres-cpp-" + version + "-" + artifact + ".zip"

        http_archive(
            name = repo_name,
            build_file_content = build_file_content,
            strip_prefix = prefix,
            url = "https://frcmaven.wpi.edu/artifactory/development/edu/wpi/first/thirdparty/" + year + "/ceres/ceres-cpp/" + version + "/" + url_fname,
            integrity = integrity,
        )
