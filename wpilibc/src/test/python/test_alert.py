import typing as T

import pytest

from ntcore import NetworkTableInstance
from wpilib import Alert, SmartDashboard
from wpilib.simulation import pauseTiming, resumeTiming, stepTiming

AlertType = Alert.AlertType


@pytest.fixture(scope="function")
def group_name(nt, request):

    group_name = f"AlertTest_{request.node.name}"
    yield group_name

    SmartDashboard.updateValues()
    assert len(get_active_alerts(nt, group_name, AlertType.kError)) == 0
    assert len(get_active_alerts(nt, group_name, AlertType.kWarning)) == 0
    assert len(get_active_alerts(nt, group_name, AlertType.kInfo)) == 0


def get_subscriber_for_type(
    nt: NetworkTableInstance, group_name: str, alert_type: AlertType
):
    subtable_name = {
        AlertType.kError: "errors",
        AlertType.kWarning: "warnings",
        AlertType.kInfo: "infos",
    }.get(alert_type, "unknown")
    topic = f"/SmartDashboard/{group_name}/{subtable_name}"
    return nt.getStringArrayTopic(topic).subscribe([])


def get_active_alerts(
    nt: NetworkTableInstance, group_name: str, alert_type: AlertType
) -> T.List[str]:
    SmartDashboard.updateValues()
    with get_subscriber_for_type(nt, group_name, alert_type) as sub:
        return sub.get()


def is_alert_active(
    nt: NetworkTableInstance, group_name: str, text: str, alert_type: AlertType
):
    active_alerts = get_active_alerts(nt, group_name, alert_type)
    return text in active_alerts


def assert_state(
    nt: NetworkTableInstance,
    group_name: str,
    alert_type: AlertType,
    expected_state: T.List[str],
):
    assert expected_state == get_active_alerts(nt, group_name, alert_type)


def test_set_unset_single(nt, group_name):
    with Alert(group_name, "one", AlertType.kError) as one:

        assert not is_alert_active(nt, group_name, "one", AlertType.kError)
        assert not is_alert_active(nt, group_name, "two", AlertType.kInfo)

        one.set(True)
        assert is_alert_active(nt, group_name, "one", AlertType.kError)

        one.set(True)
        assert is_alert_active(nt, group_name, "one", AlertType.kError)

        one.set(False)
        assert not is_alert_active(nt, group_name, "one", AlertType.kError)


def test_set_unset_multiple(nt, group_name):
    with (
        Alert(group_name, "one", AlertType.kError) as one,
        Alert(group_name, "two", AlertType.kInfo) as two,
    ):

        assert not is_alert_active(nt, group_name, "one", AlertType.kError)
        assert not is_alert_active(nt, group_name, "two", AlertType.kInfo)

        one.set(True)
        assert is_alert_active(nt, group_name, "one", AlertType.kError)
        assert not is_alert_active(nt, group_name, "two", AlertType.kInfo)

        one.set(True)
        two.set(True)
        assert is_alert_active(nt, group_name, "one", AlertType.kError)
        assert is_alert_active(nt, group_name, "two", AlertType.kInfo)

        one.set(False)
        assert not is_alert_active(nt, group_name, "one", AlertType.kError)
        assert is_alert_active(nt, group_name, "two", AlertType.kInfo)


def test_set_is_idempotent(nt, group_name):
    group_name = group_name
    with (
        Alert(group_name, "A", AlertType.kInfo) as a,
        Alert(group_name, "B", AlertType.kInfo) as b,
        Alert(group_name, "C", AlertType.kInfo) as c,
    ):

        a.set(True)
        b.set(True)
        c.set(True)

        start_state = get_active_alerts(nt, group_name, AlertType.kInfo)
        assert set(start_state) == {"A", "B", "C"}

        b.set(True)
        assert_state(nt, group_name, AlertType.kInfo, start_state)

        a.set(True)
        assert_state(nt, group_name, AlertType.kInfo, start_state)


def test_close_unsets_alert(nt, group_name):
    group_name = group_name
    with Alert(group_name, "alert", AlertType.kWarning) as alert:
        alert.set(True)
        assert is_alert_active(nt, group_name, "alert", AlertType.kWarning)
    assert not is_alert_active(nt, group_name, "alert", AlertType.kWarning)


def test_set_text_while_unset(nt, group_name):
    group_name = group_name
    with Alert(group_name, "BEFORE", AlertType.kInfo) as alert:
        assert alert.getText() == "BEFORE"
        alert.set(True)
        assert is_alert_active(nt, group_name, "BEFORE", AlertType.kInfo)
        alert.set(False)
        assert not is_alert_active(nt, group_name, "BEFORE", AlertType.kInfo)
        alert.setText("AFTER")
        assert alert.getText() == "AFTER"
        alert.set(True)
        assert not is_alert_active(nt, group_name, "BEFORE", AlertType.kInfo)
        assert is_alert_active(nt, group_name, "AFTER", AlertType.kInfo)


def test_set_text_while_set(nt, group_name):
    with Alert(group_name, "BEFORE", AlertType.kInfo) as alert:
        assert alert.getText() == "BEFORE"
        alert.set(True)
        assert is_alert_active(nt, group_name, "BEFORE", AlertType.kInfo)
        alert.setText("AFTER")
        assert alert.getText() == "AFTER"
        assert not is_alert_active(nt, group_name, "BEFORE", AlertType.kInfo)
        assert is_alert_active(nt, group_name, "AFTER", AlertType.kInfo)


def test_set_text_does_not_affect_sort(nt, group_name):
    pauseTiming()
    try:
        with (
            Alert(group_name, "A", AlertType.kInfo) as a,
            Alert(group_name, "B", AlertType.kInfo) as b,
            Alert(group_name, "C", AlertType.kInfo) as c,
        ):

            a.set(True)
            stepTiming(1)
            b.set(True)
            stepTiming(1)
            c.set(True)

            expected_state = get_active_alerts(nt, group_name, AlertType.kInfo)
            expected_state[expected_state.index("B")] = "AFTER"

            b.setText("AFTER")
            assert_state(nt, group_name, AlertType.kInfo, expected_state)
    finally:
        resumeTiming()


def test_sort_order(nt, group_name):
    pauseTiming()
    try:
        with (
            Alert(group_name, "A", AlertType.kInfo) as a,
            Alert(group_name, "B", AlertType.kInfo) as b,
            Alert(group_name, "C", AlertType.kInfo) as c,
        ):

            a.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["A"])

            stepTiming(1)
            b.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["B", "A"])

            stepTiming(1)
            c.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["C", "B", "A"])

            stepTiming(1)
            c.set(False)
            assert_state(nt, group_name, AlertType.kInfo, ["B", "A"])

            stepTiming(1)
            c.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["C", "B", "A"])

            stepTiming(1)
            a.set(False)
            assert_state(nt, group_name, AlertType.kInfo, ["C", "B"])

            stepTiming(1)
            b.set(False)
            assert_state(nt, group_name, AlertType.kInfo, ["C"])

            stepTiming(1)
            b.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["B", "C"])

            stepTiming(1)
            a.set(True)
            assert_state(nt, group_name, AlertType.kInfo, ["A", "B", "C"])
    finally:
        resumeTiming()
