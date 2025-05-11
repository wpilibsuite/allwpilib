import argparse
import json
import os
import pathlib
import shutil

from rules_python.python.runfiles import runfiles


def write_maven_metadata(output_path, group_id, artifact_id, version, timestamp):
    template = f"""<?xml version="1.0" encoding="UTF-8"?>
<metadata>
  <groupId>{group_id}</groupId>
  <artifactId>{artifact_id}</artifactId>
  <versioning>
    <latest>{version}</latest>
    <release>{version}</release>
    <versions>
      <version>{version}</version>
    </versions>
    <lastUpdated>{timestamp}</lastUpdated>
  </versioning>
</metadata>
"""
    with open(output_path, "w") as f:
        f.write(template)


def write_pom(output_file, group_id, artifact_id, version):
    contents = f"""<?xml version="1.0" encoding="UTF-8"?>
<project xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 https://maven.apache.org/xsd/maven-4.0.0.xsd" xmlns="http://maven.apache.org/POM/4.0.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <modelVersion>4.0.0</modelVersion>
  <groupId>{group_id}</groupId>
  <artifactId>{artifact_id}</artifactId>
  <version>{version}</version>
  <packaging>pom</packaging>
</project>
"""

    with open(output_file, "w") as f:
        f.write(contents)


def main():
    bazel_root = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"])

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        type=pathlib.Path,
        default=bazel_root / "build/bazel_maven",
    )
    parser.add_argument("--publishing_version")
    parser.add_argument("--bundles", nargs="+")
    args = parser.parse_args()

    publishing_version = args.publishing_version or "6969"

    timestamp = "20250424182753"

    print(f"Deploying artifacts to {args.output_directory}")

    r = runfiles.Create()
    for bundle in args.bundles:
        with open(r.Rlocation("__main__/" + bundle), "r") as f:
            json_data = json.load(f)

        for maven_info in json_data:
            artifact = bazel_root / pathlib.Path(maven_info["artifact"])
            output_path = (
                args.output_directory
                / maven_info["maven_base"].replace(".", "/")
                / maven_info["artifact_name"]
                / publishing_version
                / (
                    maven_info["artifact_name"]
                    + "-"
                    + publishing_version
                    + maven_info["suffix"]
                    + artifact.suffix
                )
            )

            output_path.parent.mkdir(parents=True, exist_ok=True)

            shutil.copy(artifact, output_path)
            os.chmod(output_path, 0o777)

            maven_metadata_path = (
                args.output_directory
                / maven_info["maven_base"].replace(".", "/")
                / maven_info["artifact_name"]
                / "maven-metadata-local.xml"
            )
            write_maven_metadata(
                maven_metadata_path,
                maven_info["maven_base"],
                maven_info["artifact_name"],
                publishing_version,
                timestamp,
            )

            pom_path = (
                args.output_directory
                / maven_info["maven_base"].replace(".", "/")
                / maven_info["artifact_name"]
                / publishing_version
                / (maven_info["artifact_name"] + "-" + publishing_version + ".pom")
            )
            write_pom(
                pom_path,
                maven_info["maven_base"],
                maven_info["artifact_name"],
                publishing_version,
            )


if __name__ == "__main__":
    main()
