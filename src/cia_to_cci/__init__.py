"""CIA to CCI Converter - Python library for converting 3DS CIA files to decrypted CCI files.

This package provides Python bindings to the Project_CTR tools (ctrtool and makerom)
for converting Nintendo 3DS CIA files to decrypted CCI files.
"""

__version__ = "0.1.0"

from .converter import CIAConverter, convert_cia_to_cci
from .ctrtool import CtrToolError, extract_cia, extract_cxi, get_cxi_info, CXIInfo
from .makerom import MakeRomError, build_cxi, build_cci

__all__ = [
    "__version__",
    # Main API
    "CIAConverter",
    "convert_cia_to_cci",
    # ctrtool functions
    "CtrToolError",
    "extract_cia",
    "extract_cxi",
    "get_cxi_info",
    "CXIInfo",
    # makerom functions
    "MakeRomError",
    "build_cxi",
    "build_cci",
]
