"""Host platform information for downloading prebuilt tools."""

import platform


class Platform:
    def __init__(self, *, system: str, machine: str, exe: str):
        self.system = system
        """Operating system name: "windows" or "linux"."""
        self.machine = machine
        """Machine architecture: "x86_64"."""
        self.exe = exe
        """Executable extension: ".exe" on Windows, "" elsewhere."""


def get_platform() -> Platform | None:
    system = platform.system()
    exe = ""
    if system == "Windows" or system.startswith("MINGW") or system.startswith("MSYS"):
        system = "windows"
        exe = ".exe"
    elif system == "Linux":
        system = "linux"
    else:
        print(f"Unsupported host system: {system}")
        print("See docs/SETUP.md for how to build the toolchain from source.")
        return None

    match platform.machine().lower():
        case "amd64" | "x86_64":
            machine = "x86_64"
        case other:
            print(f"Unsupported host machine: {other}")
            return None

    return Platform(system=system, machine=machine, exe=exe)
