# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Download vendordep artifacts and retain reproducible, explicit coverage records."""

import concurrent.futures
import hashlib
import json
import re
import shutil
import subprocess
import urllib.error
import urllib.parse
import urllib.request
import zipfile

PLATFORM = "linuxx86-64"
MAX_DOWNLOAD = 512 * 1024 * 1024


def digest(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def vendor_series(tag):
    """Map release tags to the vendor registry's stable, beta, or alpha series."""
    match = re.fullmatch(r"v(\d{4})\.\d+\.\d+(?:-(alpha|beta|rc)-(\d+))?", tag)
    if not match:
        raise ValueError(f"Cannot select vendor series for {tag}")
    year, stage, number = match.groups()
    if stage == "alpha":
        return f"{year}_alpha{number}"
    if stage in ("beta", "rc"):
        return f"{year}beta"
    return year


def artifact_requests(manifests, platform=PLATFORM, configuration="release"):
    """Select Java and release native artifacts actually declared for our platform."""
    requests = {}
    skipped = []
    for manifest in manifests:
        for kind in ("java", "cpp", "jni"):
            for dep in manifest.get(f"{kind}Dependencies", []):
                classifier = ""
                extension = "jar"
                if kind != "java":
                    platforms = dep.get(
                        "binaryPlatforms" if kind == "cpp" else "validPlatforms", []
                    )
                    if platform not in platforms:
                        skipped.append(
                            {
                                "manifest": manifest["file"],
                                "kind": kind,
                                "dependency": dep,
                                "reason": f"No {platform} binary declared",
                            }
                        )
                        continue
                    classifier = "-" + platform
                    if kind == "cpp" and not dep.get("sharedLibrary", False):
                        classifier += "static"
                    if configuration == "debug":
                        classifier += "debug"
                    extension = "jar" if kind == "jni" and dep.get("isJar") else "zip"
                group, artifact, version = (
                    dep[k] for k in ("groupId", "artifactId", "version")
                )
                # Coordinates become paths, so reject separators and traversal components.
                if any(
                    not re.fullmatch(r"[A-Za-z0-9_+.-]+", s) or s in (".", "..")
                    for s in (group, artifact, version)
                ) or any(not s for s in group.split(".")):
                    raise ValueError(f"Invalid Maven coordinate: {dep}")
                path = f"{group.replace('.', '/')}/{artifact}/{version}/{artifact}-{version}{classifier}.{extension}"
                request = requests.setdefault(
                    path, {"path": path, "kinds": [], "manifests": [], "urls": []}
                )
                request["kinds"] = sorted(set(request["kinds"]) | {kind})
                request["manifests"] = sorted(
                    set(request["manifests"]) | {manifest["file"]}
                )
                for repository in manifest["mavenUrls"]:
                    # Artifactory's ordinary endpoint can redirect anonymous clients to HTML.
                    if (
                        urllib.parse.urlsplit(repository).hostname == "frcmaven.wpi.edu"
                        and "/api/download/" not in repository
                    ):
                        repository = repository.replace(
                            "/artifactory/", "/artifactory/api/download/"
                        )
                    url = repository.rstrip("/") + "/" + path
                    if url not in request["urls"]:
                        request["urls"].append(url)
    return list(requests.values()), skipped


def download_artifact(request, destination):
    """Try declared Maven repositories in order; never accept an HTML login page."""
    result = dict(request, status="unavailable", attempts=[])
    target = destination / request["path"]
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = target.with_suffix(target.suffix + ".part")
    for url in request["urls"]:
        try:
            if urllib.parse.urlsplit(url).scheme != "https":
                raise ValueError("Only HTTPS Maven repositories are supported")
            req = urllib.request.Request(
                url, headers={"User-Agent": "Mozilla/5.0 WPILib-compatibility"}
            )
            with (
                urllib.request.urlopen(req, timeout=30) as response,
                temporary.open("wb") as stream,
            ):
                if urllib.parse.urlsplit(response.url).scheme != "https":
                    raise ValueError("Download redirected away from HTTPS")
                size = 0
                while chunk := response.read(1024 * 1024):
                    size += len(chunk)
                    if size > MAX_DOWNLOAD:
                        raise ValueError("Artifact exceeds download size limit")
                    stream.write(chunk)
            with zipfile.ZipFile(temporary) as archive:
                if not archive.infolist():
                    raise ValueError("Empty archive")
            temporary.replace(target)
            result.update(
                status="downloaded",
                url=url,
                sha256=digest(target),
                bytes=target.stat().st_size,
            )
            return result
        except (OSError, ValueError, zipfile.BadZipFile) as error:
            result["attempts"].append({"url": url, "error": str(error)})
        finally:
            temporary.unlink(missing_ok=True)
    return result


def download(args):
    """Always write an index, including selection and individual download failures."""
    args.output.mkdir(parents=True, exist_ok=True)
    index = {
        "platform": getattr(args, "platform", PLATFORM),
        "configuration": getattr(args, "configuration", "release"),
        "artifacts": [],
        "skipped": [],
        "errors": [],
    }
    try:
        tag = subprocess.check_output(
            [
                "git",
                "-C",
                str(args.repo),
                "describe",
                "--tags",
                "--first-parent",
                "--match",
                "v[0-9]*",
                "--abbrev=0",
                args.baseline,
            ],
            text=True,
        ).strip()
        series = args.series or vendor_series(tag)
        if not re.fullmatch(r"\d{4}(?:_alpha\d+|beta)?", series):
            raise ValueError(f"Invalid vendor series: {series}")
        revision = subprocess.check_output(
            ["git", "-C", str(args.inventory), "rev-parse", "HEAD"], text=True
        ).strip()
        index.update(tag=tag, series=series, revision=revision)
        manifests = []
        for path in sorted((args.inventory / series).glob("*.json")):
            manifest = json.loads(path.read_text())
            manifest["file"] = path.name
            manifests.append(manifest)
        if not manifests:
            raise ValueError(f"No vendor manifests for {series} at {revision}")
        (args.output / "manifests.json").write_text(
            json.dumps(manifests, indent=2) + "\n"
        )
        index["manifests_sha256"] = digest(args.output / "manifests.json")
        index["manifest_count"] = len(manifests)
        requests, index["skipped"] = artifact_requests(
            manifests, index["platform"], index["configuration"]
        )
        with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
            index["artifacts"] = list(
                pool.map(
                    lambda req: download_artifact(req, args.output / "artifacts"),
                    requests,
                )
            )
        for artifact in index["artifacts"]:
            if artifact["status"] != "downloaded":
                index["errors"].append(
                    f"Unavailable vendor artifact: {artifact['path']}"
                )
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        index["errors"].append(str(error))
    (args.output / "index.json").write_text(json.dumps(index, indent=2) + "\n")
    return int(bool(index["errors"]))


def load_inventory(directory, output):
    """Verify recorded downloads before analysis and copy provenance into the report."""
    index = json.loads((directory / "index.json").read_text())
    shutil.copyfile(directory / "index.json", output / "vendors-downloads.json")
    if (directory / "manifests.json").exists():
        if digest(directory / "manifests.json") != index["manifests_sha256"]:
            raise ValueError("Vendor manifest checksum mismatch")
        shutil.copyfile(directory / "manifests.json", output / "vendors-manifests.json")
    for artifact in index["artifacts"]:
        if artifact["status"] != "downloaded":
            continue
        path = (directory / "artifacts" / artifact["path"]).resolve()
        if (
            not path.is_relative_to((directory / "artifacts").resolve())
            or digest(path) != artifact["sha256"]
        ):
            raise ValueError(f"Vendor artifact checksum mismatch: {artifact['path']}")
    return index
