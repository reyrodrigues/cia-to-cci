"""High-level Python API for makerom functionality."""

from pathlib import Path
from typing import Optional

from ._makerom import CXIBuilder, CCIBuilder, MakeRomError

__all__ = ['build_cxi', 'build_cci', 'MakeRomError']


def build_cxi(
    output_path: Path | str,
    rsf_path: Path | str,
    exheader_path: Path | str,
    code_path: Path | str,
    icon_path: Path | str,
    banner_path: Path | str,
    romfs_path: Optional[Path | str] = None,
    logo_path: Optional[Path | str] = None,
    plain_path: Optional[Path | str] = None
) -> Path:
    """
    Build a CXI file from components.

    Args:
        output_path: Output path for CXI file
        rsf_path: Path to RSF configuration file
        exheader_path: Path to extended header
        code_path: Path to code.bin
        icon_path: Path to icon.bin
        banner_path: Path to banner.bin
        romfs_path: Optional path to RomFS binary
        logo_path: Optional path to logo
        plain_path: Optional path to plain region

    Returns:
        Path to output CXI file

    Raises:
        MakeRomError: If building fails
    """
    output_path = Path(output_path)

    builder = CXIBuilder()
    builder.build_cxi(
        str(output_path),
        str(rsf_path),
        str(exheader_path),
        str(code_path),
        str(icon_path),
        str(banner_path),
        str(romfs_path) if romfs_path else "",
        str(logo_path) if logo_path else "",
        str(plain_path) if plain_path else ""
    )

    return output_path


def build_cci(
    output_path: Path | str,
    content_path: Path | str,
    content_index: int = 0,
    content_id: int = 0
) -> Path:
    """
    Build a CCI file from CXI content.

    Args:
        output_path: Output path for CCI file
        content_path: Path to CXI content
        content_index: Content index (usually 0)
        content_id: Content ID (usually 0)

    Returns:
        Path to output CCI file

    Raises:
        MakeRomError: If building fails
    """
    output_path = Path(output_path)

    builder = CCIBuilder()
    builder.build_cci(
        str(output_path),
        str(content_path),
        content_index,
        content_id
    )

    return output_path
