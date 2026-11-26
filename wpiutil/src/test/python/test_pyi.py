import pathlib

import wpiutil._wpiutil


def test_compiled_extension_has_type_stubs():
    extension_path = pathlib.Path(wpiutil._wpiutil.__file__)
    stub_dir = extension_path.with_name("_wpiutil")

    for stub_name in ("__init__.pyi", "sync.pyi", "wpistruct.pyi"):
        stub_path = stub_dir / stub_name
        assert stub_path.is_file()
        assert stub_path.stat().st_size > 0
