#!/uer/bin/python3

from subprocess import run
from sys import exit

cmd_compile = [
    "clang",
    "-std=c18",
    "-target",
    "wasm32",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wimplicit-int-float-conversion",
    "-Wfloat-conversion",
    "-Wfloat-overflow-conversion",
    "-Wimplicit-int-conversion",
    "-Wused-but-marked-unused",
    "-Werror",
    "-O3",
    "-ffast-math",
    "-funsafe-math-optimizations",
    "-fno-builtin",
    "-nostdlib",
    "-nostdinc",
	"-Wl,--no-entry",
    # "-Wl,--allow-undefined",
    "-Wl,--strip-all",
    "-Wl,--export-all",
    "src/root.c",
    "-o",
    "www/mod.wasm",
]

code = run(cmd_compile).returncode
if code != 0:
    print("compiling failed with code", code)
    exit(code)
