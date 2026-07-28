import typing as T

import pytest

from wpilib import Alert
from wpilib.simulation import AlertSim

Level = Alert.Level


@pytest.fixture(scope="function")
def group_name(request):
    AlertSim.reset_data()
    group_name = f"AlertTest_{request.node.name}"
    yield group_name
    AlertSim.reset_data()


def get_active_alerts(level: Alert.Level) -> T.List[str]:
    return [a.text for a in AlertSim.get_all() if a.level == level and a.is_active()]


def is_alert_active(text: str, level: Alert.Level):
    matches = [
        a
        for a in AlertSim.get_all()
        if a.level == level and a.text == text and a.is_active()
    ]
    return len(matches) > 0


def assert_state(
    level: Alert.Level,
    expected_state: T.List[str],
):
    assert expected_state == get_active_alerts(level)


def test_no_alerts_initially(group_name):
    assert AlertSim.get_count() == 0
    assert not AlertSim.get_all()


def test_no_alerts_after_reset(group_name):
    with Alert(group_name, "alert", Alert.Level.HIGH) as alert:

        alert.set(True)
        assert is_alert_active("alert", Alert.Level.HIGH)

        AlertSim.reset_data()
        assert AlertSim.get_count() == 0
        assert not AlertSim.get_all()


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
        assert alert.get_text() == "BEFORE"
        alert.set(True)
        assert is_alert_active("BEFORE", Alert.Level.LOW)
        alert.set(False)
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        alert.set_text("AFTER")
        assert alert.get_text() == "AFTER"
        alert.set(True)
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        assert is_alert_active("AFTER", Alert.Level.LOW)


def test_set_text_while_set(group_name):
    with Alert(group_name, "BEFORE", Alert.Level.LOW) as alert:
        assert alert.get_text() == "BEFORE"
        alert.set(True)
        assert is_alert_active("BEFORE", Alert.Level.LOW)
        alert.set_text("AFTER")
        assert alert.get_text() == "AFTER"
        assert not is_alert_active("BEFORE", Alert.Level.LOW)
        assert is_alert_active("AFTER", Alert.Level.LOW)


def test_get_active(group_name):
    with (
        Alert(group_name, "A", Alert.Level.HIGH) as a,
        Alert(group_name, "B", Alert.Level.HIGH) as b,
        Alert(group_name, "C", Alert.Level.HIGH) as c,
    ):

        a.set(True)
        b.set(True)
        c.set(False)

        active = AlertSim.get_active()
        all_alerts = AlertSim.get_all()

        assert len(active) == 2
        assert len(all_alerts) == 3

        active_texts = [a.text for a in active]
        assert set(active_texts) == {"A", "B"}

        a.set(False)
        active = AlertSim.get_active()
        all_alerts = AlertSim.get_all()
        assert len(active) == 1
        assert len(all_alerts) == 3
        assert active[0].text == "B"
