#!/usr/bin/env python3
"""
Command-line interface for CIA to CCI converter.
Usage: python -m cia_to_cci <file.cia>
"""

import sys
import argparse
from pathlib import Path

from .converter import convert_cia_to_cci
from .ctrtool import CtrToolError
from .makerom import MakeRomError


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description='Converts and DECRYPTS a .cia file to a .cci file',
        epilog='Example: cia-to-cci game.cia'
    )

    parser.add_argument(
        'input_file',
        type=Path,
        help='Input CIA file to convert'
    )

    parser.add_argument(
        '-o', '--output',
        type=Path,
        default=None,
        help='Output CCI file path (default: same name with .cci extension)'
    )

    parser.add_argument(
        '--keys',
        type=Path,
        default=None,
        help='Path to AES keys file (default: ~/.3ds/aes_keys.txt)'
    )

    parser.add_argument(
        '--keep-temp',
        action='store_true',
        help='Keep temporary files for debugging'
    )

    args = parser.parse_args()

    # Validate input file
    if not args.input_file.exists():
        print(f"Error: File '{args.input_file}' not found.", file=sys.stderr)
        sys.exit(1)

    if args.input_file.suffix.lower() != '.cia':
        print("Error: Input file must be a .cia file.", file=sys.stderr)
        sys.exit(1)

    # Check for AES keys
    keys_path = args.keys or Path.home() / ".3ds" / "aes_keys.txt"
    if not keys_path.exists():
        print(f"Warning: AES keys file not found at {keys_path}", file=sys.stderr)
        print("Decryption may fail without the proper keys.", file=sys.stderr)
        print("Place your aes_keys.txt in ~/.3ds/", file=sys.stderr)
        print("", file=sys.stderr)

    try:
        # Import here to show usage errors quickly without loading the C++ modules
        from .converter import CIAConverter

        # Create converter
        converter = CIAConverter(aes_keys_path=keys_path if keys_path.exists() else None)

        # Convert
        output = converter.convert(
            args.input_file,
            args.output,
            keep_temp=args.keep_temp
        )

        sys.exit(0)

    except (CtrToolError, MakeRomError) as e:
        print(f"\nError during conversion: {e}", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n\nConversion interrupted by user.", file=sys.stderr)
        sys.exit(130)
    except Exception as e:
        print(f"\nUnexpected error: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
