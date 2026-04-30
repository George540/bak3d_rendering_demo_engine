#!/usr/bin/env python3
"""
bak3d CMake Setup Script (Python)

This script automates the CMake build setup process for the bak3d rendering 
demo engine. It works on Windows, macOS, and Linux.

Features:
- Cross-platform compatibility (Windows, macOS, Linux)
- Support for Visual Studio 2019, 2022, and 2026
- Support for both .sln (older) and .slnx (newer) file formats
- Automatic CMake detection and installation guidance
- Interactive setup with clear prompts
- Error handling and validation
- Color-coded output for better readability

Usage:
    python setup.py
    python3 setup.py
    
Requirements:
    - Python 3.6+
    - CMake 3.20+
"""

import os
import sys
import subprocess
import platform
import shutil
from pathlib import Path
from typing import Optional


# ---------------------------------------------------------------------------
# Colors
# ---------------------------------------------------------------------------

class Colors:
    """ANSI color codes for terminal output"""
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


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

SEP  = f"{Colors.CYAN}{'─' * 60}{Colors.RESET}"   # thin section divider
HSEP = f"{Colors.CYAN}{'═' * 60}{Colors.RESET}"   # heavy header/footer divider


def _tag(color: str, label: str, message: str) -> None:
    print(f"{color}[{label}]{Colors.RESET} {message}")

def print_ok(msg: str)      -> None: _tag(Colors.GREEN,  "OK",      msg)
def print_error(msg: str)   -> None: _tag(Colors.RED,    "ERROR",   msg)
def print_warning(msg: str) -> None: _tag(Colors.YELLOW, "WARNING", msg)
def print_info(msg: str)    -> None: _tag(Colors.CYAN,   "INFO",    msg)

def print_section(title: str) -> None:
    """Print a lightweight section header — no stacked blank lines."""
    print(f"\n{SEP}")
    print(f"  {Colors.BOLD}{title}{Colors.RESET}")
    print(SEP)


# ---------------------------------------------------------------------------
# CMakeSetup
# ---------------------------------------------------------------------------

class CMakeSetup:

    VS_CONFIGS = {
        "1": {"name": "Visual Studio 2026", "generator": "Visual Studio 18 2026",
              "slnx": True,  "min_cmake": "3.27"},
        "2": {"name": "Visual Studio 2022", "generator": "Visual Studio 17 2022",
              "slnx": False, "min_cmake": "3.20"},
        "3": {"name": "Visual Studio 2019", "generator": "Visual Studio 16 2019",
              "slnx": False, "min_cmake": "3.20"},
    }

    ARCHITECTURES = {
        "1": {"name": "x64 (Recommended — 64-bit)", "arch": "x64"},
        "2": {"name": "Win32 (32-bit)",              "arch": "Win32"},
    }

    UNIX_GENERATORS = {
        "1": {"name": "Unix Makefiles", "generator": "Unix Makefiles"},
        "2": {"name": "Ninja",          "generator": "Ninja"},
        "3": {"name": "Xcode (macOS)",  "generator": "Xcode"},
    }

    def __init__(self):
        self.repo_root    = Path(__file__).parent.resolve()
        self.build_dir    = self.repo_root / "build"
        self.vs_config    = None
        self.vs_version   = None
        self.architecture = None
        self.generator    = None
        self.use_slnx     = False
        self.is_windows   = platform.system() == "Windows"
        Colors.disable_on_windows()

    # ------------------------------------------------------------------
    # Header / footer
    # ------------------------------------------------------------------

    def print_header(self):
        print(f"\n{HSEP}")
        print(f"  {Colors.BOLD}{Colors.CYAN}bak3d CMake Setup Script{Colors.RESET}")
        print(HSEP)

    def print_footer(self, success: bool):
        print(f"\n{HSEP}")
        if success:
            print(f"  {Colors.GREEN}{Colors.BOLD}Setup completed successfully!{Colors.RESET}")
        else:
            print(f"  {Colors.RED}{Colors.BOLD}Setup failed or was cancelled.{Colors.RESET}")
        print(HSEP)

    # ------------------------------------------------------------------
    # Prerequisites
    # ------------------------------------------------------------------

    def check_prerequisites(self) -> bool:
        print_section("Prerequisites")

        if not (self.repo_root / "bak3d").exists():
            print_error("Could not find 'bak3d' directory")
            print("  Run this script from the repository root.")
            return False
        print_ok("bak3d/ directory found")

        if not (self.repo_root / "CMakeLists.txt").exists():
            print_error("Could not find CMakeLists.txt")
            return False
        print_ok("CMakeLists.txt found")

        return self.check_cmake()

    def check_cmake(self) -> bool:
        try:
            result = subprocess.run(
                ["cmake", "--version"],
                capture_output=True, text=True, timeout=5
            )
            if result.returncode == 0:
                version_line = result.stdout.split("\n")[0]
                print_ok(f"CMake found: {version_line}")
                return True
            print_error("CMake returned a non-zero exit code")
            return False
        except FileNotFoundError:
            print_error("CMake not found in PATH")
            print(f"  {Colors.YELLOW}Install from https://cmake.org/download/ and add to PATH{Colors.RESET}")
            return False
        except subprocess.TimeoutExpired:
            print_error("CMake version check timed out")
            return False

    # ------------------------------------------------------------------
    # Generator / platform selection
    # ------------------------------------------------------------------

    def select_platform(self) -> bool:
        return self.select_visual_studio() if self.is_windows else self.select_unix_generator()

    def select_visual_studio(self) -> bool:
        print_section("Visual Studio Version")

        for key, cfg in self.VS_CONFIGS.items():
            note = "  (.slnx)" if cfg["slnx"] else ""
            print(f"  {key}. {cfg['name']}{note}")
        print("  4. Cancel")

        choice = input("\nChoice (1-4): ").strip()
        if choice == "4":
            print_info("Setup cancelled")
            return False
        if choice not in self.VS_CONFIGS:
            print_error("Invalid choice")
            return False

        self.vs_config  = self.VS_CONFIGS[choice]
        self.vs_version = self.vs_config["name"]
        self.generator  = self.vs_config["generator"]
        self.use_slnx   = self.vs_config["slnx"]
        return self.select_architecture()

    def select_architecture(self) -> bool:
        print_section("Architecture")

        for key, cfg in self.ARCHITECTURES.items():
            print(f"  {key}. {cfg['name']}")
        print("  3. Cancel")

        choice = input("\nChoice (1-3): ").strip()
        if choice == "3":
            print_info("Setup cancelled")
            return False
        if choice not in self.ARCHITECTURES:
            print_error("Invalid choice")
            return False

        self.architecture = self.ARCHITECTURES[choice]["arch"]
        return True

    def select_unix_generator(self) -> bool:
        system = platform.system()
        print_section(f"Build Generator  [{system}]")

        for key, cfg in self.UNIX_GENERATORS.items():
            if cfg["generator"] == "Xcode" and system != "Darwin":
                continue
            print(f"  {key}. {cfg['name']}")
        print("  4. Cancel")

        choice = input("\nChoice: ").strip()
        if choice == "4":
            print_info("Setup cancelled")
            return False
        if choice not in self.UNIX_GENERATORS:
            print_error("Invalid choice")
            return False

        cfg            = self.UNIX_GENERATORS[choice]
        self.generator = cfg["generator"]
        self.vs_version = cfg["name"]
        return True

    # ------------------------------------------------------------------
    # Summary
    # ------------------------------------------------------------------

    def print_summary(self):
        print_section("Configuration Summary")
        if self.is_windows:
            fmt = ".slnx" if self.use_slnx else ".sln"
            print(f"  VS version   : {self.vs_version}")
            print(f"  Architecture : {self.architecture}")
            print(f"  Solution fmt : {fmt}")
        else:
            print(f"  Generator    : {self.vs_version}")
        print(f"  CMake gen    : {self.generator}")
        print(f"  Repo root    : {self.repo_root}")
        print(f"  Build dir    : {self.build_dir}")

    # ------------------------------------------------------------------
    # Build directory
    # ------------------------------------------------------------------

    def create_build_directory(self) -> bool:
        try:
            if not self.build_dir.exists():
                print_info(f"Creating build dir: {self.build_dir}")
                self.build_dir.mkdir(parents=True, exist_ok=True)
            else:
                print_info(f"Build dir exists: {self.build_dir}")
            return True
        except Exception as e:
            print_error(f"Failed to create build dir: {e}")
            return False

    # ------------------------------------------------------------------
    # CMake generation
    # ------------------------------------------------------------------

    def run_cmake(self) -> bool:
        print_section("CMake Generation")

        try:
            os.chdir(self.build_dir)

            cmake_cmd = ["cmake", "-G", self.generator]
            if self.is_windows and self.architecture:
                cmake_cmd += ["-A", self.architecture]
            cmake_cmd.append("..")

            print_info(f"Command: {' '.join(cmake_cmd)}")
            print()

            result = subprocess.run(
                cmake_cmd,
                capture_output=True, text=True, timeout=120
            )

            if result.stdout:
                print(result.stdout)
            if result.returncode != 0:
                if result.stderr:
                    print(result.stderr)
                print_error("CMake generation failed")
                return False

            print_ok("CMake generation succeeded")
            return True

        except subprocess.TimeoutExpired:
            print_error("CMake timed out")
            return False
        except Exception as e:
            print_error(f"CMake generation error: {e}")
            return False
        finally:
            os.chdir(self.repo_root)

    # ------------------------------------------------------------------
    # Solution file
    # ------------------------------------------------------------------

    def find_solution_file(self) -> Optional[Path]:
        if self.use_slnx:
            files = list(self.build_dir.glob("*.slnx"))
            if files:
                return files[0]
        files = list(self.build_dir.glob("*.sln"))
        return files[0] if files else None

    def open_solution(self, solution_path: Path) -> bool:
        try:
            print_info(f"Opening: {solution_path.name}")
            if self.is_windows:
                os.startfile(str(solution_path))
            elif platform.system() == "Darwin":
                subprocess.run(["open", str(solution_path)], check=False)
            else:
                for editor in ["code", "gedit", "nano"]:
                    if shutil.which(editor):
                        subprocess.run([editor, str(solution_path)], check=False)
                        break
            return True
        except Exception as e:
            print_error(f"Could not open solution: {e}")
            return False

    def ask_open_solution(self, solution_path: Path) -> bool:
        print_ok(f"Solution ready: {solution_path.absolute()}")
        verb = "open in Visual Studio" if self.is_windows else "open"
        response = input(f"\nDo you want to {verb} now? (y/n): ").strip().lower()
        return response in ("y", "yes")

    # ------------------------------------------------------------------
    # Next-steps guidance
    # ------------------------------------------------------------------

    def print_next_steps(self, solution_path: Path):
        print_section("Next Steps")
        sln_name = solution_path.name if self.use_slnx else "build/bak3d.sln"
        print(f"  1. Open  {sln_name}  in Visual Studio")
        print(f"  2. Select Debug or Release configuration")
        print(f"  3. Build → Build Solution  (Ctrl+Shift+B)")
        print(f"  4. Run bak3d")
        print()
        print(f"  Debug   bin: {self.build_dir / 'Debug'   / 'bin' / 'bak3d.exe'}")
        print(f"  Release bin: {self.build_dir / 'Release' / 'bin' / 'bak3d.exe'}")
        if not self.is_windows:
            print()
            print(f"  CLI build:")
            print(f"    cd {self.build_dir}")
            print(f"    cmake --build . --config Release")

    # ------------------------------------------------------------------
    # Main
    # ------------------------------------------------------------------

    def run(self) -> bool:
        self.print_header()

        if not self.check_prerequisites():
            return False
        if not self.select_platform():
            return False

        self.print_summary()

        if not self.create_build_directory():
            return False
        if not self.run_cmake():
            return False

        solution_path = self.find_solution_file()
        if not solution_path:
            print_warning("Generated solution file not found")
            print_info(f"Check {self.build_dir} manually")
            return False

        self.print_next_steps(solution_path)

        if self.ask_open_solution(solution_path):
            self.open_solution(solution_path)

        return True


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> int:
    os.chdir(Path(__file__).parent.resolve())
    try:
        setup = CMakeSetup()
        success = setup.run()
        setup.print_footer(success)
        return 0 if success else 1
    except KeyboardInterrupt:
        print(f"\n\n{Colors.YELLOW}Setup cancelled by user.{Colors.RESET}\n")
        return 1
    except Exception as e:
        print(f"\n{Colors.RED}Unexpected error: {e}{Colors.RESET}\n")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
