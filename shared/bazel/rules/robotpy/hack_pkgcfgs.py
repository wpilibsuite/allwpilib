import os
import pathlib
from typing import List


def hack_pkgconfig(pkgcfgs: List[pathlib.Path]):
    """
    This will place the given files in the PKG_CONFIG_PATH in such a way that will trick
    semiwrap into thinking the libraries have been installed
    """

    pkg_config_paths = os.environ.get("PKG_CONFIG_PATH", "").split(os.pathsep)

    if pkgcfgs:
        for pc in pkgcfgs:
            # pkg_config_paths.append(str(pc.parent.absolute()))
            pkg_config_paths.append(str(pc.parent))

    os.environ["PKG_CONFIG_PATH"] = os.pathsep.join(pkg_config_paths)
