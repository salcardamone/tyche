from pathlib import Path
from vunit import VUnit

vu = VUnit.from_argv()
vu.add_vhdl_builtins()

lib = vu.add_library("lib")
lib.add_source_files(Path(__file__).parent / "packages" / "*.vhd")
lib.add_source_files(Path(__file__).parent / "components" / "dot_product" / "*.vhd")

vu.main()
