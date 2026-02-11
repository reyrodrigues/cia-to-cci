"""CIA to CCI conversion workflow."""

import tempfile
import shutil
from pathlib import Path
from typing import Optional

from . import ctrtool
from . import makerom

__all__ = ['CIAConverter', 'convert_cia_to_cci']


class CIAConverter:
    """Converts CIA files to decrypted CCI files."""

    def __init__(self, aes_keys_path: Optional[Path | str] = None):
        """
        Initialize converter.

        Args:
            aes_keys_path: Path to AES keys file (default: ~/.3ds/aes_keys.txt)
        """
        if aes_keys_path is None:
            aes_keys_path = Path.home() / ".3ds" / "aes_keys.txt"
        self.aes_keys_path = Path(aes_keys_path)

        # Warn if keys file doesn't exist
        if not self.aes_keys_path.exists():
            print(f"Warning: AES keys file not found at {self.aes_keys_path}")
            print("Decryption may fail without proper keys.")
            print("Place your aes_keys.txt in ~/.3ds/")

    def convert(
        self,
        cia_path: Path | str,
        output_path: Optional[Path | str] = None,
        keep_temp: bool = False
    ) -> Path:
        """
        Convert CIA to decrypted CCI.

        Args:
            cia_path: Path to input CIA file
            output_path: Optional output path (default: same name with .cci)
            keep_temp: Whether to keep temporary files for debugging

        Returns:
            Path to output CCI file
        """
        cia_path = Path(cia_path)

        if not cia_path.exists():
            raise FileNotFoundError(f"CIA file not found: {cia_path}")

        if output_path is None:
            output_path = cia_path.with_suffix('.cci')
        else:
            output_path = Path(output_path)

        print("=========================================")
        print("  CIA to CCI Converter (Decrypted)")
        print("=========================================")
        print(f"Input:  {cia_path}")
        print(f"Output: {output_path}")

        # Create temp directory
        temp_dir = Path(tempfile.mkdtemp(prefix='cia_to_cci_'))
        print(f"Temp:   {temp_dir}")
        print("-----------------------------------------")

        try:
            # Step 1: Extract CIA contents
            print("[1/6] Extracting contents from CIA...")
            main_content = ctrtool.extract_cia(cia_path, temp_dir)
            print(f"       Found main content: {main_content.name}")

            # Step 2: Read metadata
            print("[2/6] Reading metadata from CXI...")
            cxi_info = ctrtool.get_cxi_info(main_content)
            print(f"       Title ID: {cxi_info.title_id}")
            print(f"       Product Code: {cxi_info.product_code}")

            # Step 3: Extract CXI components
            print("[3/6] Extracting and decrypting CXI components...")
            exefs_dir = temp_dir / "exefs"
            exefs_dir.mkdir(exist_ok=True)

            ctrtool.extract_cxi(
                main_content,
                exheader_path=temp_dir / "exheader.bin",
                exefs_dir=exefs_dir,
                romfs_path=temp_dir / "romfs.bin",
                logo_path=temp_dir / "logo.bin",
                plain_path=temp_dir / "plain.bin",
                decompress_code=True
            )
            print("       Components extracted and decrypted.")

            # Step 4: Generate RSF
            print("[4/6] Generating RSF file...")
            rsf_path = temp_dir / "build.rsf"
            self._generate_rsf(rsf_path, cxi_info)
            print("       RSF file generated.")

            # Step 5: Build decrypted CXI
            print("[5/6] Rebuilding decrypted CXI...")
            decrypted_cxi = temp_dir / "decrypted.cxi"

            makerom.build_cxi(
                decrypted_cxi,
                rsf_path,
                temp_dir / "exheader.bin",
                exefs_dir / "code.bin",
                exefs_dir / "icon.bin",
                exefs_dir / "banner.bin",
                temp_dir / "romfs.bin" if (temp_dir / "romfs.bin").exists() else None,
                temp_dir / "logo.bin" if (temp_dir / "logo.bin").exists() else None,
                temp_dir / "plain.bin" if (temp_dir / "plain.bin").exists() else None
            )
            print("       Decrypted CXI built.")

            # Step 6: Build CCI
            print("[6/6] Building decrypted CCI...")
            makerom.build_cci(output_path, decrypted_cxi, content_index=0, content_id=0)

            print("=========================================")
            print(f"  Done! Output: {output_path}")
            print("  The CCI file is now decrypted.")
            print("=========================================")

            return output_path

        finally:
            if not keep_temp:
                print("Cleaning up temporary files...")
                shutil.rmtree(temp_dir)
            else:
                print(f"Temp files kept at: {temp_dir}")

    def _generate_rsf(self, rsf_path: Path, cxi_info: ctrtool.CXIInfo) -> None:
        """Generate RSF configuration file."""
        # Extract unique ID from title ID (last 5 hex digits before final 3)
        unique_id = cxi_info.title_id[-8:-3] if len(cxi_info.title_id) >= 8 else "00000"

        rsf_content = f"""BasicInfo:
  Title                 : "{cxi_info.product_code}"
  ProductCode           : "{cxi_info.product_code}"
  Logo                  : None

TitleInfo:
  UniqueId              : 0x{unique_id}
  Category              : Application

Option:
  UseOnSD               : true
  FreeProductCode       : true
  MediaFootPadding      : false
  EnableCrypt           : false
  EnableCompress        : false

AccessControlInfo:
  UseExtSaveData        : false
  UseOtherVariationSaveData : false
  IdealProcessor        : 0
  AffinityMask          : 1
  Priority              : 16
  MaxCpu                : 0x00
  ResourceLimitCategory : APPLICATION
  CoreVersion           : 2
  DescVersion           : 2
  MemoryType            : Application
  HandleTableSize       : 512
  SystemModeExt         : Legacy
  SystemMode            : 64MB
  ReleaseKernelMajor    : "02"
  ReleaseKernelMinor    : "33"

  FileSystemAccess:
   - DirectSdmc
   - CtrNandRo
   - CtrNandRw
   - CategorySystemApplication

  SystemCallAccess:
    ArbitrateAddress          : 34
    Break                     : 60
    CancelTimer               : 28
    ClearEvent                : 25
    ClearTimer                : 29
    CloseHandle               : 35
    ConnectToPort             : 45
    ControlMemory             : 1
    CreateAddressArbiter      : 33
    CreateEvent               : 23
    CreateMemoryBlock         : 30
    CreateMutex               : 19
    CreateSemaphore           : 21
    CreateThread              : 8
    CreateTimer               : 26
    DuplicateHandle           : 39
    ExitProcess               : 3
    ExitThread                : 9
    GetCurrentProcessorNumber : 17
    GetHandleInfo             : 41
    GetProcessId              : 53
    GetProcessIdOfThread      : 54
    GetProcessIdealProcessor  : 6
    GetProcessInfo            : 43
    GetResourceLimit          : 56
    GetResourceLimitCurrentValues : 58
    GetResourceLimitLimitValues : 57
    GetSystemInfo             : 42
    GetSystemTick             : 40
    GetThreadContext          : 59
    GetThreadId               : 55
    GetThreadIdealProcessor   : 15
    GetThreadInfo             : 44
    GetThreadPriority         : 11
    MapMemoryBlock            : 31
    OutputDebugString         : 61
    QueryMemory               : 2
    ReleaseMutex              : 20
    ReleaseSemaphore          : 22
    SendSyncRequest           : 50
    SetThreadPriority         : 12
    SetTimer                  : 27
    SignalEvent               : 24
    SleepThread               : 10
    UnmapMemoryBlock          : 32
    WaitSynchronization1      : 36
    WaitSynchronizationN      : 37

  ServiceAccessControl:
   - APT:U
   - ac:u
   - am:net
   - boss:U
   - cam:u
   - cecd:u
   - cfg:u
   - dlp:FKCL
   - dlp:SRVR
   - dsp::DSP
   - frd:u
   - fs:USER
   - gsp::Gpu
   - hid:USER
   - http:C
   - ir:USER
   - ir:u
   - ir:rst
   - mic:u
   - ndm:u
   - news:u
   - nwm::UDS
   - pdn:s
   - ptm:u
   - ptm:sysm
   - pxi:dev
   - soc:U
   - ssl:C
   - y2r:u
   - ldr:ro
   - csnd:SND
   - nim:aoc
   - ns:s
   - ps:ps
"""
        rsf_path.write_text(rsf_content)


def convert_cia_to_cci(
    cia_path: Path | str,
    output_path: Optional[Path | str] = None,
    aes_keys_path: Optional[Path | str] = None
) -> Path:
    """
    Convenience function to convert CIA to CCI.

    Args:
        cia_path: Path to input CIA file
        output_path: Optional output path
        aes_keys_path: Optional path to AES keys file

    Returns:
        Path to output CCI file
    """
    converter = CIAConverter(aes_keys_path)
    return converter.convert(cia_path, output_path)
