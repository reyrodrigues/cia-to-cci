#!/usr/bin/env python3
"""
Example script for converting CIA to CCI.

Usage:
    python examples/convert_cia.py path/to/game.cia
"""

import sys
from pathlib import Path

# Add src to path for development
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

from cia_to_cci import convert_cia_to_cci, CtrToolError, MakeRomError


def main():
    if len(sys.argv) < 2:
        print("Usage: python convert_cia.py <file.cia>")
        print("Example: python convert_cia.py game.cia")
        sys.exit(1)

    cia_path = Path(sys.argv[1])

    if not cia_path.exists():
        print(f"Error: File '{cia_path}' not found.")
        sys.exit(1)

    if cia_path.suffix.lower() != '.cia':
        print("Error: Input file must be a .cia file.")
        sys.exit(1)

    try:
        # Convert CIA to CCI
        output = convert_cia_to_cci(cia_path)
        print(f"\nSuccess! Converted to: {output}")

    except (CtrToolError, MakeRomError) as e:
        print(f"\nError during conversion: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
