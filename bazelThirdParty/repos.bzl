load("@wpi_bazel_deps//third_party/gtest:repo.bzl", "third_party_gtest")
load("//bazelThirdParty:maven.bzl", "third_party_maven")
load("@local_imgui//:repo.bzl", "third_party_imgui")
load("@local_opencv//:repo.bzl", "third_party_opencv")
load("@local_ni//:repo.bzl", "third_party_ni_libraries")

def wpilib_third_party_repositories():
    third_party_gtest()
    third_party_opencv()
    third_party_imgui()
    third_party_ni_libraries()

    third_party_maven()
