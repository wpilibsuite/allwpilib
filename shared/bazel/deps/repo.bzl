load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@rules_python//python:pip.bzl", "pip_parse")
load("//shared/bazel/deps/gtest:repo.bzl", "third_party_gtest")

def load_third_party():
    third_party_gtest()

    # Last compared to gradle on 10/15/22
    maven_artifacts = [
        "org.ejml:ejml-simple:0.41",
        "com.fasterxml.jackson.core:jackson-annotations:2.12.4",
        "com.fasterxml.jackson.core:jackson-core:2.12.4",
        "com.fasterxml.jackson.core:jackson-databind:2.12.4",
        "org.junit.jupiter:junit-jupiter-api:5.8.2",
        "org.junit.jupiter:junit-jupiter-params:5.8.2",
        "org.junit.jupiter:junit-jupiter-engine:5.8.2",
        "org.junit.platform:junit-platform-commons:1.6.1",
        "org.junit.platform:junit-platform-console:1.6.1",
        "org.junit.platform:junit-platform-engine:1.6.1",
        "org.junit.platform:junit-platform-launcher:1.6.1",
        "org.junit.platform:junit-platform-suite-api:1.6.1",
        "com.google.code.gson:gson:2.8.9",
        "org.hamcrest:hamcrest-all:1.3",
        "com.googlecode.junit-toolbox:junit-toolbox:2.4",
        "org.apache.ant:ant:1.10.12",
        "org.apache.ant:ant-junit:1.10.12",
        "org.mockito:mockito-core:4.1.0",
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
