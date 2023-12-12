from pathlib import Path
from vunit import VUnit

vu = VUnit.from_argv()
vu.add_vhdl_builtins()

lib = vu.add_library("lib")
lib.add_source_files(Path(__file__).parent / "packages" / "fixed" / "*.vhd")
lib.add_source_files(Path(__file__).parent / "test" / "packages" / "fixed" / "*.vhd")

#lib.add_source_files(Path(__file__).parent / "components" / "complex_complex_multiply" / "*.vhd")
#lib.add_source_files(
#    Path(__file__).parent / "test" / "components" / "complex_complex_multiply" / "*.vhd"
#)

lib.add_source_files(Path(__file__).parent / "components" / "axi_lite_multiply" / "*.vhd")
lib.add_source_files(
    Path(__file__).parent / "test" / "components" / "axi_lite_multiply" / "*.vhd"
)

lib.set_sim_option("nvc.sim_flags", ["--dump-arrays", "--format=fst"])

vu.main()
