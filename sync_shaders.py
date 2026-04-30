#!/usr/bin/env python3
"""
bak3d Shader Sync Script (Python)

Syncs shader source files into the build directory and optionally triggers
a hot-reload signal. Can be invoked manually or called at runtime from C++
via std::system() / std::async().

Usage:
    python sync_shaders.py
    python sync_shaders.py --build-dir path/to/build
    python sync_shaders.py --silent          (no prompts, just sync and exit)

Requirements:
    - Python 3.6+
    - CMake 3.20+ (used to invoke the sync_shaders.cmake script)
"""

import os
import sys
import subprocess
import platform
import argparse
from pathlib import Path


class Colors:
    """ANSI color codes — same as setup.py"""
    RESET  = "\033[0m"
    RED    = "\033[91m"
    GREEN  = "\033[92m"
    YELLOW = "\033[93m"
    CYAN   = "\033[96m"
    BOLD   = "\033[1m"

    @classmethod
    def disable_on_windows(cls):
        if platform.system() == "Windows":
            try:
                import ctypes
                ctypes.windll.kernel32.SetConsoleMode(
                    ctypes.windll.kernel32.GetStdHandle(-11), 7
                )
            except Exception:
                cls.RESET = cls.RED = cls.GREEN = cls.YELLOW = cls.CYAN = cls.BOLD = ""

SHADER_EXTENSIONS = {".vert", ".frag", ".glsl", ".geom", ".comp", ".tesc", ".tese"}

class ShaderSync:

    def __init__(self, silent: bool = False, build_dir: str = None):
        self.silent     = silent
        self.repo_root  = Path(__file__).resolve().parent
        self.build_dir  = Path(build_dir) if build_dir else self.repo_root / "build"
        self.src_shaders  = self.repo_root / "shaders"
        self.dest_shaders = self.build_dir / "shaders"
        self.sync_script  = self.repo_root / "sync_shaders.cmake"
        Colors.disable_on_windows()

    # ------------------------------------------------------------------ output

    def _ok(self, msg):
        print(f"{Colors.GREEN}[OK]{Colors.RESET} {msg}")

    def _err(self, msg):
        print(f"{Colors.RED}[ERROR]{Colors.RESET} {msg}", file=sys.stderr)

    def _info(self, msg):
        if not self.silent:
            print(f"{Colors.CYAN}[INFO]{Colors.RESET} {msg}")

    def _warn(self, msg):
        print(f"{Colors.YELLOW}[WARNING]{Colors.RESET} {msg}")

    def _header(self):
        if self.silent:
            return
        print(f"\n{Colors.CYAN}{Colors.BOLD}")
        print("=" * 60)
        print(" bak3d Shader Sync")
        print("=" * 60)
        print(Colors.RESET)

    # ------------------------------------------------------------ validation

    def _check_cmake(self) -> bool:
        try:
            result = subprocess.run(
                ["cmake", "--version"],
                capture_output=True, text=True, timeout=5
            )
            if result.returncode == 0:
                self._ok(f"CMake found: {result.stdout.splitlines()[0]}")
                return True
        except FileNotFoundError:
            pass
        self._err("CMake not found. Install from https://cmake.org/download/")
        return False

    def _check_paths(self) -> bool:
        ok = True

        if not self.src_shaders.exists():
            self._err(f"Source shaders directory not found: {self.src_shaders}")
            ok = False
        else:
            self._ok(f"Source shaders: {self.src_shaders}")

        if not self.build_dir.exists():
            self._err(f"Build directory not found: {self.build_dir}")
            self._info("Run setup.py first to generate the build directory.")
            ok = False
        else:
            self._ok(f"Build directory:  {self.build_dir}")

        if not self.sync_script.exists():
            self._err(f"sync_shaders.cmake not found: {self.sync_script}")
            ok = False
        else:
            self._ok(f"Sync script:      {self.sync_script}")

        return ok

    # --------------------------------------------------------------- sync

    def _count_shaders(self, directory: Path) -> int:
        return sum(1 for f in directory.rglob("*") if f.suffix in SHADER_EXTENSIONS)

    def _sync_shaders(self) -> list[Path] | None:
        """
        Replicates what sync_shaders.cmake does, but file-by-file so each
        copy can be logged individually. Returns a list of copied files, or
        None on failure.
        """
        import shutil

        # Wipe destination so stale/moved files don't linger
        if self.dest_shaders.exists():
            self._info("Removing existing destination folder...")
            shutil.rmtree(self.dest_shaders)

        copied = []
        errors = []

        for src_file in self.src_shaders.rglob("*"):
            if src_file.is_dir() or src_file.suffix not in SHADER_EXTENSIONS:
                continue

            # Preserve sub-directory structure under dest_shaders
            rel_path  = src_file.relative_to(self.src_shaders)
            dest_file = self.dest_shaders / rel_path

            try:
                dest_file.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_file, dest_file)
                self._info(f"  Copied: {rel_path}")
                copied.append(dest_file)
            except Exception as e:
                self._err(f"  Failed to copy {rel_path}: {e}")
                errors.append(src_file)

        if errors:
            self._warn(f"{len(errors)} file(s) failed to copy.")
            return None

        return copied

    def run(self) -> bool:
        self._header()

        if not self._check_cmake():
            return False

        if not self._check_paths():
            return False

        before = self._count_shaders(self.src_shaders)
        self._info(f"Shader files found in source: {before}")
        self._info(f"Syncing  →  {self.dest_shaders}")

        copied = self._sync_shaders()
        if copied is None:
            return False

        if not self.silent:
            print(f"\n{Colors.GREEN}{Colors.BOLD}")
            print("=" * 60)
            print(" Shader sync complete!")
            print("=" * 60)
            print(Colors.RESET)
            print(f"  Shaders synced : {len(copied)}")
            print(f"  Destination    : {self.dest_shaders}\n")

        return True


# --------------------------------------------------------------------------- #

def parse_args():
    parser = argparse.ArgumentParser(
        description="Sync bak3d shader source files into the build directory."
    )
    parser.add_argument(
        "--silent",
        action="store_true",
        help="Suppress all prompts and banners (for programmatic / C++ invocation)."
    )
    parser.add_argument(
        "--build-dir",
        metavar="PATH",
        help="Override the build directory path (default: <repo_root>/build)."
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    try:
        syncer = ShaderSync(silent=args.silent, build_dir=args.build_dir)
        success = syncer.run()

        if not args.silent:
            input("\nPress Enter to close...")

        return 0 if success else 1
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}Cancelled.{Colors.RESET}")
        return 1
    except Exception as e:
        print(f"\n{Colors.RED}Unexpected error: {e}{Colors.RESET}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())