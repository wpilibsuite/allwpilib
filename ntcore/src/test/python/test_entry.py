#
# Ensure that the NetworkTableEntry objects work
#


def test_entry_repr(nt):
    e = nt.getEntry("/k1")
    assert repr(e) == "<NetworkTableEntry '/k1'>"


def test_topic_repr(nt):
    topic = nt.getIntegerTopic("/int")
    assert repr(topic) == "<IntegerTopic '/int'>"

    pub = topic.publish()
    assert repr(pub) == "<IntegerPublisher '/int'>"

    entry = topic.getEntry(0)
    assert repr(entry) == "<IntegerEntry '/int'>"

    generic_entry = topic.getGenericEntry()
    assert repr(generic_entry) == "<GenericEntry '/int'>"


def test_entry_string(nt):
    e = nt.getEntry("/k1")
    assert e.getString(None) is None
    e.setString("value")
    assert e.getString(None) == "value"
    assert e.getValue().value() == "value"
    assert e.value == "value"
    e.unpublish()
    assert e.getString(None) is None
    e.setString("value")
    assert e.getString(None) == "value"


def test_entry_string_array(nt):
    e = nt.getEntry("/k1")
    assert e.getStringArray(None) is None
    e.setStringArray(["value"])
    assert e.getStringArray(None) == ["value"]
    assert e.getValue().value() == ["value"]
    assert e.value == ["value"]
    e.unpublish()
    assert e.getStringArray(None) is None
    e.setStringArray(["value"])
    assert e.getStringArray(None) == ["value"]


def test_entry_persistence(nt):
    e = nt.getEntry("/k2")

    for _ in range(2):
        assert not e.isPersistent()
        # persistent flag cannot be set unless the entry has a value
        e.setString("value")

        assert not e.isPersistent()
        e.setPersistent()
        assert e.isPersistent()
        e.clearPersistent()
        assert not e.isPersistent()

        e.unpublish()


def test_entry_publish_empty_double_array(nt):
    topic = nt.getDoubleArrayTopic("/Topic")
    pub = topic.publish()
    pub.set([])
