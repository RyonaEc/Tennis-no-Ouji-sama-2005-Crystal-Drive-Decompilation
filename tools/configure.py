#!/usr/bin/env python3
"""Generates build.ninja for the Crystal Drive decompilation.

Usage:
    python tools/configure.py ATOJ

The graph is driven by dsd (https://github.com/AetiasHax/ds-decomp): dsd
delinks the base ROM into relocatable objects, generates the linker command
file, and rebuilds the ROM from the linked ELF.

Running ninja again after editing sources, symbols or delinks re-runs this
script automatically.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

import ninja_syntax
from get_platform import Platform, get_platform

# ---------------------------------------------------------------------------
# Tool versions
# ---------------------------------------------------------------------------

GAME = "tennis"
DSD_VERSION = "v0.12.1"
WIBO_VERSION = "1.1.0"
SJISWRAP_VERSION = "v1.2.2"
OBJDIFF_VERSION = "v3.8.3"

# CodeWarrior for Nintendo DS. The exact service pack used by the original
# build is not known yet; sweep it once functions can be compared (see
# docs/WORKFLOW.md), then pin the winner here.
DEFAULT_MWCC_VERSION = "1.2/sp2"
# Name of the matching compiler on decomp.me for the nds_arm9 platform.
DECOMP_ME_COMPILER = "mwcc_30_131"

CC_FLAGS = " ".join(
    [
        "-O4,p",  # Optimize maximally for performance
        "-enum int",  # Use int-sized enums
        "-proc arm946e",  # Target processor
        "-gccext,on",  # Enable GCC extensions
        "-fp soft",  # Compute float operations in software
        "-char signed",  # Char type is signed
        "-inline on,noauto",  # Inline only functions marked 'inline'
        "-Cpp_exceptions off",  # Disable C++ exceptions
        "-gccinc",  # Interpret #include "..." and <...> equally
        "-interworking",  # Enable ARM/Thumb interworking
        "-sym on",  # Debug info, including line numbers
        "-nolink",  # Do not link
        "-msgstyle gcc",  # GCC-like diagnostics
        "-str reuse",  # Reuse string literals
        "-enc sjis",  # Shift-JIS source encoding
    ]
)
LD_FLAGS = " ".join(
    [
        "-proc arm946e",
        "-nodead",  # Keep overlays that are only reached at runtime
        "-nostdlib",  # No C/C++ standard library
        "-interworking",
        "-map closure,unused",  # Generate a map file
        "-msgstyle gcc",
    ]
)
MODULE_LD_FLAGS = "-library"
ARM9_LD_FLAGS = "-m Entry"

DSD_OBJDIFF_ARGS = " ".join(
    [
        "--scratch",
        f"--compiler {DECOMP_ME_COMPILER}",
        f'--c-flags "{CC_FLAGS}"',
        "--custom-make ninja",
    ]
)
DSD_BASE_FLAGS = "--force-color"

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

ROOT = Path(__file__).resolve().parent.parent
BUILD_NINJA = ROOT / "build.ninja"
CONFIG_DIR = ROOT / "config"
BUILD_DIR = ROOT / "build"
SRC_DIR = ROOT / "src"
LIBS_DIR = ROOT / "libs"
INCLUDE_DIR = ROOT / "include"
EXTRACT_DIR = ROOT / "extract"
TOOLS_DIR = ROOT / "tools"
from rom_info import find_arm7_bios, secure_area_encrypted


def rel(path: Path) -> str:
    """Path relative to the repository root, with forward slashes."""
    return Path(os.path.relpath(path, ROOT)).as_posix()


def tool(path: str | Path) -> str:
    """Executable path in the form the host can actually launch.

    Windows CreateProcess does not resolve a relative path that uses forward
    slashes, so native separators are required for the tool binaries while the
    rest of the build keeps POSIX-style paths.
    """
    text = rel(path) if Path(path).is_absolute() else str(path)
    return text.replace("/", "\\") if os.name == "nt" else text


def get_config_files(directory: Path, name: str) -> list[str]:
    return sorted(rel(path) for path in directory.rglob(name))


def source_files() -> list[Path]:
    """Every C/C++ translation unit in src/ and libs/, relative to the root."""
    files: list[Path] = []
    for directory in (SRC_DIR, LIBS_DIR):
        for path in sorted(directory.rglob("*")):
            if path.suffix in (".c", ".cpp"):
                files.append(Path(rel(path)))
    return files


def include_dirs() -> list[Path]:
    directories = [INCLUDE_DIR]
    for path in LIBS_DIR.rglob("include"):
        if path.is_dir():
            directories.append(path)
    return directories


# ---------------------------------------------------------------------------
# Project description
# ---------------------------------------------------------------------------


def find_baserom(version: str) -> str:
    """Path of the dump this project was initialised from.

    tools/init.py records the path in config/<version>/rom.txt, so the dump may
    have any file name. The dump is only located when the build graph is
    generated, so `ninja download_tools` still works before a dump is present.
    """
    record = CONFIG_DIR / version / "rom.txt"
    candidates: list[Path] = []
    if record.is_file():
        recorded = Path(record.read_text(encoding="utf-8").strip())
        candidates.append(recorded if recorded.is_absolute() else ROOT / recorded)
    candidates.extend(sorted(p for p in EXTRACT_DIR.glob("*.nds") if p.is_file()))
    candidates.append(EXTRACT_DIR / f"baserom_{GAME}_{version}.nds")
    for path in candidates:
        if path.is_file():
            return rel(path)
    print(
        f"warning: no base ROM found for {version}; put a dump under extract/ "
        "and run python tools/init.py",
        file=sys.stderr,
    )
    return rel(EXTRACT_DIR / f"baserom_{GAME}_{version}.nds")


class Project:
    def __init__(self, version: str, platform: Platform, delinks_json: Any | None):
        self.version = version
        self.platform = platform
        self.delinks_json = delinks_json

        self.game_config = CONFIG_DIR / version
        self.game_build = BUILD_DIR / version
        self.game_extract = EXTRACT_DIR / version

        self.delinks_files = get_config_files(self.game_config, "delinks.txt")
        self.relocs_files = get_config_files(self.game_config, "relocs.txt")
        self.symbols_files = get_config_files(self.game_config, "symbols.txt")

    # --- dsd inputs ------------------------------------------------------

    def dsd_configs(self) -> list[str]:
        return self.delinks_files + self.relocs_files + self.symbols_files

    def arm9_config_yaml(self) -> str:
        return rel(self.game_config / "arm9" / "config.yaml")

    def baserom(self) -> str:
        return find_baserom(self.version)

    def baserom_config(self) -> str:
        return rel(self.game_extract / "config.yaml")

    # --- build outputs ---------------------------------------------------

    def build_rom(self) -> str:
        return f"{GAME}_{self.version}.nds"

    def build_rom_config(self) -> str:
        return rel(self.game_build / "build" / "rom_config.yaml")

    def arm9_o(self) -> str:
        return rel(self.game_build / "arm9.o")

    def sha1_file(self) -> str:
        return rel(self.game_config / f"{GAME}_{self.version}.sha1")

    def objdiff_report(self) -> str:
        return rel(self.game_build / "report.json")

    def source_objects(self) -> list[str]:
        return [rel(self.game_build / path.with_suffix(".o")) for path in source_files()]

    # --- delinks JSON ----------------------------------------------------

    def files(self) -> list[dict[str, str]]:
        if not self.delinks_json:
            return []
        return self.delinks_json["files"]

    def delink_files(self) -> list[str]:
        return sorted({entry["delink_file"] for entry in self.files()})

    def arm9_lcf_file(self) -> str:
        return self.delinks_json["arm9_lcf_file"] if self.delinks_json else ""

    def arm9_objects_file(self) -> str:
        return self.delinks_json["arm9_objects_file"] if self.delinks_json else ""


# ---------------------------------------------------------------------------


def check_can_run_dsd(dsd: str) -> bool:
    if not Path(dsd).is_file() and shutil.which(dsd) is None:
        return False
    try:
        result = subprocess.run(
            [dsd, "--version"], capture_output=True, text=True, timeout=30, cwd=ROOT
        )
    except (OSError, subprocess.SubprocessError):
        return False
    return result.returncode == 0


def dsd_version(dsd: str) -> str:
    try:
        result = subprocess.run(
            [dsd, "--version"], capture_output=True, text=True, timeout=30, cwd=ROOT
        )
    except (OSError, subprocess.SubprocessError):
        return "unknown"
    version = result.stdout.strip().split()[-1] if result.stdout.strip() else "unknown"
    return version if version.startswith("v") or version == "unknown" else "v" + version


def add_download_tool_builds(n: ninja_syntax.Writer, args, platform: Platform) -> None:
    downloads: list[str] = []

    def want(tool: str, outputs, tag: str, path: str) -> None:
        """Declare a download edge only when a tool is actually missing.

        Ninja considers an existing output dirty when no build log entry
        records how it was produced, so pre-installed tools must not get an
        edge at all.
        """
        outputs_list = outputs if isinstance(outputs, list) else [outputs]
        downloads.extend(outputs_list)
        if all((ROOT / Path(p.replace("\\", "/"))).is_file() for p in outputs_list):
            return
        n.build(
            rule="download_tool",
            outputs=outputs,
            variables={"tool": tool, "tag": tag, "path": path},
        )
        n.newline()

    if args.dsd is None:
        want("dsd", args.dsd_path, DSD_VERSION, args.dsd_path)

    want("objdiff", args.objdiff, OBJDIFF_VERSION, args.objdiff)

    if args.compiler is None:
        want("mwccarm", [args.mwcc, args.mwld], "latest", rel(TOOLS_DIR))

    want("sjiswrap", args.sjiswrap, SJISWRAP_VERSION, args.sjiswrap)

    if platform.system != "windows" and args.wine == "./wibo":
        want("wibo", args.wibo, WIBO_VERSION, args.wibo)

    if downloads:
        n.build(inputs=downloads, rule="phony", outputs="download_tools")
        n.newline()


def add_extract_build(n: ninja_syntax.Writer, project: Project, dsd: str) -> None:
    if args.no_extract:
        return
    n.build(
        inputs=project.baserom(),
        implicit=dsd,
        rule="extract",
        outputs=project.baserom_config(),
        variables={"output_path": rel(project.game_extract)},
    )
    n.newline()


def add_delink_and_lcf_builds(n: ninja_syntax.Writer, project: Project, dsd: str) -> None:
    delink_files = project.delink_files()
    if delink_files:
        n.comment("Delink the base ROM when any dsd configuration changes")
        n.build(
            inputs=project.dsd_configs() + [project.baserom_config()],
            implicit=dsd,
            rule="delink",
            outputs=delink_files,
            variables={"config_path": project.arm9_config_yaml()},
        )
        n.newline()
        n.build(inputs=delink_files, rule="phony", outputs="delink")
        n.newline()

    n.build(
        inputs=project.delinks_files + [project.baserom_config()],
        implicit=dsd,
        rule="lcf",
        outputs=[project.arm9_lcf_file(), project.arm9_objects_file()],
        variables={"config_path": project.arm9_config_yaml()},
    )
    n.newline()


def add_disassemble_build(n: ninja_syntax.Writer, project: Project, dsd: str) -> None:
    n.build(
        inputs=project.dsd_configs(),
        implicit=dsd,
        rule="disassemble",
        outputs="dis",
        variables={
            "config_path": project.arm9_config_yaml(),
            "output_path": rel(project.game_build / "asm"),
        },
    )
    n.newline()


def mwcc_version_overrides() -> dict[str, str]:
    """Per-file compiler versions.

    Retail was not produced by a single mwccarm: a number of translation units
    only reproduce with an earlier 1.2 build. A source file may therefore name
    its own compiler in config/ATOJ/mwcc_versions.txt; everything else uses
    --mwcc-version (1.2/sp2).
    """
    path = ROOT / "config" / "ATOJ" / "mwcc_versions.txt"
    overrides: dict[str, str] = {}
    if not path.is_file():
        return overrides
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if not line or "=" not in line:
            continue
        source, version = (part.strip() for part in line.split("=", 1))
        overrides[source.replace("\\", "/")] = version
    return overrides


def add_mwcc_builds(n: ninja_syntax.Writer, project: Project, args, platform: Platform) -> None:
    includes = " ".join(f"-i {rel(path)}" for path in include_dirs())
    mwcc_cmd = (
        f'{args.wine} "{args.sjiswrap}" "$mwcc_exe" {CC_FLAGS} {includes} '
        "$cc_flags -d $game_version -MD -c $in -o $basedir"
    )
    overrides = mwcc_version_overrides()
    compiler_root = Path(args.compiler) if args.compiler else TOOLS_DIR / "mwccarm"
    mwcc_implicit = [args.mwcc, args.sjiswrap]
    if platform.system != "windows":
        transform_dep = rel(TOOLS_DIR / "transform_dep.py")
        mwcc_cmd += f" && $python {transform_dep} $basefile.d"
        mwcc_implicit.append(transform_dep)
        if args.wine == "./wibo":
            mwcc_implicit.append(args.wibo)

    n.rule(name="mwcc", command=mwcc_cmd, depfile="$basefile.d")
    n.newline()

    for source in source_files():
        object_path = project.game_build / source.with_suffix(".o")
        lang = "-lang=c++" if source.suffix == ".cpp" else "-lang=c"
        base = Path(rel(object_path)).with_suffix("")
        version = overrides.get(rel(ROOT / source), args.mwcc_version)
        if version == args.mwcc_version:
            mwcc_exe = args.mwcc
        else:
            exe = compiler_root / version / "mwccarm.exe"
            if not exe.is_file():
                raise SystemExit(f"{rel(ROOT / source)}: unknown mwccarm version {version!r}")
            mwcc_exe = tool(rel(exe))
        implicit = mwcc_implicit if mwcc_exe in mwcc_implicit else [*mwcc_implicit, mwcc_exe]
        n.build(
            inputs=rel(ROOT / source),
            implicit=implicit,
            rule="mwcc",
            outputs=rel(object_path),
            variables={
                "game_version": project.version,
                "cc_flags": lang,
                "basedir": base.parent.as_posix(),
                "basefile": base.as_posix(),
                "mwcc_exe": mwcc_exe,
            },
        )
        n.newline()


def add_mwld_and_rom_builds(n: ninja_syntax.Writer, project: Project, args) -> None:
    objects = [entry["object_to_link"] for entry in project.files()]
    elf = project.arm9_o()

    if objects:
        n.comment("Link the ARM9 program")
        n.build(
            inputs=[*objects, project.arm9_lcf_file(), project.arm9_objects_file()],
            implicit=args.mwld,
            rule="mwld",
            outputs=elf,
            variables={
                "extra_ld_flags": ARM9_LD_FLAGS,
                "lcf_file": project.arm9_lcf_file(),
                "objects_file": project.arm9_objects_file(),
            },
        )
        n.newline()

    n.build(inputs=elf, rule="phony", outputs="arm9")
    n.newline()

    rom_config = project.build_rom_config()
    n.build(
        inputs=elf,
        implicit=args.dsd_path,
        rule="rom_config",
        outputs=rom_config,
        variables={"config_path": project.arm9_config_yaml()},
    )
    n.newline()

    n.build(
        inputs=rom_config,
        implicit=args.dsd_path,
        rule="rom_build",
        outputs=project.build_rom(),
    )
    n.newline()

    n.build(inputs=project.build_rom(), rule="phony", outputs="rom")
    n.newline()

    n.build(
        inputs=project.build_rom(),
        rule="sha1",
        outputs="sha1",
        variables={"sha1_file": project.sha1_file()},
    )
    n.newline()


def add_check_builds(n: ninja_syntax.Writer, project: Project, dsd: str) -> None:
    n.build(
        outputs="check_modules",
        inputs=project.arm9_o(),
        implicit=dsd,
        rule="check_modules",
        variables={"config_path": project.arm9_config_yaml()},
    )
    n.newline()

    n.build(
        outputs="check_symbols",
        inputs=project.arm9_o(),
        implicit=dsd,
        rule="check_symbols",
        variables={
            "config_path": project.arm9_config_yaml(),
            "elf_path": project.arm9_o(),
        },
    )
    n.newline()

    n.build(inputs=["check_modules", "check_symbols"], rule="phony", outputs="check")
    n.newline()


def add_objdiff_builds(n: ninja_syntax.Writer, project: Project, args) -> None:
    n.build(
        outputs="objdiff.json",
        inputs=project.dsd_configs(),
        implicit=args.dsd_path,
        rule="objdiff",
        variables={"config_path": project.arm9_config_yaml()},
    )
    n.newline()

    n.build(inputs="objdiff.json", rule="phony", outputs="objdiff")
    n.newline()

    n.build(
        outputs=project.objdiff_report(),
        inputs="objdiff.json",
        implicit=[args.objdiff] + project.delink_files() + project.source_objects(),
        rule="objdiff_report",
    )
    n.newline()

    n.build(inputs=project.objdiff_report(), rule="phony", outputs="report")
    n.newline()


def add_apply_build(n: ninja_syntax.Writer, project: Project, dsd: str) -> None:
    n.build(
        outputs="apply",
        inputs=project.arm9_o(),
        implicit=dsd,
        rule="apply",
        variables={
            "config_path": project.arm9_config_yaml(),
            "elf_path": project.arm9_o(),
        },
    )
    n.newline()


def add_configure_build(n: ninja_syntax.Writer, args) -> None:
    command = subprocess.list2cmdline([sys.executable, rel(Path(__file__)), *sys.argv[1:]])
    n.rule(name="configure", command=command, generator=True)
    n.newline()

    implicit = [rel(Path(__file__))]
    if args.dsd is None:
        implicit.append(args.dsd_path)
    n.build(outputs="build.ninja", rule="configure", implicit=implicit)
    n.newline()


def write_rules(n: ninja_syntax.Writer, args) -> None:
    n.variable("python", sys.executable)
    baserom = Path(find_baserom(args.version))
    bios = find_arm7_bios(ROOT)
    # The BIOS is only needed when the dump is encrypted: it decrypts the
    # secure area on extract and re-encrypts it on build. Passing it for a
    # decrypted dump would encrypt the rebuilt ROM and break the digest.
    if baserom.is_file() and bios is not None and secure_area_encrypted(baserom):
        n.variable("arm7_bios_flag", f"--arm7-bios {rel(bios)}")
    else:
        n.variable("arm7_bios_flag", "")
    n.newline()

    n.rule(
        name="download_tool",
        command=f'{sys.executable} {rel(TOOLS_DIR / "download_tool.py")} $tool $tag --path $path',
        description="Download $tool",
    )
    n.newline()

    n.rule(
        name="extract",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} rom extract --rom $in --output-path $output_path $arm7_bios_flag",
        description="Extract the base ROM",
    )
    n.newline()

    n.rule(
        name="delink",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} delink --config-path $config_path",
        description="Delink the base ROM",
    )
    n.newline()

    n.rule(
        name="disassemble",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} dis --config-path $config_path --asm-path $output_path --ual",
        description="Disassemble the base ROM",
    )
    n.newline()

    n.rule(
        name="lcf",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} lcf --config-path $config_path",
        description="Generate the linker script",
    )
    n.newline()

    n.rule(
        name="mwld",
        command=f'{args.wine} "{args.mwld}" {LD_FLAGS} $extra_ld_flags @$objects_file $lcf_file -o $out',
        description="Link $out",
    )
    n.newline()

    n.rule(
        name="rom_config",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} rom config --elf $in --config $config_path",
        description="Generate the ROM configuration",
    )
    n.newline()

    n.rule(
        name="rom_build",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} rom build --config $in --rom $out $arm7_bios_flag",
        description="Build $out",
    )
    n.newline()

    n.rule(
        name="objdiff",
        command=(
            f'$python {rel(TOOLS_DIR / "objdiff_gen.py")} "{args.dsd_path}" '
            f"$config_path {DSD_OBJDIFF_ARGS}"
        ),
        description="Generate objdiff.json",
    )
    n.newline()

    n.rule(
        name="objdiff_report",
        command=f"{args.objdiff} report generate -o $out",
        description="Generate the objdiff report",
    )
    n.newline()

    n.rule(
        name="check_modules",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} check modules --config-path $config_path --fail",
        description="Check modules against the base ROM",
    )
    n.newline()

    n.rule(
        name="check_symbols",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} check symbols --config-path $config_path "
        "--elf-path $elf_path --fail --max-lines 20",
        description="Check symbols against the base ROM",
    )
    n.newline()

    n.rule(
        name="apply",
        command=f"{args.dsd_path} {DSD_BASE_FLAGS} apply --config-path $config_path --elf-path $elf_path",
        description="Apply symbol data from the linked ELF",
    )
    n.newline()

    n.rule(
        name="sha1",
        command=f"{sys.executable} {rel(TOOLS_DIR / 'sha1.py')} $in -c $sha1_file",
        description="Verify the built ROM",
    )
    n.newline()


def parse_args():
    parser = argparse.ArgumentParser(description="Generates build.ninja")
    parser.add_argument("version", help="Game version directory under config/ (ATOJ)")
    parser.add_argument(
        "-w",
        "--wine",
        default="./wibo",
        help="Path to Wine/Wibo, used on Linux (default: ./wibo)",
    )
    parser.add_argument("--compiler", type=Path, help="Pre-installed compiler root directory")
    parser.add_argument("--mwcc-version", default=DEFAULT_MWCC_VERSION, help="Compiler service pack")
    parser.add_argument("--no-extract", action="store_true", help="Skip the ROM extract step")
    parser.add_argument("--dsd", type=Path, help="Pre-installed dsd CLI")
    return parser.parse_args()


args: Any = None


def main() -> int:
    global args
    args = parse_args()

    platform = get_platform()
    if platform is None:
        return 1

    args.platform = platform
    args.dsd_path = tool(rel(args.dsd) if args.dsd else f"./dsd{platform.exe}")
    args.objdiff = tool(f"./objdiff-cli{platform.exe}")
    args.sjiswrap = tool(rel(TOOLS_DIR / "sjiswrap.exe"))
    compiler_root = Path(args.compiler) if args.compiler else TOOLS_DIR / "mwccarm"
    args.mwcc = tool(rel(compiler_root / args.mwcc_version / "mwccarm.exe"))
    args.mwld = tool(rel(compiler_root / args.mwcc_version / "mwldarm.exe"))
    args.wibo = tool(f"wibo{platform.exe}")
    if platform.system == "windows":
        args.wine = ""

    can_run_dsd = check_can_run_dsd(args.dsd_path)
    if can_run_dsd and dsd_version(args.dsd_path) != DSD_VERSION:
        print(
            f"warning: dsd {dsd_version(args.dsd_path)} is installed, "
            f"this project pins {DSD_VERSION}"
        )

    config_ready = (CONFIG_DIR / args.version / "arm9" / "config.yaml").is_file()

    delinks_json = None
    if can_run_dsd and config_ready:
        result = subprocess.run(
            [
                args.dsd_path,
                DSD_BASE_FLAGS,
                "json",
                "delinks",
                "--config-path",
                rel(CONFIG_DIR / args.version / "arm9" / "config.yaml"),
            ],
            capture_output=True,
            text=True,
            cwd=ROOT,
        )
        if result.returncode != 0:
            print("error: dsd failed to read the project configuration:", file=sys.stderr)
            print(result.stderr.strip(), file=sys.stderr)
            return 1
        delinks_json = json.loads(result.stdout)

    project = Project(args.version, platform, delinks_json)
    # mwld writes each MEMORY output relative to the -o directory, and dsd's
    # linker script names them "build/<module>.bin"; the directory must exist.
    (project.game_build / "build").mkdir(parents=True, exist_ok=True)

    with BUILD_NINJA.open("w", encoding="utf-8") as file:
        n = ninja_syntax.Writer(file)

        write_rules(n, args)
        add_download_tool_builds(n, args, platform)
        add_configure_build(n, args)

        if not can_run_dsd:
            n.default(["download_tools"])
            print(
                "dsd was not found: build.ninja currently only downloads tools.\n"
                "Run `ninja` to fetch them, then re-run this script."
            )
            return 0

        if not config_ready:
            n.default(["download_tools"])
            print(
                f"error: {rel(CONFIG_DIR / args.version / 'arm9' / 'config.yaml')} is missing.\n"
                "Run `python tools/init.py` to extract and initialise the base ROM,\n"
                "then run this script again.",
                file=sys.stderr,
            )
            return 1

        add_extract_build(n, project, args.dsd_path)
        add_delink_and_lcf_builds(n, project, args.dsd_path)
        add_disassemble_build(n, project, args.dsd_path)
        add_mwcc_builds(n, project, args, platform)
        add_mwld_and_rom_builds(n, project, args)
        add_check_builds(n, project, args.dsd_path)
        add_objdiff_builds(n, project, args)
        add_apply_build(n, project, args.dsd_path)

        n.default(["rom", "objdiff"])

    print(f"Wrote {rel(BUILD_NINJA)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
