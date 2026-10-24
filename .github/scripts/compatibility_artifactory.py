# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Read published WPILib baselines without running their build scripts."""

import concurrent.futures
import functools
import io
import json
import re
import subprocess
import tarfile
import urllib.error
import urllib.request
import xml.etree.ElementTree as ET

import compatibility_vendors
import tomllib

ARTIFACTORY = "https://frcmaven.wpi.edu/artifactory"


def component(value):
    """Validate Maven path components before constructing URLs or filenames."""
    if not re.fullmatch(r"[A-Za-z0-9_+.-]+", value) or value in (".", ".."):
        raise ValueError(f"Invalid Maven component: {value!r}")
    return value


@functools.lru_cache(maxsize=4096)
def read_url(url):
    request = urllib.request.Request(
        url, headers={"User-Agent": "WPILib-compatibility"}
    )
    with urllib.request.urlopen(request, timeout=60) as response:
        data = response.read(8 * 1024 * 1024 + 1)
    if len(data) > 8 * 1024 * 1024:
        raise ValueError(f"Oversized metadata: {url}")
    return data


@functools.lru_cache(maxsize=4096)
def storage(repository, path):
    """Return anonymous storage metadata; only a 404 means absent publication."""
    try:
        return json.loads(read_url(f"{ARTIFACTORY}/api/storage/{repository}/{path}"))
    except urllib.error.HTTPError as error:
        if error.code == 404:
            return None
        raise


def git(repo, *args):
    return subprocess.check_output(
        ["git", "-C", str(repo), *args], text=True, stderr=subprocess.DEVNULL
    ).strip()


@functools.lru_cache(maxsize=32)
def sources(repo, revision):
    """Read build declarations as data, never execute historical Gradle code."""
    names = git(repo, "ls-tree", "-r", "--name-only", revision).splitlines()
    names = [
        p for p in names if p.endswith(".gradle") or p == "gradle/libs.versions.toml"
    ]
    archive = subprocess.check_output(
        ["git", "-C", str(repo), "archive", revision, "--", *names]
    )
    with tarfile.open(fileobj=io.BytesIO(archive)) as contents:
        return {
            m.name: contents.extractfile(m).read().decode()
            for m in contents
            if m.isfile()
        }


def publications(source):
    """Recover the same Java, shared native, JNI and HALSim scope as the producer."""
    result = []
    prefix_match = re.search(
        r"artifactGroupId\s*=\s*['\"]([^'\"]+)\$\{nativeName\}",
        source.get("shared/jni/publish.gradle", ""),
    )
    prefix = prefix_match[1] if prefix_match else "org.wpilib."
    for path, text in sorted(source.items()):
        if not re.fullmatch(r"(?:[^/]+|simulation/[^/]+)/build.gradle", path):
            continue
        project = path.removesuffix("/build.gradle")
        if project == "ntcoreffi":
            continue

        def literal(name, default="", text=text):
            match = re.search(rf"\b{name}\s*=\s*['\"]([^'\"$]*)['\"]", text)
            return match[1] if match else default

        native = literal("nativeName")
        common = (
            "shared/jni/setupBuild.gradle" in text
            or "shared/javacpp/setupBuild.gradle" in text
        )
        base = native if common else literal("baseId", native)
        group = prefix + native if common else literal("groupId", prefix + native)
        java = any(
            p in text
            for p in (
                "shared/java/javacommon.gradle",
                "shared/jni/setupBuild.gradle",
                "shared/javacpp/setupBuild.gradle",
            )
        )
        if java:
            result.append(
                {
                    "project": project,
                    "group": group,
                    "artifact": base + "-java",
                    "kind": "java",
                }
            )
        publish = text + source.get(project + "/publish.gradle", "")
        cpp = native and (
            "shared/jni/setupBuild.gradle" in text
            or "shared/javacpp/setupBuild.gradle" in text
            or "-cpp" in publish
        )
        if cpp:
            result.append(
                {
                    "project": project,
                    "group": prefix + native,
                    "artifact": native + "-cpp",
                    "kind": "native",
                }
            )
            # fields publishes its image library independently of its main library.
            for artifact in re.findall(
                r"artifactId\s*=\s*['\"]([^'\"$]+-cpp)['\"]", publish
            ):
                if artifact != native + "-cpp":
                    result.append(
                        {
                            "project": project,
                            "group": group,
                            "artifact": artifact,
                            "kind": "native",
                        }
                    )
        if "JNICvStatic" in text:
            result.append(
                {
                    "project": project,
                    "group": prefix + native,
                    "artifact": native + "-jnicvstatic",
                    "kind": "native",
                }
            )
        plugin = literal("pluginName")
        if project.startswith("simulation/") and plugin:
            result.append(
                {
                    "project": project,
                    "group": prefix + "halsim",
                    "artifact": plugin,
                    "kind": "native",
                }
            )
    for item in result:
        component(item["group"])
        component(item["artifact"])
    if not result:
        raise ValueError("No supported publications in baseline build declarations")
    return result


def external_dependencies(source, inventory):
    """Recover direct compile dependencies from the baseline's version catalog."""
    catalog = tomllib.loads(source["gradle/libs.versions.toml"])
    libraries = catalog["libraries"]
    aliases = {re.sub(r"[-_]", ".", key): key for key in libraries}
    selected = set()
    build_files = {
        p["project"] + "/build.gradle" for p in inventory if p["kind"] == "java"
    }
    build_files.update(
        p for p in source if p.startswith("shared/") and p.endswith(".gradle")
    )
    for path in sorted(build_files):
        for alias in re.findall(
            r"(?m)^\s*(?:api|implementation|compileOnly|compileOnlyApi)\s*\(?\s*libs\.([\w.]+)",
            source[path],
        ):
            if alias.startswith("bundles."):
                selected.update(catalog["bundles"][alias.removeprefix("bundles.")])
            else:
                selected.add(aliases[alias])
    dependencies = []
    for alias in sorted(selected):
        item = libraries[alias]
        group, artifact = item["module"].split(":")
        version = item["version"]
        if isinstance(version, dict):
            version = catalog["versions"][version["ref"]]
        dependencies.append(tuple(component(s) for s in (group, artifact, version)))
    exclusions = set()
    for path in build_files:
        exclusions.update(
            re.findall(
                r"exclude\(group:\s*['\"]([^'\"]+)['\"],\s*module:\s*['\"]([^'\"]+)['\"]",
                source[path],
            )
        )
    return dependencies, sorted(exclusions)


def version_candidates(repo, revision, candidate, branch):
    """Match exact commits, then walk published revisions newest first on the branch."""
    tag = git(
        repo,
        "describe",
        "--tags",
        "--first-parent",
        "--match",
        "v[0-9]*",
        "--abbrev=0",
        revision,
    )
    year = re.match(r"v(\d{4})", tag)[1]
    try:
        branch_head = git(
            repo, "rev-parse", "--verify", f"refs/remotes/origin/{branch}^{{commit}}"
        )
    except subprocess.CalledProcessError:
        branch_head = revision
    history = git(repo, "rev-list", "--first-parent", branch_head).splitlines()
    if revision not in history:
        # A stale/refetched branch must not hide versions on the requested history.
        history = git(repo, "rev-list", "--first-parent", revision).splitlines()
    rank = {sha: i for i, sha in enumerate(history)}
    repositories = ["release", "development", f"release-{year}", f"development-{year}"]
    modules = publications(sources(repo, revision))
    anchor = next((p for p in modules if p["artifact"] == "wpiutil-java"), None)
    if anchor is None:
        raise ValueError("Cannot identify the baseline wpiutil-java publication")
    anchor_path = anchor["group"].replace(".", "/") + "/" + anchor["artifact"]
    versions = []
    for repository in repositories:
        try:
            metadata = ET.fromstring(
                read_url(
                    f"{ARTIFACTORY}/api/download/{repository}/{anchor_path}/maven-metadata.xml"
                )
            )
        except urllib.error.HTTPError as error:
            if error.code == 404:
                continue
            raise
        for node in metadata.findall("./versioning/versions/version"):
            version = component(node.text or "")
            if not version.startswith(year + "."):
                continue
            match = re.search(r"-\d+-g([0-9a-f]{7,40})$", version)
            ref = match[1] if match else "refs/tags/v" + version
            try:
                sha = git(repo, "rev-parse", "--verify", ref + "^{commit}")
            except subprocess.CalledProcessError:
                continue
            if sha == candidate or (sha != revision and sha not in rank):
                continue
            versions.append(
                {"repository": repository, "version": version, "revision": sha}
            )
    # Prefer release publications at tags; a development build can have the same version.
    versions.sort(
        key=lambda v: (
            v["revision"] != revision,
            rank.get(v["revision"], -1),
            not v["repository"].startswith("release"),
        )
    )
    return versions


def inventory(repo, selection, platform):
    """Require all baseline publications for both configurations of this platform."""
    source = sources(repo, selection["revision"])
    modules = publications(source)
    result = {config: {"java": [], "native": []} for config in ("release", "debug")}

    def module_files(item):
        group, artifact = item["group"], item["artifact"]
        folder = f"{group.replace('.', '/')}/{artifact}/{selection['version']}"
        # GUI, image data and HALSim libraries are desktop-only publications.
        if platform == "linuxsystemcore" and (
            item["project"].startswith("simulation/")
            or item["artifact"] in ("field-images-cpp", "wpigui-cpp")
        ):
            return []
        metadata = storage(selection["repository"], folder)
        if metadata is None:
            raise ValueError(f"Missing publication {folder}")
        names = {
            c["uri"].removeprefix("/") for c in metadata["children"] if not c["folder"]
        }
        if (
            item["kind"] == "native"
            and not any(
                re.search(
                    r"-(?:linux(?:x86-64|arm64|systemcore)|windows(?:x86-64|arm64)|osxuniversal)(?:debug)?\.zip$",
                    name,
                )
                for name in names
            )
            and any("static" in name for name in names)
        ):
            return []  # Static-only libraries have no shared ABI to compare.
        entries = []
        for config in result:
            classifier = (
                ""
                if item["kind"] == "java"
                else "-" + platform + ("debug" if config == "debug" else "")
            )
            suffix = ".jar" if item["kind"] == "java" else ".zip"
            filename = f"{artifact}-{selection['version']}{classifier}{suffix}"
            if filename not in names:
                raise ValueError(f"Missing published artifact {folder}/{filename}")
            project = item["project"].replace("/", "-")
            name = project + ("" if item["kind"] == "java" else "-" + artifact) + suffix
            entries.append(
                (
                    config,
                    item["kind"],
                    {"file": filename, "name": name, "path": folder + "/" + filename},
                )
            )
        return entries

    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        for entries in pool.map(module_files, modules):
            for config, kind, entry in entries:
                result[config][kind].append(entry)
    return result


def select(repo, revision, candidate, branch, targets):
    """Choose one complete published version per platform, recording every deviation."""
    plans = [
        dict(
            t,
            baseline_revision=revision,
            baseline_version="",
            baseline_repository="",
            baseline_warning="",
            baseline_error="",
        )
        for t in targets
    ]
    diagnostics = []
    try:
        versions = version_candidates(repo, revision, candidate, branch)
        for version in versions:
            for target in plans:
                if target["baseline_version"]:
                    continue
                try:
                    inventory(repo, version, target["platform"])
                except ValueError as error:
                    diagnostics.append(
                        f"{version['repository']}/{version['version']} {target['platform']}: {error}"
                    )
                    continue
                target.update(
                    baseline_revision=version["revision"],
                    baseline_version=version["version"],
                    baseline_repository=version["repository"],
                )
                if version["revision"] != revision:
                    target["baseline_warning"] = (
                        f"Requested baseline {revision} is unavailable for {target['platform']}. "
                        f"Using Artifactory {version['repository']}/{version['version']} "
                        f"({version['revision']}), the most recent available version on {branch}."
                    )
            if all(p["baseline_version"] for p in plans):
                break
    except (
        OSError,
        ValueError,
        KeyError,
        ET.ParseError,
        subprocess.SubprocessError,
    ) as error:
        diagnostics.append(str(error))
    for target in plans:
        if not target["baseline_version"]:
            target["baseline_error"] = (
                f"No complete Artifactory baseline for {revision} or published versions on {branch} ({target['platform']}). "
                + "; ".join(diagnostics[-3:])
            )
        names = target["artifacts"]
        target["pattern"] = names[0] if len(names) == 1 else "{" + ",".join(names) + "}"
    return plans


def dependency_pom(dependencies, exclusions, repositories):
    """Use Maven's dependency resolver, without invoking any WPILib build."""
    root = ET.Element("project", xmlns="http://maven.apache.org/POM/4.0.0")
    for key, value in (
        ("modelVersion", "4.0.0"),
        ("groupId", "org.wpilib.compatibility"),
        ("artifactId", "baseline-classpath"),
        ("version", "1"),
    ):
        ET.SubElement(root, key).text = value
    repos = ET.SubElement(root, "repositories")
    for repository in repositories:
        node = ET.SubElement(repos, "repository")
        ET.SubElement(node, "id").text = repository
        ET.SubElement(node, "url").text = f"{ARTIFACTORY}/api/download/{repository}"
    deps = ET.SubElement(root, "dependencies")
    for group, artifact, version in dependencies:
        node = ET.SubElement(deps, "dependency")
        for key, value in (
            ("groupId", group),
            ("artifactId", artifact),
            ("version", version),
        ):
            ET.SubElement(node, key).text = value
        excluded = ET.SubElement(node, "exclusions")
        for ex_group, ex_artifact in exclusions:
            item = ET.SubElement(excluded, "exclusion")
            ET.SubElement(item, "groupId").text = ex_group
            ET.SubElement(item, "artifactId").text = ex_artifact
    return ET.tostring(root, encoding="unicode")


def download(repo, selection, platform, products):
    """Download and checksum published inputs, synthesizing local analyzer metadata."""
    targets = inventory(repo, selection, platform)
    products.mkdir(parents=True, exist_ok=True)
    extras = products / "compatibility"
    extras.mkdir(exist_ok=True)
    (extras / "complete").unlink(missing_ok=True)
    requests = {
        e["path"]: e
        for t in targets.values()
        for entries in t.values()
        for e in entries
    }

    def fetch(entry):
        path = entry["path"]
        info = storage(selection["repository"], path)
        expected = (info or {}).get("checksums", {}).get("sha256", "")
        if not re.fullmatch(r"[0-9a-f]{64}", expected):
            raise ValueError(f"Missing published SHA-256: {path}")
        request = {
            "path": entry["file"],
            "urls": [f"{ARTIFACTORY}/api/download/{selection['repository']}/{path}"],
        }
        result = compatibility_vendors.download_artifact(request, products)
        if result["status"] != "downloaded":
            raise ValueError(f"Cannot download {path}: {result['attempts']}")
        if result["sha256"] != expected:
            (products / entry["file"]).unlink(missing_ok=True)
            raise ValueError(f"Published SHA-256 mismatch: {path}")
        return {
            "path": path,
            "url": result["url"],
            "sha256": expected,
            "bytes": result["bytes"],
        }

    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        downloaded = list(pool.map(fetch, requests.values()))
    source = sources(repo, selection["revision"])
    dependencies, exclusions = external_dependencies(source, publications(source))
    year = selection["version"].split(".")[0]
    pom = dependency_pom(
        dependencies,
        exclusions,
        ["release", "development", f"release-{year}", f"development-{year}", "ex-mvn"],
    )
    (extras / "dependencies.pom").write_text(pom)
    manifest = {
        "schema": 1,
        "revision": selection["revision"],
        "platform": platform,
        "targets": targets,
        "source": "artifactory",
        "repository": selection["repository"],
        "version": selection["version"],
        "downloads": downloaded,
    }
    (extras / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    # Compilation is never needed; absence of packaged debug information is reported per library.
    (extras / "complete").write_text("complete\n")


def resolve_classpath(products, log):
    """Resolve transitive dependency JARs using a standalone generated Maven project."""
    extras = products / "compatibility"
    pom = extras / "dependencies.pom"
    if not pom.exists():
        return
    with log.open("w") as output:
        subprocess.run(
            [
                "mvn",
                "--batch-mode",
                "--no-transfer-progress",
                "-f",
                str(pom.resolve()),
                "org.apache.maven.plugins:maven-dependency-plugin:3.8.1:copy-dependencies",
                "-DincludeScope=compile",
                "-DoutputDirectory=" + str((extras / "classpath").resolve()),
            ],
            stdout=output,
            stderr=subprocess.STDOUT,
            timeout=600,
            check=True,
        )
