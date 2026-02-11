"""High-level Python API for ctrtool functionality."""

from pathlib import Path
from typing import Optional
from dataclasses import dataclass

from ._ctrtool import CIAExtractor, CXIExtractor, CtrToolError

__all__ = ['extract_cia', 'extract_cxi', 'get_cxi_info', 'CXIInfo', 'CtrToolError']


@dataclass
class CXIInfo:
    """CXI metadata information."""
    title_id: str
    product_code: str


def extract_cia(cia_path: Path | str, output_dir: Path | str) -> Path:
    """
    Extract contents from a CIA file.

    Args:
        cia_path: Path to input CIA file
        output_dir: Directory to extract contents to

    Returns:
        Path to main content file (index 0000)

    Raises:
        CtrToolError: If extraction fails
    """
    cia_path = Path(cia_path)
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    extractor = CIAExtractor()
    extractor.extract_contents(str(cia_path), str(output_dir))

    # Find main content file
    content_files = sorted(output_dir.glob("contents.0000.*"))
    if not content_files:
        raise CtrToolError("Could not find main content (index 0000) in CIA")

    return content_files[0]


def extract_cxi(
    cxi_path: Path | str,
    exheader_path: Optional[Path | str] = None,
    exefs_dir: Optional[Path | str] = None,
    romfs_path: Optional[Path | str] = None,
    logo_path: Optional[Path | str] = None,
    plain_path: Optional[Path | str] = None,
    decompress_code: bool = True
) -> None:
    """
    Extract components from a CXI file.

    Args:
        cxi_path: Path to input CXI file
        exheader_path: Optional output path for extended header
        exefs_dir: Optional output directory for ExeFS components
        romfs_path: Optional output path for RomFS binary
        logo_path: Optional output path for logo
        plain_path: Optional output path for plain region
        decompress_code: Whether to decompress code.bin

    Raises:
        CtrToolError: If extraction fails
    """
    # Create directories if needed
    if exefs_dir:
        Path(exefs_dir).mkdir(parents=True, exist_ok=True)

    extractor = CXIExtractor()
    extractor.extract_components(
        str(cxi_path),
        str(exheader_path) if exheader_path else "",
        str(exefs_dir) if exefs_dir else "",
        str(romfs_path) if romfs_path else "",
        str(logo_path) if logo_path else "",
        str(plain_path) if plain_path else "",
        decompress_code
    )


def get_cxi_info(cxi_path: Path | str) -> CXIInfo:
    """
    Get metadata from a CXI file.

    Args:
        cxi_path: Path to CXI file

    Returns:
        CXIInfo with title_id and product_code

    Raises:
        CtrToolError: If reading metadata fails
    """
    extractor = CXIExtractor()
    info = extractor.get_info(str(cxi_path))
    return CXIInfo(title_id=info.title_id, product_code=info.product_code)
