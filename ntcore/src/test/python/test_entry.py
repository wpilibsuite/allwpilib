#
# Ensure that the NetworkTableEntry objects work
#


def test_entry_repr(nt):
    e = nt.get_entry("/k1")
    assert repr(e) == "<NetworkTableEntry '/k1'>"


def test_topic_repr(nt):
    topic = nt.get_integer_topic("/int")
    assert repr(topic) == "<IntegerTopic '/int'>"

    pub = topic.publish()
    assert repr(pub) == "<IntegerPublisher '/int'>"

    entry = topic.get_entry(0)
    assert repr(entry) == "<IntegerEntry '/int'>"

    generic_entry = topic.get_generic_entry()
    assert repr(generic_entry) == "<GenericEntry '/int'>"


def test_entry_string(nt):
    e = nt.get_entry("/k1")
    assert e.get_string(None) is None
    e.set_string("value")
    assert e.get_string(None) == "value"
    assert e.get_value().value() == "value"
    assert e.value == "value"
    e.unpublish()
    assert e.get_string(None) is None
    e.set_string("value")
    assert e.get_string(None) == "value"


def test_entry_string_array(nt):
    e = nt.get_entry("/k1")
    assert e.get_string_array(None) is None
    e.set_string_array(["value"])
    assert e.get_string_array(None) == ["value"]
    assert e.get_value().value() == ["value"]
    assert e.value == ["value"]
    e.unpublish()
    assert e.get_string_array(None) is None
    e.set_string_array(["value"])
    assert e.get_string_array(None) == ["value"]


def test_entry_persistence(nt):
    e = nt.get_entry("/k2")

    for _ in range(2):
        assert not e.is_persistent()
        # persistent flag cannot be set unless the entry has a value
        e.set_string("value")

        assert not e.is_persistent()
        e.set_persistent()
        assert e.is_persistent()
        e.clear_persistent()
        assert not e.is_persistent()

        e.unpublish()


def test_entry_publish_empty_double_array(nt):
    topic = nt.get_double_array_topic("/Topic")
    pub = topic.publish()
    pub.set([])
