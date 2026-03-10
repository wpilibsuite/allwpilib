import typing as T

import pytest

from wpilib import Alert
from wpilib.simulation import AlertSim

Level = Alert.Level


@pytest.fixture(scope="function")
def group_name(request):

    group_name = f"AlertTest_{request.node.name}"
    yield group_name

    AlertSim.resetData()


def get_active_alerts(level: Alert.Level) -> T.List[str]:
    return [
        a.text
        for a in AlertSim.getAll()
        if a.level == level and a.isActive()
    ]


def is_alert_active(text: str, level: Alert.Level):
    matches = [
        a
        for a in AlertSim.getAll()
        if a.level == level and a.text == text and a.isActive()
    ]
    return len(matches) > 0


def assert_state(
    level: Alert.Level,
    expected_state: T.List[str],
):
    assert expected_state == get_active_alerts(level)


def test_no_alerts_initially(group_name):
    assert AlertSim.getCount() == 0
    assert not AlertSim.getAll()


def test_no_alerts_after_reset(group_name):
    with Alert(group_name, "alert", Alert.Level.HIGH) as alert:

        alert.set(True)
        assert is_alert_active("alert", Alert.Level.HIGH)

        AlertSim.resetData()
        assert AlertSim.getCount() == 0
        assert not AlertSim.getAll()


def test_set_unset_single(group_name):
    with Alert(group_name, "one", Alert.Level.HIGH) as one:

        assert not is_alert_active("one", Alert.Level.HIGH)

        one.set(True)
        assert is_alert_active("one", Alert.Level.HIGH)

        one.set(False)
        assert not is_alert_active("one", Alert.Level.HIGH)


def test_set_unset_multiple(group_name):
    with (
        Alert(group_name, "one", Alert.Level.HIGH) as one,
        Alert(group_name, "two", Alert.Level.LOW) as two,
    ):

        assert not is_alert_active("one", Alert.Level.HIGH)
        assert not is_alert_active("two", Alert.Level.LOW)

        one.set(True)
        assert is_alert_active("one", Alert.Level.HIGH)
        assert not is_alert_active("two", Alert.Level.LOW)

        one.set(True)
        two.set(True)
        assert is_alert_active("one", Alert.Level.HIGH)
        assert is_alert_active("two", Alert.Level.LOW)

        one.set(False)
        assert not is_alert_active("one", Alert.Level.HIGH)
        assert is_alert_active("two", Alert.Level.LOW)


def test_set_is_idempotent(group_name):
    with (
        Alert(group_name, "A", Alert.Level.LOW) as a,
        Alert(group_name, "B", Alert.Level.LOW) as b,
        Alert(group_name, "C", Alert.Level.LOW) as c,
    ):

        a.set(True)
        b.set(True)
        c.set(True)

        start_state = get_active_alerts(Alert.Level.LOW)
        assert set(start_state) == {"A", "B", "C"}

        b.set(True)
        assert_state(Alert.Level.LOW, start_state)

        a.set(True)
        assert_state(Alert.Level.LOW, start_state)


def test_close_unsets_alert(group_name):
    with Alert(group_name, "alert", Alert.Level.MEDIUM) as alert:
        alert.set(True)
        assert is_alert_active("alert", Alert.Level.MEDIUM)
    assert not is_alert_active("alert", Alert.Level.MEDIUM)


def test_set_text_while_unset(group_name):
    with Alert(group_name, "BEFORE", Alert.Level.LOW) as alert:
        assert alert.getText() == "BEFORE"
        alert.set(True)
        assert is_alert_active("BEFORE", Alert.Level.LOW)
        alert.set(False)
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        alert.setText("AFTER")
        assert alert.getText() == "AFTER"
        alert.set(True)
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        assert is_alert_active("AFTER", Alert.Level.LOW)


def test_set_text_while_set(group_name):
    with Alert(group_name, "BEFORE", Alert.Level.LOW) as alert:
        assert alert.getText() == "BEFORE"
        alert.set(True)
        assert is_alert_active("BEFORE", Alert.Level.LOW)
        alert.setText("AFTER")
        assert alert.getText() == "AFTER"
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        assert is_alert_active("AFTER", Alert.Level.LOW)
