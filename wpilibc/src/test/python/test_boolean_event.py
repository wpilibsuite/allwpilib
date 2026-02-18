from wpilib import BooleanEvent, EventLoop


def test_binary_compositions():
    loop = EventLoop()
    and_counter = {"value": 0}
    or_counter = {"value": 0}

    def inc_and():
        and_counter["value"] += 1

    def inc_or():
        or_counter["value"] += 1

    assert and_counter["value"] == 0
    assert or_counter["value"] == 0

    BooleanEvent(loop, lambda: True).and_(lambda: False).ifHigh(inc_and)
    BooleanEvent(loop, lambda: True).or_(lambda: False).ifHigh(inc_or)

    loop.poll()

    assert and_counter["value"] == 0
    assert or_counter["value"] == 1


def test_binary_compositions_with_edge_decorators():
    loop = EventLoop()
    bool1 = {"value": False}
    bool2 = {"value": False}
    bool3 = {"value": False}
    bool4 = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    event1 = BooleanEvent(loop, lambda: bool1["value"]).rising()
    event2 = BooleanEvent(loop, lambda: bool2["value"]).rising()
    event3 = BooleanEvent(loop, lambda: bool3["value"]).rising()
    event4 = BooleanEvent(loop, lambda: bool4["value"]).rising()
    event1.and_(event2).ifHigh(inc)
    event3.or_(event4).ifHigh(inc)
    assert counter["value"] == 0

    bool1["value"] = True
    bool2["value"] = True
    bool3["value"] = True
    bool4["value"] = True
    loop.poll()

    assert counter["value"] == 2

    loop.poll()

    assert counter["value"] == 2

    bool1["value"] = False
    bool2["value"] = False
    bool3["value"] = False
    bool4["value"] = False
    loop.poll()

    assert counter["value"] == 2

    bool1["value"] = True
    loop.poll()

    assert counter["value"] == 2

    bool2["value"] = True
    loop.poll()

    assert counter["value"] == 2

    bool1["value"] = False
    bool2["value"] = False
    loop.poll()

    assert counter["value"] == 2

    bool1["value"] = True
    bool2["value"] = True
    loop.poll()

    assert counter["value"] == 3

    bool3["value"] = True
    loop.poll()

    assert counter["value"] == 4

    loop.poll()

    assert counter["value"] == 4

    bool4["value"] = True
    loop.poll()

    assert counter["value"] == 5

    loop.poll()

    assert counter["value"] == 5


def test_binary_composition_loop_semantics():
    loop1 = EventLoop()
    loop2 = EventLoop()
    bool1 = {"value": True}
    bool2 = {"value": True}
    counter1 = {"value": 0}
    counter2 = {"value": 0}

    def inc1():
        counter1["value"] += 1

    def inc2():
        counter2["value"] += 1

    BooleanEvent(loop1, lambda: bool1["value"]).and_(
        BooleanEvent(loop2, lambda: bool2["value"])
    ).ifHigh(inc1)

    BooleanEvent(loop2, lambda: bool2["value"]).and_(
        BooleanEvent(loop1, lambda: bool1["value"])
    ).ifHigh(inc2)

    assert counter1["value"] == 0
    assert counter2["value"] == 0

    loop1.poll()

    assert counter1["value"] == 1
    assert counter2["value"] == 0

    loop2.poll()

    assert counter1["value"] == 1
    assert counter2["value"] == 1

    bool2["value"] = False
    loop1.poll()

    assert counter1["value"] == 2
    assert counter2["value"] == 1

    loop2.poll()

    assert counter1["value"] == 2
    assert counter2["value"] == 1

    loop1.poll()

    assert counter1["value"] == 2
    assert counter2["value"] == 1

    bool2["value"] = True
    loop2.poll()

    assert counter1["value"] == 2
    assert counter2["value"] == 2

    loop1.poll()

    assert counter1["value"] == 3
    assert counter2["value"] == 2

    bool1["value"] = False
    loop2.poll()

    assert counter1["value"] == 3
    assert counter2["value"] == 3

    loop1.poll()

    assert counter1["value"] == 3
    assert counter2["value"] == 3

    loop2.poll()

    assert counter1["value"] == 3
    assert counter2["value"] == 3


def test_poll_ordering():
    loop = EventLoop()
    bool1 = {"value": True}
    bool2 = {"value": True}
    enable_assert = {"value": False}
    counter = {"value": 0}

    def action1():
        if enable_assert["value"]:
            counter["value"] += 1
            assert counter["value"] % 3 == 1
        return bool1["value"]

    def action2():
        if enable_assert["value"]:
            counter["value"] += 1
            assert counter["value"] % 3 == 2
        return bool2["value"]

    def action3():
        if enable_assert["value"]:
            counter["value"] += 1
            assert counter["value"] % 3 == 0

    BooleanEvent(loop, action1).and_(BooleanEvent(loop, action2)).ifHigh(action3)
    enable_assert["value"] = True
    loop.poll()
    loop.poll()
    loop.poll()
    loop.poll()


def test_edge_decorators():
    loop = EventLoop()
    flag = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    def dec():
        counter["value"] -= 1

    BooleanEvent(loop, lambda: flag["value"]).falling().ifHigh(dec)
    BooleanEvent(loop, lambda: flag["value"]).rising().ifHigh(inc)

    assert counter["value"] == 0

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 0

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 1

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 1

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 0


def test_edge_reuse():
    loop = EventLoop()
    flag = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    event = BooleanEvent(loop, lambda: flag["value"]).rising()
    event.ifHigh(inc)
    event.ifHigh(inc)

    assert counter["value"] == 0

    loop.poll()

    assert counter["value"] == 0

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 2

    loop.poll()

    assert counter["value"] == 2

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 2

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 4


def test_edge_reconstruct():
    loop = EventLoop()
    flag = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    event = BooleanEvent(loop, lambda: flag["value"])
    event.rising().ifHigh(inc)
    event.rising().ifHigh(inc)

    assert counter["value"] == 0

    loop.poll()

    assert counter["value"] == 0

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 2

    loop.poll()

    assert counter["value"] == 2

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 2

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 4


def test_mid_loop_boolean_change():
    loop = EventLoop()
    flag = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    event = BooleanEvent(loop, lambda: flag["value"]).rising()

    def first_action():
        flag["value"] = False
        counter["value"] += 1

    event.ifHigh(first_action)
    event.ifHigh(inc)

    assert counter["value"] == 0

    loop.poll()

    assert counter["value"] == 0

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 2

    loop.poll()

    assert counter["value"] == 2

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 2

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 4


def test_mid_loop_boolean_change_with_composed_events():
    loop = EventLoop()
    bool1 = {"value": False}
    bool2 = {"value": False}
    bool3 = {"value": False}
    bool4 = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    event1 = BooleanEvent(loop, lambda: bool1["value"])
    event2 = BooleanEvent(loop, lambda: bool2["value"])
    event3 = BooleanEvent(loop, lambda: bool3["value"])
    event4 = BooleanEvent(loop, lambda: bool4["value"])

    def action1():
        bool2["value"] = False
        bool3["value"] = False
        counter["value"] += 1

    event1.ifHigh(action1)

    def action2():
        bool1["value"] = False
        counter["value"] += 1

    event3.or_(event4).ifHigh(action2)

    def action3():
        bool4["value"] = False
        counter["value"] += 1

    event1.and_(event2).ifHigh(action3)

    assert counter["value"] == 0

    bool1["value"] = True
    bool2["value"] = True
    bool3["value"] = True
    bool4["value"] = True
    loop.poll()

    assert counter["value"] == 3

    loop.poll()

    assert counter["value"] == 3

    bool1["value"] = True
    bool2["value"] = True
    loop.poll()

    assert counter["value"] == 5

    bool1["value"] = False
    loop.poll()

    assert counter["value"] == 5

    bool1["value"] = True
    bool3["value"] = True
    loop.poll()

    assert counter["value"] == 7

    bool1["value"] = False
    bool4["value"] = True
    loop.poll()

    assert counter["value"] == 8


def test_negation():
    loop = EventLoop()
    flag = {"value": False}
    counter = {"value": 0}

    def inc():
        counter["value"] += 1

    BooleanEvent(loop, lambda: flag["value"]).negate().ifHigh(inc)

    assert counter["value"] == 0

    loop.poll()

    assert counter["value"] == 1

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 1

    flag["value"] = False
    loop.poll()

    assert counter["value"] == 2

    flag["value"] = True
    loop.poll()

    assert counter["value"] == 2
