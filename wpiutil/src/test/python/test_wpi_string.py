from wpiutil_test import module


def test_wpi_string_load():
    assert "Hello World" == module.load_wpi_string("Hello World")


def test_wpi_string_cast():
    assert "Hello WPI_String!" == module.cast_wpi_string()


def test_wpi_string_in_struct_cast():
    the_struct = module.cast_struct_with_wpi_string()
    assert 3504 == the_struct.x
    assert "I'm in a struct!" == the_struct.str
