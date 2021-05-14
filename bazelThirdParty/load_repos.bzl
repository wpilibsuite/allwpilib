
load("@wpi_bazel_deps//third_party/edu_wpi_first_thirdparty_imgui:load_imgui.bzl", "load_imgui")
load("@wpi_bazel_deps//third_party/edu_wpi_first_thirdparty_opencv:load_opencv.bzl", "load_opencv")
load("@wpi_bazel_deps//third_party/com_ni_libraries:load_ni_libraries.bzl", "load_ni")


def load_repos():
    load_imgui(version="1.82-1")
    load_opencv(version="4.5.2-1")
    load_ni("2020.10.1")