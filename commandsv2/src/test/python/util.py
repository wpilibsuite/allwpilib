from typing import Any, Dict, TypeVar, Type

import inspect

import commands2
from wpilib.simulation import DriverStationSim, pauseTiming, resumeTiming, stepTiming


Y = TypeVar("Y")


def full_subclass_of(cls: Type[Y]) -> Type[Y]:
    # Pybind classes can't be monkeypatched.
    # This generates a subclass with every method filled out
    # so that it can be monkeypatched.
    retlist = []
    clsname = cls.__name__  # + "_Subclass"
    classdef = f"class {clsname}(cls):\n"
    for name in dir(cls):
        # for name in set(dir(cls)):
        value = getattr(cls, name)
        if callable(value) and not inspect.isclass(value) and not name.startswith("_"):
            classdef += f"  def {name}(self, *args, **kwargs):\n"
            classdef += f"    return super().{name}(*args, **kwargs)\n"
    classdef += "  ...\n"
    classdef += f"retlist.append({clsname})\n"
    print(classdef)
    exec(classdef, globals(), locals())
    return retlist[0]


class ManualSimTime:
    def __enter__(self) -> "ManualSimTime":
        pauseTiming()
        return self

    def __exit__(self, *args):
        resumeTiming()

    def step(self, delta: float):
        stepTiming(delta)


class OOInteger:
    def __init__(self, value: int = 0) -> None:
        self.value = value

    def get(self) -> int:
        return self.value

    def set(self, value: int):
        self.value = value

    def incrementAndGet(self) -> int:
        self.value += 1
        return self.value

    def addAndGet(self, value: int) -> int:
        self.value += value
        return self.value

    def __eq__(self, value: float) -> bool:
        return self.value == value

    def __lt__(self, value: float) -> bool:
        return self.value < value

    def __call__(self) -> int:
        return self.value


class OOBoolean:
    def __init__(self, value: bool = False) -> None:
        self.value = value

    def get(self) -> bool:
        return self.value

    def set(self, value: bool):
        self.value = value

    def __eq__(self, value: object) -> bool:
        return self.value == value

    def __bool__(self) -> bool:
        return self.value

    def __call__(self) -> bool:
        return self.value


class InternalButton(commands2.button.Trigger):
    def __init__(self):
        super().__init__(self.isPressed)
        self.pressed = False

    def isPressed(self) -> bool:
        return self.pressed

    def setPressed(self, value: bool):
        self.pressed = value

    def __call__(self) -> bool:
        return self.pressed


class OOFloat:
    def __init__(self, value: float = 0.0) -> None:
        self.value = value

    def get(self) -> float:
        return self.value

    def set(self, value: float):
        self.value = value

    def incrementAndGet(self) -> float:
        self.value += 1
        return self.value

    def addAndGet(self, value: float) -> float:
        self.value += value
        return self.value

    def __eq__(self, value: float) -> bool:
        return self.value == value

    def __lt__(self, value: float) -> bool:
        return self.value < value

    def __call__(self) -> float:
        return self.value

    def __name__(self) -> str:
        return "OOFloat"


##########################################
# Fakito Framework


def _get_all_args_as_kwargs(method, *args, **kwargs) -> Dict[str, Any]:
    try:
        import inspect

        method_args = inspect.getcallargs(method, *args, **kwargs)

        method_arg_names = list(inspect.signature(method).parameters.keys())

        for idx, arg in enumerate(args):
            method_args[method_arg_names[idx]] = arg

        try:
            del method_args["self"]
        except KeyError:
            pass
        return method_args
    except TypeError:
        # Pybind methods can't be inspected
        # The exact args/kwargs that are passed in are checked instead
        r = {}
        for idx, arg in enumerate(args):
            r[idx] = arg
        r.update(kwargs)
        return r


class MethodWrapper:
    def __init__(self, method):
        self.method = method
        self.og_method = method
        self.times_called = 0
        self.call_log = []

    def __call__(self, *args, **kwargs):
        self.times_called += 1
        method_args = _get_all_args_as_kwargs(self.method, *args, **kwargs)
        self.call_log.append(method_args)
        return self.method(*args, **kwargs)

    def called_with(self, *args, **kwargs):
        return _get_all_args_as_kwargs(self.method, *args, **kwargs) in self.call_log

    def times_called_with(self, *args, **kwargs):
        return self.call_log.count(
            _get_all_args_as_kwargs(self.method, *args, **kwargs)
        )


def start_spying_on(obj: Any) -> None:
    """
    Mocks all methods on an object, so that that call info can be used in asserts.

    Example:
    ```
    obj = SomeClass()
    start_spying_on(obj)
    obj.method()
    obj.method = lambda: None # supports monkeypatching
    assert obj.method.times_called == 2
    assert obj.method.called_with(arg1=1, arg2=2)
    assert obj.method.times_called_with(arg1=1, arg2=2) == 2
    ```
    """

    for name in dir(obj):
        value = getattr(obj, name)
        if callable(value) and not inspect.isclass(value) and not name.startswith("_"):
            setattr(obj, name, MethodWrapper(value))

    if not hasattr(obj.__class__, "_is_being_spied_on"):
        try:
            old_setattr = obj.__class__.__setattr__
        except AttributeError:
            old_setattr = object.__setattr__

        def _setattr(self, name, value):
            if name in dir(self):
                existing_value = getattr(self, name)
                if isinstance(existing_value, MethodWrapper):
                    existing_value.method = value
                    return
            old_setattr(self, name, value)

        obj.__class__.__setattr__ = _setattr
        obj.__class__._is_being_spied_on = True


# fakito verify
def reset(obj: Any) -> None:
    """
    Resets the call log of all mocked methods on an object.
    Also restores all monkeypatched methods.
    """
    for name in dir(obj):
        value = getattr(obj, name)
        if isinstance(value, MethodWrapper):
            value.method = value.og_method
            value.times_called = 0
            value.call_log = []


class times:
    def __init__(self, times: int) -> None:
        self.times = times


def never() -> times:
    return times(0)


class _verify:
    def __init__(self, obj: Any, times: times = times(1)):
        self.obj = obj
        self.times = times.times

    def __getattribute__(self, name: str) -> Any:
        def self_dot(name: str):
            return super(_verify, self).__getattribute__(name)

        def times_string(times: int) -> str:
            if times == 1:
                return "1 time"
            else:
                return f"{times} times"

        def check(*args, **kwargs):
            __tracebackhide__ = True
            # import code
            # code.interact(local={**globals(), **locals()})
            method = getattr(self_dot("obj"), name)
            # method = getattr(self1.obj, name)
            assert method.times_called_with(*args, **kwargs) == self_dot(
                "times"
            ), f"Expected {name} to be called {times_string(self_dot('times'))} with {args} {kwargs}, but was called {times_string(method.times_called_with(*args, **kwargs))}"

        return check


T = TypeVar("T")


def verify(obj: T, times: times = times(1)) -> T:
    # import code
    # code.interact(local={**globals(), **locals()})
    return _verify(obj, times)  # type: ignore


def patch_via_decorator(obj: Any):
    def decorator(method):
        setattr(obj, method.__name__, method.__get__(obj, obj.__class__))
        return method

    return decorator
