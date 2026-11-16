import wpiutil
import pytest


@pytest.fixture(autouse=True)
def reset_report_usage_impl():
    wpiutil.set_report_usage_impl(None)
    yield
    wpiutil.set_report_usage_impl(None)


def test_default():
    wpiutil.report_usage("Resource", "Data")


def test_custom_report_usage():
    reports = []

    wpiutil.set_report_usage_impl(
        lambda resource, data: reports.append((resource, data))
    )
    wpiutil.report_usage("Resource", "Data")

    assert reports == [("Resource", "Data")]


def test_report_usage_instance():
    reports = []

    wpiutil.set_report_usage_impl(
        lambda resource, data: reports.append((resource, data))
    )
    wpiutil.report_usage("Resource", 3, "Data")

    assert reports == [("Resource[3]", "Data")]
