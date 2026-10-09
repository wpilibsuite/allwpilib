import pytest

import ntcore
from wpiutil import Alert


def test_alert_backend_startup_and_lifetime():
    # Follow the startup-only installation contract for this test process.
    instance = ntcore.NetworkTableInstance.create()
    other = ntcore.NetworkTableInstance.create()
    try:
        ntcore.install_alert_backend(instance, "coprocessor//alerts/")
        text_topic = instance.get_string_topic("/coprocessor/alerts/group/1/id/text")
        active_topic = instance.get_integer_topic(
            "/coprocessor/alerts/group/1/id/active"
        )
        text = text_topic.subscribe("")
        active = active_topic.subscribe(
            -1, ntcore.PubSubOptions(keep_duplicates=True, send_all=True)
        )
        with Alert("group", "id", "initial", Alert.Level.MEDIUM) as alert:
            assert not alert.get()
            assert text.get() == "initial"
            assert active.get() == 0
            assert not other.get_topic(text_topic.get_name()).exists()
            with pytest.raises(RuntimeError):
                Alert("group", "id", "duplicate", Alert.Level.MEDIUM)
            alert.set_text("inactive edit")
            assert text.get() == "inactive edit"
            alert.set(True)
            activation = active.get()
            assert activation > 0
            alert.set(True)
            alert.set_text("active edit")
            assert active.get() == activation
            assert text.get() == "active edit"
            alert.set(False)
            assert [change.value for change in active.read_queue()] == [
                0,
                activation,
                0,
            ]
        assert not text_topic.exists()
        assert not active_topic.exists()
        with Alert("shutdown", "id", "text", Alert.Level.LOW) as shutdown:
            instance._reset()
            shutdown.set(True)
            assert not shutdown.get()
            with pytest.raises(RuntimeError):
                Alert("new", "text", Alert.Level.LOW)
    finally:
        ntcore.NetworkTableInstance.destroy(instance)
        ntcore.NetworkTableInstance.destroy(other)
