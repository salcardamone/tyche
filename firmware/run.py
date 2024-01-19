from pathlib import Path
from vunit import VUnit
from vunit.vhdl_standard import VHDL

def move_waveforms(results):
    sim_dir = Path(__file__).parent / "test" / "sim"
    sim_dir.mkdir(parents=True, exist_ok=True)

    sim_files = list(ROOT.glob("*.fst"))
    for sim_file in sim_files:
        new_file = sim_file.parents[0] / "test" / "sim" / sim_file.name
        sim_file.rename(new_file)

vu = VUnit.from_argv()
vu.add_vhdl_builtins()

ROOT = Path(__file__).parent

tyche_lib = vu.add_library("tyche_lib")
tyche_lib.add_source_files(ROOT / "components" / "*" / "*.vhd", vhdl_standard="2008")
tyche_lib.add_source_files(ROOT / "packages" / "*" / "*.vhd", vhdl_standard="2008")

tyche_tb_lib = vu.add_library("tyche_tb_lib")
tyche_tb_lib.add_source_files(ROOT / "test" / "components" / "*" / "*.vhd", vhdl_standard="2008")
tyche_tb_lib.add_source_files(ROOT / "test" / "components" / "*.vhd", vhdl_standard="2008")
tyche_tb_lib.add_source_files(ROOT / "test" / "packages" / "*" / "*.vhd", vhdl_standard="2008")

tyche_tb_lib.set_sim_option("nvc.sim_flags", ["-w", "--dump-arrays", "--format=fst"])

vu.main(post_run=move_waveforms)
