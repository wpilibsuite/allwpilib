load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@rules_python//python:pip.bzl", "pip_parse")
load("//shared/bazel/deps/gtest:repo.bzl", "third_party_gtest")

def load_third_party():
    third_party_gtest()

    maven_artifacts = [
        "org.ejml:ejml-simple:0.41",
        "com.fasterxml.jackson.core:jackson-annotations:2.10.0",
        "com.fasterxml.jackson.core:jackson-core:2.10.0",
        "com.fasterxml.jackson.core:jackson-databind:2.10.0",
        "org.junit.jupiter:junit-jupiter-api:5.6.1",
        "org.junit.jupiter:junit-jupiter-params:5.6.1",
        "org.junit.jupiter:junit-jupiter-engine:5.6.1",
        "org.junit.platform:junit-platform-commons:1.6.1",
        "org.junit.platform:junit-platform-console:1.6.1",
        "org.junit.platform:junit-platform-engine:1.6.1",
        "org.junit.platform:junit-platform-launcher:1.6.1",
        "org.junit.platform:junit-platform-suite-api:1.6.1",
        "com.google.code.gson:gson:2.8.5",
        "org.hamcrest:hamcrest-all:1.3",
        "com.googlecode.junit-toolbox:junit-toolbox:2.0",
        "org.apache.ant:ant:1.9.4",
        "org.apache.ant:ant-junit:1.9.4",
        "com.google.guava:guava:19.0",
        "org.mockito:mockito-core:2.27.0",
    ]

    maven_install(
        name = "maven",
        artifacts = maven_artifacts,
        repositories = [
            "https://repo1.maven.org/maven2",
            "https://frcmaven.wpi.edu/artifactory/release/",
        ],
        maven_install_json = "//shared/bazel/deps:maven_install.json",
    )

    pip_parse(
        name = "__allwpilib_pip_deps",
        requirements_lock = "//shared/bazel/deps:requirements_lock.txt",
    )
