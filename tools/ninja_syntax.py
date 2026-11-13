"""Minimal writer for Ninja build files.

Only the subset of the Ninja syntax used by tools/configure.py is
implemented. Escaping follows the Ninja manual: paths escape ``$``, space and
``:``; values (commands, variables) are written verbatim so that ``$in``,
``$out`` and friends keep working.
"""


def escape_path(path) -> str:
    return str(path).replace("$", "$$").replace(" ", "$ ").replace(":", "$:")


def _as_list(value):
    if value is None:
        return []
    if isinstance(value, (list, tuple)):
        return list(value)
    return [value]


class Writer:
    """Writes a build.ninja file to an open text stream."""

    def __init__(self, output, width: int = 78):
        self.output = output
        self.width = width

    def _line(self, text: str = "") -> None:
        self.output.write(text + "\n")

    def newline(self) -> None:
        self._line()

    def comment(self, text: str) -> None:
        for line in str(text).splitlines() or [""]:
            self._line("# " + line if line else "#")

    def variable(self, key: str, value) -> None:
        self._line(f"{key} = {value}")

    def include(self, path) -> None:
        self._line(f"include {escape_path(path)}")

    def rule(
        self,
        name: str,
        command: str,
        description: str | None = None,
        depfile: str | None = None,
        deps: str | None = None,
        generator: bool = False,
        restat: bool = False,
        pool: str | None = None,
        rspfile: str | None = None,
        rspfile_content: str | None = None,
    ) -> None:
        self._line(f"rule {name}")
        self._line(f"  command = {command}")
        if depfile:
            self._line(f"  depfile = {depfile}")
        if deps:
            self._line(f"  deps = {deps}")
        if description:
            self._line(f"  description = {description}")
        if generator:
            self._line("  generator = 1")
        if restat:
            self._line("  restat = 1")
        if pool:
            self._line(f"  pool = {pool}")
        if rspfile:
            self._line(f"  rspfile = {rspfile}")
        if rspfile_content:
            self._line(f"  rspfile_content = {rspfile_content}")

    def build(
        self,
        rule: str,
        outputs,
        inputs=None,
        implicit=None,
        order_only=None,
        variables: dict | None = None,
        implicit_outputs=None,
        pool: str | None = None,
    ) -> None:
        outputs = _as_list(outputs)
        line = "build " + " ".join(escape_path(o) for o in outputs)
        if implicit_outputs:
            line += " | " + " ".join(escape_path(o) for o in _as_list(implicit_outputs))
        line += f": {rule}"
        if inputs:
            line += " " + " ".join(escape_path(i) for i in _as_list(inputs))
        if implicit:
            line += " | " + " ".join(escape_path(i) for i in _as_list(implicit))
        if order_only:
            line += " || " + " ".join(escape_path(i) for i in _as_list(order_only))
        self._line(line)
        if pool:
            self._line(f"  pool = {pool}")
        for key, value in (variables or {}).items():
            self._line(f"  {key} = {value}")

    def default(self, paths) -> None:
        self._line("default " + " ".join(escape_path(p) for p in _as_list(paths)))
