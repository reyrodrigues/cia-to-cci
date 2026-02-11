# CIA to CCI Converter

A Python library with C/C++ bindings for converting Nintendo 3DS CIA (CTR Importable Archive) files to decrypted CCI (CTR Cart Image) files.

## Features

- **Zero External Dependencies**: All tools (ctrtool, makerom) are statically linked into Python extension modules
- **Full Decryption Support**: Extracts and decrypts CXI components using AES keys
- **Native Performance**: C/C++ implementation with Python bindings via pybind11
- **Cross-Platform**: Works on macOS and Linux
- **Clean Python API**: High-level interface for easy integration
- **Low-Level Access**: Direct access to ctrtool and makerom functionality

## What This Tool Does

This library replicates the functionality of command-line tools `ctrtool` and `makerom` from the [Project_CTR](https://github.com/3DSGuy/Project_CTR) repository, packaged as a convenient Python library.

The conversion process:
1. **Extracts** contents from CIA files
2. **Reads** metadata (Title ID, Product Code) from CXI
3. **Extracts and decrypts** CXI components (ExeFS, RomFS, extended header)
4. **Generates** RSF configuration file with system calls and service access
5. **Rebuilds** decrypted CXI with all components
6. **Creates** final decrypted CCI file

## Requirements

### System Requirements
- **Python**: 3.8 or higher
- **Platform**: macOS or Linux
- **Build Tools** (for building from source):
  - CMake 3.15+
  - C++17 compatible compiler (GCC 7+, Clang 5+)
  - Python development headers

### AES Keys (Required)

This tool requires Nintendo 3DS AES keys for decryption. You must provide your own keys at:

```
~/.3ds/aes_keys.txt
```

**Important**: This project does NOT include or provide AES keys. You must obtain them legally through your own means.

## Installation

### From Source

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/reyrodrigues/cia-to-cci.git
cd cia-to-cci

# Build and install
pip install .
```

### Development Installation

```bash
# Install in editable mode
pip install -e .

# Run tests
pytest tests/
```

## Usage

### Python API

#### Basic Conversion

```python
from cia_to_cci import convert_cia_to_cci

# Convert CIA to decrypted CCI
output_path = convert_cia_to_cci("game.cia", "game.cci")
print(f"Converted successfully: {output_path}")
```

#### Advanced Usage

```python
from cia_to_cci import CIAConverter
from pathlib import Path

# Create converter with custom AES keys path
converter = CIAConverter(aes_keys_path=Path("custom/path/aes_keys.txt"))

# Convert with automatic output naming
output = converter.convert(Path("game.cia"))
```

#### Low-Level API

```python
from cia_to_cci import ctrtool, makerom
from pathlib import Path
import tempfile

with tempfile.TemporaryDirectory() as temp_dir:
    temp_path = Path(temp_dir)

    # Step 1: Extract CIA contents
    main_content = ctrtool.extract_cia("game.cia", temp_path)

    # Step 2: Get CXI metadata
    info = ctrtool.get_cxi_info(main_content)
    print(f"Title ID: {info.title_id}")
    print(f"Product Code: {info.product_code}")

    # Step 3: Extract CXI components
    ctrtool.extract_cxi(
        cxi_path=main_content,
        exheader_path=temp_path / "exheader.bin",
        exefs_dir=temp_path / "exefs",
        romfs_path=temp_path / "romfs.bin",
        logo_path=temp_path / "logo.bin",
        plain_path=temp_path / "plain.bin",
        decompress_code=True
    )

    # Step 4: Build decrypted CXI
    cxi_output = makerom.build_cxi(
        output_path=temp_path / "decrypted.cxi",
        rsf_path=temp_path / "build.rsf",
        exheader_path=temp_path / "exheader.bin",
        code_path=temp_path / "exefs/code.bin",
        icon_path=temp_path / "exefs/icon.bin",
        banner_path=temp_path / "exefs/banner.bin",
        romfs_path=temp_path / "romfs.bin"
    )

    # Step 5: Build final CCI
    cci_output = makerom.build_cci(
        output_path="game.cci",
        content_path=cxi_output
    )
```

## Building from Source

### Prerequisites

Install build dependencies:

**macOS:**
```bash
brew install cmake python3
```

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake python3-dev build-essential
```

### Build Steps

```bash
# Clone with submodules
git clone --recursive https://github.com/reyrodrigues/cia-to-cci.git
cd cia-to-cci

# Create virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install in development mode
pip install -e .
```

### Build Configuration

The project uses CMake for building native extensions. Build options:

```bash
# Release build (default) - optimized binaries
pip install -e .

# Debug build - for development
CMAKE_BUILD_TYPE=Debug pip install -e .
```

## Project Structure

```
cia_to_cci/
├── src/
│   └── cia_to_cci/
│       ├── __init__.py          # Main package exports
│       ├── converter.py         # High-level conversion API
│       ├── ctrtool.py          # ctrtool wrapper
│       ├── makerom.py          # makerom wrapper
│       └── _bindings/          # C++ pybind11 bindings
├── native/
│   ├── project_ctr/           # Git submodule: Project_CTR
│   ├── ctrtool_wrapper/       # C++ wrapper for ctrtool
│   └── makerom_wrapper/       # C wrapper for makerom
├── tests/                     # Unit and integration tests
├── CMakeLists.txt            # Root build configuration
└── pyproject.toml            # Python package metadata
```

## Dependencies

This project incorporates code from:

- **[Project_CTR](https://github.com/3DSGuy/Project_CTR)** - ctrtool and makerom (MIT License)
  - Copyright (c) 2014 3DSGuy
  - Copyright (c) 2014 applestash
  - Copyright (c) 2015-2022 Jakcron

All dependencies are statically linked, so no external installation is required.

## Troubleshooting

### "Failed to load cryptographic keys"

Ensure your AES keys file exists at `~/.3ds/aes_keys.txt` and contains valid keys.

### Build fails on macOS

Make sure you have Xcode Command Line Tools installed:
```bash
xcode-select --install
```

### Build fails on Linux

Install Python development headers:
```bash
sudo apt-get install python3-dev
```

### ImportError when importing module

Rebuild the extension modules:
```bash
pip install --force-reinstall -e .
```

## Legal Notice

**Important**: This tool is intended for legal use only, such as:
- Backup and preservation of games you legally own
- Development and homebrew purposes
- Educational use

Users are responsible for complying with all applicable laws regarding:
- Copyright and intellectual property
- DMCA and anti-circumvention statutes
- Software licensing agreements

This project does NOT provide or include:
- Nintendo 3DS AES keys
- Copyrighted game files
- Any circumvention tools

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## Testing

```bash
# Run tests
pytest tests/

# Run with coverage
pytest --cov=cia_to_cci tests/
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

This project incorporates code from [Project_CTR](https://github.com/3DSGuy/Project_CTR), which is also MIT licensed.

## Acknowledgments

- **3DSGuy, applestash, and Jakcron** for the original ctrtool and makerom implementations
- **Project_CTR contributors** for maintaining the tools
- **Nintendo** for the 3DS platform

---

**Disclaimer**: This tool is provided as-is for educational and legal purposes only. The authors are not responsible for any misuse of this software.
