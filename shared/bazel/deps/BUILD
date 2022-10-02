load("@rules_python//python:pip.bzl", "compile_pip_requirements")

# This rule adds a convenient way to update the requirements file.
compile_pip_requirements(
    name = "python_lock",
    extra_args = ["--allow-unsafe"],
    requirements_in = "//:requirements.txt",
    requirements_txt = ":requirements_lock.txt",
    tags = ["manual"],
)
