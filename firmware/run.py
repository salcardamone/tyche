from pathlib import Path
from vunit import VUnit

vu = VUnit.from_argv()
vu.add_vhdl_builtins()

lib = vu.add_library("lib")
lib.add_source_files(Path(__file__).parent / "packages" / "vector" / "*.vhd")
lib.add_source_files(Path(__file__).parent / "test" / "packages" / "vector" / "*.vhd")

lib.add_source_files(Path(__file__).parent / "packages" / "fixed" / "*.vhd")
lib.add_source_files(Path(__file__).parent / "test" / "packages" / "fixed" / "*.vhd")

lib.set_sim_option("nvc.sim_flags", ["--dump-arrays", "--format=fst"])

vu.main()
