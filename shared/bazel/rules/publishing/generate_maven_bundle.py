import argparse
import json


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output_file")
    parser.add_argument("--maven_infos", nargs="+")
    args = parser.parse_args()

    json_data = []

    for maven_info in args.maven_infos:
        artifact, maven_base, artifact_name, suffix = maven_info.split(",")
        json_data.append(
            dict(
                artifact=artifact,
                maven_base=maven_base,
                artifact_name=artifact_name,
                suffix=suffix,
            )
        )

    # print(json.dumps(json_data, indent=4))
    with open(args.output_file, "w") as f:
        f.write(json.dumps(json_data))


if __name__ == "__main__":
    main()
