"""Setup script for cia-to-cci package with CMake build support."""

import os
import sys
import subprocess
from pathlib import Path
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    """Extension class that uses CMake to build."""

    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    """Custom build extension that uses CMake."""

    def run(self):
        """Run CMake build for all extensions."""
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        """Build a single extension with CMake."""
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Required for auto-detection of Python module location
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = [
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
        ]

        build_args = ['--config', 'Release']

        # Platform-specific CMake arguments
        if sys.platform == 'darwin':
            # Build universal binaries for macOS (x86_64 + arm64)
            cmake_args += [
                '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.14',
                '-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64',
            ]
        elif sys.platform.startswith('linux'):
            # Linux-specific settings for manylinux compatibility
            cmake_args += [
                '-DCMAKE_CXX_FLAGS=-fPIC',
                '-DCMAKE_C_FLAGS=-fPIC',
            ]
        elif sys.platform == 'win32':
            # Windows-specific settings
            cmake_args += [
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE={}'.format(extdir.replace('\\', '/')),
            ]
            # Use Release configuration for Windows
            if self.debug:
                build_args += ['--config', 'Debug']
            else:
                build_args = ['--config', 'Release'] + build_args

        # Add parallel build support
        if hasattr(self, 'parallel') and self.parallel:
            build_args += [f'-j{self.parallel}']
        else:
            # Default to using all available cores
            import multiprocessing
            build_args += [f'-j{multiprocessing.cpu_count()}']

        # Create build directory
        env = os.environ.copy()
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        # Run CMake configure
        print(f"Configuring CMake with args: {cmake_args}")
        subprocess.check_call(
            ['cmake', ext.sourcedir] + cmake_args,
            cwd=self.build_temp,
            env=env
        )

        # Run CMake build
        print(f"Building with args: {build_args}")
        subprocess.check_call(
            ['cmake', '--build', '.'] + build_args,
            cwd=self.build_temp
        )


# Read README for long description
readme_path = Path(__file__).parent / "README.md"
long_description = ""
if readme_path.exists():
    long_description = readme_path.read_text(encoding='utf-8')

setup(
    name='cia-to-cci',
    version='0.1.6',
    author='Rey Rodrigues',
    description='Python library for converting Nintendo 3DS CIA files to decrypted CCI files',
    long_description=long_description,
    long_description_content_type='text/markdown',
    ext_modules=[
        CMakeExtension('cia_to_cci._ctrtool'),
        CMakeExtension('cia_to_cci._makerom'),
    ],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
    python_requires='>=3.8',
    packages=find_packages(where='src'),
    package_dir={'': 'src'},
)
