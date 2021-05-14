load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@wpi_bazel_deps//third_party:org_juni_jupiter_junit5.bzl", "get_junit5_maven_deps")
load("@wpi_bazel_deps//third_party:com_fasterxml_jackson_core.bzl", "get_fasterxml_maven_deps")
load("@local_opencv//:repo.bzl", "third_party_opencv_java_version")

def third_party_maven():
    artifacts = []
    artifacts += get_junit5_maven_deps()
    artifacts += get_fasterxml_maven_deps()
    artifacts += [
        "org.ejml:ejml-simple:0.38",
        "com.google.code.gson:gson:2.8.5",
        "org.hamcrest:hamcrest-all:1.3",
        "com.googlecode.junit-toolbox:junit-toolbox:2.0",
        "org.apache.ant:ant:1.9.4",
        "org.apache.ant:ant-junit:1.9.4",
        "com.google.guava:guava:19.0",
        "org.mockito:mockito-core:2.27.0",
        third_party_opencv_java_version(),
    ]

    maven_install(
        name = "maven",
        artifacts = artifacts,
        repositories = [
            "https://repo1.maven.org/maven2",
            "https://frcmaven.wpi.edu/artifactory/release/",
        ],
    )
