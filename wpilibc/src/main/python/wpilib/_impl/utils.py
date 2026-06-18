# novalidate

import sys
import inspect


def match_arglist(name, args, kwargs, templates, allow_extra_kwargs=False):
    """
    This compares args and kwargs against the argument templates in templates.

    :param name: Name of the function being checked
    :param args: The list of positional arguments
    :param kwargs: The list of keyword arguments
    :param templates: A list of dictionaries corresponding to possible
    argument list formats.
    :param allow_extra_kwargs: Whether or not to allow extra keyword arguments. If this
        is true, then extra keyword arguments will be added to the result dictionary.

    An argument template is structured as follows:

    Each element in the list should be a tuple, corresponding to an argument name,
    and argument type condition. See types_match() for argument type condition structures.

    :returns The id of the selected template
    :returns A dictionary of argument name, value tuples.
    :
    """
    return __match_arglist(name, args, kwargs, templates, False, allow_extra_kwargs)


def _print(*args, **kwargs):
    print(*args, **kwargs)


def __match_arglist(name, args, kwargs, templates, err, allow_extra_kwargs=False):
    # TODO: we can do better at giving the user an error message...

    if err:
        _print("*" * 50)
        _print("ERROR: Invalid arguments passed to %s()!!" % name)
        _print("       checking args against %s possible templates" % len(templates))
        _print("*" * 50)
        if len(args):
            _print("Your non-keyword arguments: ")
            for i, arg in enumerate(args):
                _print("  #%d: value %s, type %s" % (i, arg, type(arg)))
        if len(kwargs):
            _print("Your keyword args: ")
            for k, v in kwargs.items():
                _print("  %s: value %s, type %s" % (k, v, type(v)))
        _print("*" * 50)

    # Try each template until we find one that works
    for i, template in enumerate(templates):
        # List copies of the arguments
        args_copy = list(reversed(args))
        kwargs_copy = list(kwargs.copy())
        kwarg_found = False
        results = dict()

        if err:
            _print(
                "Checking template %s: %s" % (i, ", ".join(an for an, _ in template))
            )
            showed_error = False

        # Scan through all arguments and set valid to false if we find an issue.
        for j, (arg_name, arg_type_condition) in enumerate(template):
            # Check kwargs first, then check args
            if arg_name in kwargs_copy:
                kwargs_copy.remove(arg_name)
                value = kwargs[arg_name]
                match_type = "keyword"
                kwarg_found = True
            elif not kwarg_found and len(args_copy) > 0:
                value = args_copy.pop()
                match_type = "non-keyword"
            else:
                value = None
                match_type = "optional"

            results[arg_name] = value

            # Check to see if identities match:
            if not types_match(value, arg_type_condition):
                if err:
                    _print(
                        "- Error at arg %d: %s != %s"
                        % (j, arg_name, typematch_to_str(arg_type_condition))
                    )
                    _print(
                        "     your arg: %s; value %s %s"
                        % (match_type, value, type(value))
                    )
                    _print()
                    showed_error = True
                break

        else:
            # If the results are valid and the argument lists are empty, return the results.
            if len(args_copy) == 0 and (len(kwargs_copy) == 0 or allow_extra_kwargs):
                output = kwargs.copy()
                output.update(results)
                return templates.index(template), output

        if err and not showed_error:
            if len(args_copy) != 0:
                _print("- Error: too many arguments")
            elif len(kwargs_copy):
                _print(
                    "- Error: unused parameters: %s"
                    % ", ".join("%s" % s for s in kwargs_copy)
                )

            _print()
        #    _print("Template %s unmatched:" % i)
        #    if len(args_copy):
        #        _print("- Args: %s" % (' '.join('%s' % s for s in args_copy)))
        #    if len(kwargs_copy):
        #        _print("- Kwargs: %s" % (' '.join('%s' % s for s in kwargs_copy)))

    # We found nothing, then... but we need to give the user a good
    # error message, so run it again in verbose mode, then raise the error!
    if not err:
        __match_arglist(name, args, kwargs, templates, True)
    else:
        _print("*" * 50)
        raise ValueError(
            "Attribute error, attributes given did not match any argument templates. See messages above for more info"
        )


def types_match(object, type_structure):
    """
    :param object: the object to check
    :param type_structure: The structure, composed of lists, strings, and types,
    representing conditions that object must meet.

    Here are the possibilities for type_structure:
    - HasAttribute
    - type_condition
    - list of type_structures
    - None

    If type_structure is an HasAttribute, this will return False if
    any contained attribute is not present in object

    If type_structure is a type_condition, then object must be of the same type

    If type_structure is a list of type_structures, it is handled by running
    types_match on each item, and returning true if at least one matches.

    If type_structure is None, than everything matches it, and true is returned.
    """

    if type_structure is None:
        return True

    # Is it an attribute list?
    elif hasattr(type_structure, "matches"):
        return type_structure.matches(object)

    elif isinstance(type_structure, list) and len(type_structure) != 0:
        # If the list is not an attribute condition, check each element
        # and return true if a match is found
        for type_condition in type_structure:
            if types_match(object, type_condition):
                return True
    else:
        return isinstance(object, type_structure)


def typematch_to_str(type_structure):
    """Only used for debugging"""

    if type_structure is None:
        return "always matches"

    elif isinstance(type_structure, HasAttribute):
        return "hasattr(%s)" % " or ".join(type_structure.conditions)

    elif isinstance(type_structure, list):
        return " or ".join(typematch_to_str(tc) for tc in type_structure)

    else:
        return "%s" % type_structure


class HasAttribute:
    def __init__(self, *args):
        self.conditions = args[:]

    def matches(self, object):
        for attribute in self.conditions:
            if not hasattr(object, attribute):
                return False

        return True


def reset_wpilib():
    """
    Clears all devices from WPILib, and resets the hal data

    .. warning:: This is only intended to be used by test frameworks and
                 other debugging tools with the simulated HAL!
    """

    raise NotImplementedError("Not supported yet for 2020")

    modules = [
        "wpilib",
        "wpilib.buttons",
        "wpilib.command",
        "wpilib.interfaces",
        "wpilib.shuffleboard",
    ]

    for modname in modules:
        try:
            module = sys.modules[modname]
        except KeyError:
            continue

        for _, cls in inspect.getmembers(module, inspect.isclass):
            if hasattr(cls, "_reset"):
                cls._reset()

    import hal_impl.functions

    hal_impl.functions.reset_hal()
