from traceback import extract_stack, format_list
from ._wpiutil import getStackTraceDefault, _setup_stack_trace_hook
from os.path import join

_start_py = join("wpilib", "_impl", "start.py")


def _stack_trace_hook(offset: int) -> str:
    # note: this implementation ignores offset because it's not
    # actually meaningful when crossing the python/C++ boundary

    stack = extract_stack()[:-1]
    if not stack:
        return "\tat <no python frames>\n" + getStackTraceDefault(offset)

    # filter out any frames before start.py (except for one of them) to
    # make stack frames more useful for users
    for i in range(len(stack) - 1, 0, -1):
        if stack[i].filename.endswith(_start_py):
            stack = stack[i:]
            break

    trace = format_list(stack)
    return "\n".join(trace)


_setup_stack_trace_hook(_stack_trace_hook)
