# validated: 2024-01-20 DS 7a099cb02a33 button/NetworkButton.java
from typing import overload

from ntcore import BooleanSubscriber, BooleanTopic, NetworkTable, NetworkTableInstance

from ..util import format_args_kwargs
from .trigger import Trigger


class NetworkButton(Trigger):
    """
    A Button that uses a :class:`ntcore.NetworkTable` boolean field.
    """

    @overload
    def __init__(self, topic: BooleanTopic) -> None:
        """
        Creates a NetworkButton that commands can be bound to.

        :param topic: The boolean topic that contains the value.
        """
        pass

    @overload
    def __init__(self, sub: BooleanSubscriber) -> None:
        """
        Creates a NetworkButton that commands can be bound to.

        :param sub: The boolean subscriber that provides the value.
        """
        pass

    @overload
    def __init__(self, table: NetworkTable, field: str) -> None:
        """
        Creates a NetworkButton that commands can be bound to.

        :param table: The table where the networktable value is located.
        :param field: The field that is the value.
        """
        pass

    @overload
    def __init__(self, table: str, field: str) -> None:
        """
        Creates a NetworkButton that commands can be bound to.

        :param table: The table where the networktable value is located.
        :param field: The field that is the value.
        """
        pass

    @overload
    def __init__(self, inst: NetworkTableInstance, table: str, field: str) -> None:
        """
        Creates a NetworkButton that commands can be bound to.

        :param inst: The NetworkTable instance to use
        :param table: The table where the networktable value is located.
        :param field: the field that is the value.
        """
        pass

    def __init__(self, *args, **kwargs) -> None:
        def init_sub(sub: BooleanSubscriber):
            return super(NetworkButton, self).__init__(
                lambda: sub.getTopic().getInstance().isConnected() and sub.get()
            )

        def init_topic(topic: BooleanTopic):
            init_sub(topic.subscribe(False))

        def init_table_field(table: NetworkTable, field: str):
            init_topic(table.getBooleanTopic(field))

        def init_inst_table_field(inst: NetworkTableInstance, table: str, field: str):
            init_table_field(inst.getTable(table), field)

        def init_str_table_field(table: str, field: str):
            init_inst_table_field(NetworkTableInstance.getDefault(), table, field)

        num_args = len(args) + len(kwargs)

        if num_args == 1:
            if "topic" in kwargs:
                return init_topic(kwargs["topic"])
            if "sub" in kwargs:
                return init_sub(kwargs["sub"])
            if isinstance(args[0], BooleanTopic):
                return init_topic(args[0])
            if isinstance(args[0], BooleanSubscriber):
                return init_sub(args[0])
        elif num_args == 2:
            table, field, *_ = args + (None, None)
            if "table" in kwargs:
                table = kwargs["table"]
            if "field" in kwargs:
                field = kwargs["field"]
            if table is not None and field is not None:
                if isinstance(table, NetworkTable):
                    return init_table_field(table, field)
                if isinstance(table, str):
                    return init_str_table_field(table, field)
        elif num_args == 3:
            inst, table, field, *_ = args + (None, None, None)
            if "inst" in kwargs:
                inst = kwargs["inst"]
            if "table" in kwargs:
                table = kwargs["table"]
            if "field" in kwargs:
                field = kwargs["field"]
            if inst is not None and table is not None and field is not None:
                return init_inst_table_field(inst, table, field)

        raise TypeError(
            f"""
TypeError: NetworkButton(): incompatible function arguments. The following argument types are supported:
    1. (self: NetworkButton, topic: BooleanTopic)
    2. (self: NetworkButton, sub: BooleanSubscriber)
    3. (self: NetworkButton, table: NetworkTable, field: str)
    4. (self: NetworkButton, table: str, field: str)
    5. (self: NetworkButton, inst: NetworkTableInstance, table: str, field: str)

Invoked with: {format_args_kwargs(self, *args, **kwargs)}
"""
        )
