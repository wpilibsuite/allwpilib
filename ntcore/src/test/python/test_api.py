#
# These tests stand up a separate client and server instance of
# networktables and tests the 'real' user API to ensure that it
# works correctly
#

import pytest

import logging

logger = logging.getLogger("test")


# test defaults
def doc(nt):
    t = nt.get_table("nope")

    assert t.get_boolean("b", None) is None
    assert t.get_number("n", None) is None
    assert t.get_string("s", None) is None
    assert t.get_boolean_array("ba", None) is None
    assert t.get_number_array("na", None) is None
    assert t.get_string_array("sa", None) is None
    assert t.get_value("v", None) is None

    assert t.get_boolean("b", True) is True
    assert t.get_number("n", 1) == 1
    assert t.get_string("s", "sss") == "sss"
    assert t.get_boolean_array("ba", (True,)) == (True,)
    assert t.get_number_array("na", (1,)) == (1,)
    assert t.get_string_array("sa", ("ss",)) == ("ss",)
    assert t.get_value("v", "vvv") == "vvv"


def do(nt1, nt2, t):
    t1 = nt1.get_table(t)
    with nt2.expect_changes(8):
        t1.put_boolean("bool", True)
        t1.put_number("number1", 1)
        t1.put_number("number2", 1.5)
        t1.put_string("string", "string")
        t1.put_string("unicode", "\xa9")  # copyright symbol
        t1.put_boolean_array("ba", (True, False))
        t1.put_number_array("na", (1, 2))
        t1.put_string_array("sa", ("s", "t"))
        logger.info("put is done")

    t2 = nt2.get_table(t)
    assert t2.get_boolean("bool", None) is True
    assert t2.get_number("number1", None) == 1
    assert t2.get_number("number2", None) == 1.5
    assert t2.get_string("string", None) == "string"
    assert t2.get_string("unicode", None) == "\xa9"  # copyright symbol
    assert t2.get_boolean_array("ba", None) == [True, False]
    assert t2.get_number_array("na", None) == [1, 2]
    assert t2.get_string_array("sa", None) == ["s", "t"]

    # Value testing
    with nt2.expect_changes(6):
        t1.put_value("v_b", False)
        t1.put_value("v_n1", 2)
        t1.put_value("v_n2", 2.5)
        t1.put_value("v_s", "ssss")
        t1.put_value("v_s2", "\xa9")

        t1.put_value("v_v", 0)

    print(t2.get_keys())
    assert t2.get_boolean("v_b", None) is False
    assert t2.get_number("v_n1", None) == 2
    assert t2.get_number("v_n2", None) == 2.5
    assert t2.get_string("v_s", None) == "ssss"
    assert t2.get_string("v_s2", None) == "\xa9"
    assert t2.get_value("v_v", None) == 0

    # Ensure that updating values work!
    with nt2.expect_changes(8):
        t1.put_boolean("bool", False)
        t1.put_number("number1", 2)
        t1.put_number("number2", 2.5)
        t1.put_string("string", "sss")
        t1.put_string("unicode", "\u2122")  # (tm)
        t1.put_boolean_array("ba", (False, True, False))
        t1.put_number_array("na", (2, 1))
        t1.put_string_array("sa", ("t", "s"))

    t2 = nt2.get_table(t)
    assert t2.get_boolean("bool", None) is False
    assert t2.get_number("number1", None) == 2
    assert t2.get_number("number2", None) == 2.5
    assert t2.get_string("string", None) == "sss"
    assert t2.get_string("unicode", None) == "\u2122"
    assert t2.get_boolean_array("ba", None) == [False, True, False]
    assert t2.get_number_array("na", None) == [2, 1]
    assert t2.get_string_array("sa", None) == ["t", "s"]


@pytest.mark.xfail(reason="ntcore is broken")
def test_basic(nt_live):
    nt_server, nt_client = nt_live

    # assert nt_server.is_server()
    # assert not nt_client.is_server()

    doc(nt_client)
    doc(nt_server)

    # server -> client
    do(nt_server, nt_client, "server2client")

    # client -> server
    do(nt_client, nt_server, "client2server")

    assert nt_client.is_connected()
    assert nt_server.is_connected()


# def test_reconnect(nt_live):

#     nt_server, nt_client = nt_live

#     with nt_server.expect_changes(1):
#         ct = nt_client.get_table("t")
#         ct.put_boolean("foo", True)

#     st = nt_server.get_table("t")
#     assert st.get_boolean("foo", None) == True

#     # Client disconnect testing
#     nt_client.shutdown()

#     logger.info("Shutdown the client")

#     with nt_client.expect_changes(1):
#         nt_client.start_test()
#         ct = nt_client.get_table("t")

#     assert ct.get_boolean("foo", None) == True

#     # Server disconnect testing
#     nt_server.shutdown()
#     logger.info("Shutdown the server")

#     # synchronization change: if the client doesn't touch the entry locally,
#     # then it won't get transferred back to the server on reconnect. Touch
#     # it here to ensure that it comes back
#     ct.put_boolean("foo", True)

#     with nt_server.expect_changes(1):
#         nt_server.start_test()

#     st = nt_server.get_table("t")
#     assert st.get_boolean("foo", None) == True
