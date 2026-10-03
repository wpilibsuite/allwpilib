import dataclasses
import typing as T

import pytest

_FAILED_ORDER_FIXTURE = "fail_after_cannot_order"


@dataclasses.dataclass(frozen=True)
class PytestOrderWorkerState:
    extra_fixtures: tuple[str, ...] = ()


class PytestOrderWorkerPlugin:
    def __init__(self, state: PytestOrderWorkerState):
        self._state = state

    @pytest.hookimpl(trylast=True)
    def pytest_configure(self, config: pytest.Config):
        ordering_plugin = config.pluginmanager.get_plugin("orderingplugin")
        if ordering_plugin is not None:
            config.pluginmanager.unregister(ordering_plugin)

    @pytest.hookimpl
    def pytest_collection_modifyitems(self, items: list[pytest.Item]):
        for item in items:
            for fixture in reversed(self._state.extra_fixtures):
                if fixture not in item.fixturenames:
                    item.fixturenames.insert(0, fixture)


class PytestOrderAdapter:
    def __init__(self, config: pytest.Config):
        self._config = config
        self._dependency_ordering = config.getoption(
            "order_dependencies", default=False
        )
        self._marker_prefix = config.getoption("order_marker_prefix", default=None)
        self._group_scope = self._effective_group_scope(config)
        self._scope_level = config.getoption("order_scope_level", default=None) or 0
        if config.getoption("order_scope", default=None) in ("module", "class"):
            self._scope_level = 0

    def validate(self, items: list[pytest.Item]) -> None:
        if self._config.pluginmanager.get_plugin("orderingplugin") is None and any(
            self._requests_ordering(item) for item in items
        ):
            raise pytest.UsageError(
                "pytest-order is required to use order markers with isolated tests"
            )

    def groups(self, items: list[pytest.Item]) -> T.Iterator[list[pytest.Item]]:
        group: list[pytest.Item] = []
        previous_order: object = object()
        previous_dependency: object = object()
        previous_structure: object = object()

        for item in items:
            literal_order = item.get_closest_marker("order")
            order = (
                literal_order
                if literal_order is not None
                else self._prefixed_marker(item)
            )
            dependency = (
                item.get_closest_marker("dependency")
                if self._dependency_ordering or order is not None
                else None
            )
            # pytest-order partitions directory scopes before applying group
            # scopes. Preserve both boundaries in the isolated scheduler.
            directory = (
                "/".join(item.nodeid.split("::", 1)[0].split("/")[: self._scope_level])
                if self._scope_level > 0
                else None
            )
            structure = (directory, self._structural_group(item))

            if group and (
                order is not previous_order
                or dependency is not previous_dependency
                or structure != previous_structure
            ):
                yield group
                group = []

            group.append(item)
            previous_order = order
            previous_dependency = dependency
            previous_structure = structure

        if group:
            yield group

    def worker_state(self, item: pytest.Function) -> PytestOrderWorkerState:
        fixtures = (
            (_FAILED_ORDER_FIXTURE,)
            if _FAILED_ORDER_FIXTURE in item.fixturenames
            else ()
        )
        return PytestOrderWorkerState(fixtures)

    @staticmethod
    def _effective_group_scope(config: pytest.Config) -> str | None:
        ranks = {"class": 1, "module": 2, "session": 3}
        order_scope = config.getoption("order_scope", default=None)
        if order_scope not in ranks:
            order_scope = "session"
        group_scope = config.getoption("order_group_scope", default=None)
        if group_scope not in ranks:
            group_scope = order_scope
        if ranks[group_scope] >= ranks[order_scope]:
            return None
        return group_scope

    def _structural_group(self, item: pytest.Item) -> str | None:
        if self._group_scope == "module":
            return item.nodeid.split("::", 1)[0]
        if self._group_scope == "class":
            class_collector = item.getparent(pytest.Class)
            if class_collector is not None:
                return class_collector.nodeid
            return item.nodeid.split("::", 1)[0]
        return None

    def _requests_ordering(self, item: pytest.Item) -> bool:
        if item.get_closest_marker("order") is not None:
            return True
        if self._prefixed_marker(item) is not None:
            return True
        return self._dependency_ordering and (
            item.get_closest_marker("dependency") is not None
        )

    def _prefixed_marker(self, item: pytest.Item):
        if not self._marker_prefix:
            return None
        for marker in item.iter_markers():
            if marker.name.startswith(self._marker_prefix):
                try:
                    int(marker.name[len(self._marker_prefix)])
                except (IndexError, ValueError):
                    continue
                return marker
        return None
