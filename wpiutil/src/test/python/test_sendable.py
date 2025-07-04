import typing
import wpiutil
from wpiutil_test import module


class MySendable(wpiutil.Sendable):
    def __init__(self):
        super().__init__()
        wpiutil.SendableRegistry.add(self, "Test", 1)
        self.value = 0

    def initSendable(self, builder: wpiutil.SendableBuilder):
        builder.addDoubleProperty("key", self._get, self._set)

    def _set(self, value: float):
        self.value = value

    def _get(self) -> float:
        return self.value


def test_custom_sendable():
    ms = MySendable()

    uid = wpiutil.SendableRegistry.getUniqueId(ms)
    keys = {}

    module.publish(uid, keys)
    assert ms.value == 0

    getter, setter = keys["key"]
    assert getter() == 0
    setter(1)
    assert getter() == 1
    assert ms.value == 1
