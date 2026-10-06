"""Shared compiler/linker option selects for wpical targets across packages."""

UNIX_COPTS = [
    "-Wno-pedantic",
    "-Wno-format-nonliteral",
    "-Wno-unused-variable",
    "-Wno-unused-function",
    "-Wno-sign-compare",
]

CLANG_COPTS = UNIX_COPTS

COPTS = select({
    "@rules_cc//cc/compiler:clang": CLANG_COPTS,
    "@rules_cc//cc/compiler:gcc": UNIX_COPTS + [
        "-Wno-maybe-uninitialized",
    ],
    "@rules_cc//cc/compiler:msvc-cl": [
        "/wd4098",
        "/wd4267",
    ],
})

UNIX_CXXOPTS = [
    "-Wno-missing-field-initializers",
    "-Wno-pedantic",
    "-fpermissive",
    "-Wno-deprecated-declarations",
    "-Wno-return-type",
    "-Wno-missing-braces",
    "-Wno-null-conversion",
    "-Wno-unused-but-set-variable",
]

CLANG_CXXOPTS = UNIX_CXXOPTS + [
    "-Wno-unused-variable",
    "-Wno-unused-function",
    "-Wno-sign-compare",
    "-Wno-sometimes-uninitialized",
]

CXXOPTS = select({
    "@rules_cc//cc/compiler:clang": CLANG_CXXOPTS,
    "@rules_cc//cc/compiler:gcc": UNIX_CXXOPTS,
    "@rules_cc//cc/compiler:msvc-cl": [
        "/wd4068",
        "/wd4200",
        "/wd4576",
        "/wd4715",
    ],
})

MAC_LINKOPTS = [
    "-framework",
    "Accelerate",
    "-framework",
    "AVFoundation",
    "-framework",
    "CoreMedia",
]

LINKOPTS = select({
    "@platforms//os:linux": [],
    "@platforms//os:osx": MAC_LINKOPTS,
    "@platforms//os:windows": [
        "-DEFAULTLIB:Comdlg32.lib",
        "-DEFAULTLIB:dbghelp.lib",
        "-DEFAULTLIB:Advapi32.lib",
    ],
})
