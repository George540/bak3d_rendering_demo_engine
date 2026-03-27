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
from typing import Optional, Tuple, List


class Colors:
    """ANSI color codes for terminal output"""
    RESET = "\033[0m"
    RED = "\033[91m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    CYAN = "\033[96m"
    BOLD = "\033[1m"
    
    # Fallback for Windows without ANSI support
    @classmethod
    def disable_on_windows(cls):
        """Disable colors on Windows if terminal doesn't support ANSI"""
        if platform.system() == "Windows":
            # Try to enable ANSI colors on Windows 10+
            if sys.platform == "win32":
                try:
                    import ctypes
                    kernel32 = ctypes.windll.kernel32
                    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)
                except:
                    # Fall back to no colors
                    cls.RESET = ""
                    cls.RED = ""
                    cls.GREEN = ""
                    cls.YELLOW = ""
                    cls.CYAN = ""
                    cls.BOLD = ""


class CMakeSetup:
    """Main CMake setup class"""
    
    # Visual Studio configurations
    VS_CONFIGS = {
        "1": {
            "name": "Visual Studio 2026",
            "generator": "Visual Studio 18 2026",
            "slnx": True,  # Supports .slnx format
            "min_cmake": "3.27"
        },
        "2": {
            "name": "Visual Studio 2022",
            "generator": "Visual Studio 17 2022",
            "slnx": False,  # Uses traditional .sln
            "min_cmake": "3.20"
        },
        "3": {
            "name": "Visual Studio 2019",
            "generator": "Visual Studio 16 2019",
            "slnx": False,
            "min_cmake": "3.20"
        },
    }
    
    ARCHITECTURES = {
        "1": {"name": "x64 (Recommended - 64-bit)", "arch": "x64"},
        "2": {"name": "Win32 (32-bit)", "arch": "Win32"},
    }
    
    UNIX_GENERATORS = {
        "1": {"name": "Unix Makefiles", "generator": "Unix Makefiles"},
        "2": {"name": "Ninja", "generator": "Ninja"},
        "3": {"name": "Xcode (macOS only)", "generator": "Xcode"},
    }
    
    def __init__(self):
        self.repo_root = Path(__file__).parent.resolve()
        self.build_dir = self.repo_root / "build"
        self.vs_version = None
        self.vs_config = None
        self.architecture = None
        self.generator = None
        self.use_slnx = False
        self.is_windows = platform.system() == "Windows"
        
        # Enable colors
        Colors.disable_on_windows()
    
    def print_header(self):
        """Print script header"""
        print(f"\n{Colors.CYAN}{Colors.BOLD}")
        print("=" * 70)
        print(" bak3d CMake Setup Script (Python)")
        print("=" * 70)
        print(f"{Colors.RESET}\n")
    
    def print_success(self, message: str):
        """Print success message"""
        print(f"{Colors.GREEN}[OK]{Colors.RESET} {message}")
    
    def print_error(self, message: str):
        """Print error message"""
        print(f"{Colors.RED}[ERROR]{Colors.RESET} {message}")
    
    def print_warning(self, message: str):
        """Print warning message"""
        print(f"{Colors.YELLOW}[WARNING]{Colors.RESET} {message}")
    
    def print_info(self, message: str):
        """Print info message"""
        print(f"{Colors.CYAN}[INFO]{Colors.RESET} {message}")
    
    def check_prerequisites(self) -> bool:
        """Check if all prerequisites are met"""
        print(f"\n{Colors.BOLD}Checking Prerequisites...{Colors.RESET}\n")
        
        # Check if we're in the right directory
        if not (self.repo_root / "bak3d").exists():
            self.print_error("Could not find 'bak3d' directory")
            print("Please run this script from the repository root directory")
            return False
        
        self.print_success("Found 'bak3d' directory")
        
        if not (self.repo_root / "CMakeLists.txt").exists():
            self.print_error("Could not find 'CMakeLists.txt'")
            print("Please ensure CMakeLists.txt is in the repository root")
            return False
        
        self.print_success("Found 'CMakeLists.txt'")
        
        # Check for CMake
        if not self.check_cmake():
            return False
        
        return True
    
    def check_cmake(self) -> bool:
        """Check if CMake is installed and get version"""
        try:
            result = subprocess.run(
                ["cmake", "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode == 0:
                version_line = result.stdout.split('\n')[0]
                self.print_success(f"CMake is installed: {version_line}")
                return True
            else:
                self.print_error("CMake is installed but returned an error")
                return False
        
        except FileNotFoundError:
            self.print_error("CMake is not installed or not in PATH")
            print(f"\n{Colors.YELLOW}Please install CMake from https://cmake.org/download/{Colors.RESET}")
            print(f"{Colors.YELLOW}Make sure to add CMake to PATH during installation{Colors.RESET}\n")
            return False
        except subprocess.TimeoutExpired:
            self.print_error("CMake check timed out")
            return False
    
    def select_platform(self) -> bool:
        """Select the appropriate platform/generator"""
        if self.is_windows:
            return self.select_visual_studio()
        else:
            return self.select_unix_generator()
    
    def select_visual_studio(self) -> bool:
        """Select Visual Studio version on Windows"""
        print(f"\n{Colors.BOLD}Choose Your Visual Studio Version:{Colors.RESET}\n")
        
        for key, config in self.VS_CONFIGS.items():
            slnx_note = " (uses .slnx)" if config["slnx"] else ""
            print(f"{key}. {config['name']}{slnx_note}")
        
        print("4. Cancel\n")
        
        choice = input("Enter your choice (1-4): ").strip()
        
        if choice == "4":
            self.print_info("Setup cancelled")
            return False
        
        if choice not in self.VS_CONFIGS:
            self.print_error("Invalid choice")
            return False
        
        self.vs_config = self.VS_CONFIGS[choice]
        self.vs_version = self.vs_config["name"]
        self.generator = self.vs_config["generator"]
        self.use_slnx = self.vs_config["slnx"]
        
        return self.select_architecture()
    
    def select_architecture(self) -> bool:
        """Select architecture (x64 or Win32)"""
        print(f"\n{Colors.BOLD}Choose Architecture:{Colors.RESET}\n")
        
        for key, config in self.ARCHITECTURES.items():
            print(f"{key}. {config['name']}")
        
        print("3. Cancel\n")
        
        choice = input("Enter your choice (1-3): ").strip()
        
        if choice == "3":
            self.print_info("Setup cancelled")
            return False
        
        if choice not in self.ARCHITECTURES:
            self.print_error("Invalid choice")
            return False
        
        self.architecture = self.ARCHITECTURES[choice]["arch"]
        return True
    
    def select_unix_generator(self) -> bool:
        """Select generator on Unix-like systems (Linux, macOS)"""
        print(f"\n{Colors.BOLD}Choose Build Generator:{Colors.RESET}\n")
        
        system = platform.system()
        if system == "Darwin":
            self.print_info("Detected macOS")
        else:
            self.print_info(f"Detected {system}")
        
        print("\nAvailable generators:\n")
        
        for key, config in self.UNIX_GENERATORS.items():
            # Skip Xcode on non-macOS
            if config["generator"] == "Xcode" and system != "Darwin":
                continue
            print(f"{key}. {config['name']}")
        
        print("4. Cancel\n")
        
        choice = input("Enter your choice: ").strip()
        
        if choice == "4":
            self.print_info("Setup cancelled")
            return False
        
        if choice not in self.UNIX_GENERATORS:
            self.print_error("Invalid choice")
            return False
        
        config = self.UNIX_GENERATORS[choice]
        self.generator = config["generator"]
        self.vs_version = config["name"]
        return True
    
    def print_summary(self):
        """Print configuration summary"""
        print(f"\n{Colors.BOLD}=" * 70)
        print("Configuration Summary:")
        print("=" * 70 + f"{Colors.RESET}\n")
        
        if self.is_windows:
            print(f"Visual Studio Version: {self.vs_version}")
            print(f"Architecture: {self.architecture}")
            if self.use_slnx:
                print(f"Solution Format: .slnx (newer format)")
            else:
                print(f"Solution Format: .sln (traditional format)")
        else:
            print(f"Build Generator: {self.vs_version}")
        
        print(f"Generator: {self.generator}")
        print(f"Repository Root: {self.repo_root}")
        print(f"Build Directory: {self.build_dir}\n")
    
    def create_build_directory(self) -> bool:
        """Create build directory if it doesn't exist"""
        try:
            if not self.build_dir.exists():
                self.print_info(f"Creating build directory: {self.build_dir}")
                self.build_dir.mkdir(parents=True, exist_ok=True)
            else:
                self.print_info(f"Build directory already exists: {self.build_dir}")
            
            return True
        except Exception as e:
            self.print_error(f"Failed to create build directory: {e}")
            return False
    
    def run_cmake(self) -> bool:
        """Run CMake to generate solution/project files"""
        print(f"\n{Colors.BOLD}Generating build configuration...{Colors.RESET}\n")
        
        try:
            # Change to build directory
            os.chdir(self.build_dir)
            
            # Prepare CMake command
            cmake_cmd = ["cmake"]
            cmake_cmd.extend(["-G", self.generator])
            
            # Add architecture for Visual Studio
            if self.is_windows and self.architecture:
                cmake_cmd.extend(["-A", self.architecture])
            
            # Add path to CMakeLists.txt (parent directory)
            cmake_cmd.append("..")
            
            # Run CMake
            self.print_info(f"Running: {' '.join(cmake_cmd)}")
            result = subprocess.run(
                cmake_cmd,
                capture_output=True,
                text=True,
                timeout=120
            )
            
            # Print output
            if result.stdout:
                print(result.stdout)
            
            if result.returncode != 0:
                if result.stderr:
                    print(result.stderr)
                self.print_error("CMake generation failed")
                return False
            
            self.print_success("CMake configuration completed successfully!")
            return True
        
        except subprocess.TimeoutExpired:
            self.print_error("CMake generation timed out")
            return False
        except Exception as e:
            self.print_error(f"CMake generation failed: {e}")
            return False
        finally:
            # Return to original directory
            os.chdir(self.repo_root)
    
    def find_solution_file(self) -> Optional[Path]:
        """Find the generated solution file"""
        if self.use_slnx:
            # Look for .slnx file (Visual Studio 2026)
            slnx_files = list(self.build_dir.glob("*.slnx"))
            if slnx_files:
                return slnx_files[0]
        
        # Look for traditional .sln file
        sln_files = list(self.build_dir.glob("*.sln"))
        if sln_files:
            return sln_files[0]
        
        return None
    
    def open_solution(self, solution_path: Path) -> bool:
        """Open the generated solution in Visual Studio"""
        try:
            self.print_info(f"Opening solution: {solution_path.name}")
            
            if self.is_windows:
                # Use 'start' command on Windows
                os.startfile(str(solution_path))
            elif platform.system() == "Darwin":
                # Use 'open' command on macOS
                subprocess.run(["open", str(solution_path)], check=False)
            else:
                # On Linux, try common editors
                for editor in ["code", "gedit", "nano"]:
                    if shutil.which(editor):
                        subprocess.run([editor, str(solution_path)], check=False)
                        break
            
            return True
        except Exception as e:
            self.print_error(f"Failed to open solution: {e}")
            return False
    
    def ask_open_solution(self, solution_path: Path) -> bool:
        """Ask user if they want to open the solution"""
        print(f"\n{Colors.BOLD}Solution generated successfully!{Colors.RESET}")
        print(f"Location: {solution_path.absolute()}\n")
        
        if self.is_windows:
            prompt = "Do you want to open the solution in Visual Studio now? (y/n): "
        else:
            prompt = "Do you want to open the solution? (y/n): "
        
        response = input(prompt).strip().lower()
        return response in ["y", "yes"]
    
    def print_next_steps(self, solution_path: Path):
        """Print next steps for the user"""
        print(f"\n{Colors.BOLD}=" * 70)
        print("SUCCESS! Build configuration generated successfully")
        print("=" * 70 + f"{Colors.RESET}\n")
        
        print(f"{Colors.BOLD}Next Steps:{Colors.RESET}")
        print(f"1. Open {solution_path.name if self.use_slnx else 'build/bak3d.sln'} in Visual Studio")
        print(f"2. Select Debug or Release configuration")
        print(f"3. Build → Build Solution")
        print(f"4. Run the project\n")
        
        print(f"{Colors.BOLD}Build Location:{Colors.RESET}")
        print(f"Debug:   {self.build_dir / 'Debug' / 'bin' / 'bak3d.exe'}")
        print(f"Release: {self.build_dir / 'Release' / 'bin' / 'bak3d.exe'}\n")
        
        if not self.is_windows:
            print(f"{Colors.BOLD}Command Line Build:{Colors.RESET}")
            print(f"cd {self.build_dir}")
            print(f"cmake --build . --config Release\n")
    
    def run(self) -> bool:
        """Main setup routine"""
        self.print_header()
        
        # Check prerequisites
        if not self.check_prerequisites():
            return False
        
        # Select platform/generator
        if not self.select_platform():
            return False
        
        # Print configuration summary
        self.print_summary()
        
        # Create build directory
        if not self.create_build_directory():
            return False
        
        # Run CMake
        if not self.run_cmake():
            return False
        
        # Find solution file
        solution_path = self.find_solution_file()
        if not solution_path:
            self.print_warning("Could not find generated solution file")
            self.print_info(f"Check {self.build_dir} directory manually")
            return False
        
        self.print_next_steps(solution_path)
        
        # Ask if user wants to open solution
        if self.ask_open_solution(solution_path):
            self.open_solution(solution_path)
        
        return True


def main():
    print("Running from:", Path.cwd())
    print("Script location:", Path(__file__).parent.resolve())
    os.chdir(Path(__file__).parent.resolve())
    """Main entry point"""
    try:
        setup = CMakeSetup()
        success = setup.run()
        
        if success:
            print(f"\n{Colors.GREEN}Setup completed successfully!{Colors.RESET}\n")
            return 0
        else:
            print(f"\n{Colors.RED}Setup failed or was cancelled.{Colors.RESET}\n")
            return 1
    
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
