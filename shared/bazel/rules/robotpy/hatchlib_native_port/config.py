import dataclasses
import pathlib
import typing as T


@dataclasses.dataclass
class PcFileConfig:
    """
    Contents of [[tool.hatch.build.hooks.nativelib.pcfile]] items
    """

    pcfile: str
    """
    File to write pkgconf file to (relative to pyproject.toml)
    """

    description: T.Optional[str] = None
    """Description of this package. If not specified, uses the first line of the package description."""

    name: T.Optional[str] = None
    """Name of this package. If not specified, is basename of pcfile without extension"""

    version: T.Optional[str] = None
    """If not specified, set to package version"""

    includedir: T.Optional[str] = None
    """Where include files can be found (relative to pyproject.toml)"""

    libdir: T.Optional[str] = None
    """Where the library is located. If not specified, it is next to pcfile"""

    shared_libraries: T.Optional[T.List[str]] = None
    """Name of shared libraries located in libdir (without extension)"""

    libs_private: T.Optional[str] = None
    """The link flags for private libraries not exposed to applications"""

    requires: T.Optional[T.List[str]] = None
    """
    Names of other packages this package requires. They must be installed
    at build time.
    """

    requires_private: T.Optional[T.List[str]] = None
    """
    Names of private packages this package requires. They must be installed
    at build time.
    """

    extra_cflags: T.Optional[str] = None
    """A list of extra compiler flags to be added to Cflags after header search path"""

    extra_link_flags: T.Optional[str] = None
    """A list of extra link flags to be added to Libs"""

    variables: T.Optional[T.Dict[str, str]] = None
    """
    Custom variables to add to the generated file. Prefix, libdir, includedir must not be specified."""

    init_module: str = "auto"
    """
    If specified, the name of the python module that will be written next to
    the .pc file which will load the shared_libraries
    """

    enable_if: T.Optional[str] = None
    """
    This is a PEP 508 environment marker specification.

    This pcfile will only be generated if the environment marker matches the current
    build environment
    """

    def get_name(self) -> str:
        if self.name:
            return self.name
        return self.get_pc_path().name[:-3]

    def get_out_path(self) -> pathlib.Path:
        return self.get_pc_path().parent

    def get_pc_path(self) -> pathlib.Path:
        pc_path = pathlib.PurePosixPath(self.pcfile)
        if pc_path.is_absolute():
            raise ValueError(f"pcfile must not be absolute (is {pc_path})")
        if not pc_path.name.endswith(".pc"):
            raise ValueError(f"pcfile must end with .pc (is {pc_path})")
        return pathlib.Path(pc_path)

    def get_init_module(self) -> str:
        if self.init_module == "auto":
            name = self.get_pc_path().name[:-3]
            name = name.replace("-", "_").replace(".", "_")
            module = f"_init_{name}"
        else:
            module = self.init_module

        if not module.isidentifier():
            raise ValueError(
                f"init_module must be a valid python identifier (got {module})"
            )
        return module

    def get_init_module_path(self) -> pathlib.Path:
        module = self.get_init_module()
        return self.get_out_path() / f"{module}.py"
