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
    t = nt.getTable("nope")

    assert t.getBoolean("b", None) is None
    assert t.getNumber("n", None) is None
    assert t.getString("s", None) is None
    assert t.getBooleanArray("ba", None) is None
    assert t.getNumberArray("na", None) is None
    assert t.getStringArray("sa", None) is None
    assert t.getValue("v", None) is None

    assert t.getBoolean("b", True) is True
    assert t.getNumber("n", 1) == 1
    assert t.getString("s", "sss") == "sss"
    assert t.getBooleanArray("ba", (True,)) == (True,)
    assert t.getNumberArray("na", (1,)) == (1,)
    assert t.getStringArray("sa", ("ss",)) == ("ss",)
    assert t.getValue("v", "vvv") == "vvv"


def do(nt1, nt2, t):
    t1 = nt1.getTable(t)
    with nt2.expect_changes(8):
        t1.putBoolean("bool", True)
        t1.putNumber("number1", 1)
        t1.putNumber("number2", 1.5)
        t1.putString("string", "string")
        t1.putString("unicode", "\xa9")  # copyright symbol
        t1.putBooleanArray("ba", (True, False))
        t1.putNumberArray("na", (1, 2))
        t1.putStringArray("sa", ("s", "t"))
        logger.info("put is done")

    t2 = nt2.getTable(t)
    assert t2.getBoolean("bool", None) is True
    assert t2.getNumber("number1", None) == 1
    assert t2.getNumber("number2", None) == 1.5
    assert t2.getString("string", None) == "string"
    assert t2.getString("unicode", None) == "\xa9"  # copyright symbol
    assert t2.getBooleanArray("ba", None) == [True, False]
    assert t2.getNumberArray("na", None) == [1, 2]
    assert t2.getStringArray("sa", None) == ["s", "t"]

    # Value testing
    with nt2.expect_changes(6):
        t1.putValue("v_b", False)
        t1.putValue("v_n1", 2)
        t1.putValue("v_n2", 2.5)
        t1.putValue("v_s", "ssss")
        t1.putValue("v_s2", "\xa9")

        t1.putValue("v_v", 0)

    print(t2.getKeys())
    assert t2.getBoolean("v_b", None) is False
    assert t2.getNumber("v_n1", None) == 2
    assert t2.getNumber("v_n2", None) == 2.5
    assert t2.getString("v_s", None) == "ssss"
    assert t2.getString("v_s2", None) == "\xa9"
    assert t2.getValue("v_v", None) == 0

    # Ensure that updating values work!
    with nt2.expect_changes(8):
        t1.putBoolean("bool", False)
        t1.putNumber("number1", 2)
        t1.putNumber("number2", 2.5)
        t1.putString("string", "sss")
        t1.putString("unicode", "\u2122")  # (tm)
        t1.putBooleanArray("ba", (False, True, False))
        t1.putNumberArray("na", (2, 1))
        t1.putStringArray("sa", ("t", "s"))

    t2 = nt2.getTable(t)
    assert t2.getBoolean("bool", None) is False
    assert t2.getNumber("number1", None) == 2
    assert t2.getNumber("number2", None) == 2.5
    assert t2.getString("string", None) == "sss"
    assert t2.getString("unicode", None) == "\u2122"
    assert t2.getBooleanArray("ba", None) == [False, True, False]
    assert t2.getNumberArray("na", None) == [2, 1]
    assert t2.getStringArray("sa", None) == ["t", "s"]


@pytest.mark.xfail(reason="ntcore is broken")
def test_basic(nt_live):
    nt_server, nt_client = nt_live

    # assert nt_server.isServer()
    # assert not nt_client.isServer()

    doc(nt_client)
    doc(nt_server)

    # server -> client
    do(nt_server, nt_client, "server2client")

    # client -> server
    do(nt_client, nt_server, "client2server")

    assert nt_client.isConnected()
    assert nt_server.isConnected()


# def test_reconnect(nt_live):

#     nt_server, nt_client = nt_live

#     with nt_server.expect_changes(1):
#         ct = nt_client.getTable("t")
#         ct.putBoolean("foo", True)

#     st = nt_server.getTable("t")
#     assert st.getBoolean("foo", None) == True

#     # Client disconnect testing
#     nt_client.shutdown()

#     logger.info("Shutdown the client")

#     with nt_client.expect_changes(1):
#         nt_client.start_test()
#         ct = nt_client.getTable("t")

#     assert ct.getBoolean("foo", None) == True

#     # Server disconnect testing
#     nt_server.shutdown()
#     logger.info("Shutdown the server")

#     # synchronization change: if the client doesn't touch the entry locally,
#     # then it won't get transferred back to the server on reconnect. Touch
#     # it here to ensure that it comes back
#     ct.putBoolean("foo", True)

#     with nt_server.expect_changes(1):
#         nt_server.start_test()

#     st = nt_server.getTable("t")
#     assert st.getBoolean("foo", None) == True
