library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use std.env.finish;

  --package new_vector_pkg is new work.vector_pkg generic map (data_width => 16);
  --use work.new_vector_pkg;

library vunit_lib;
context vunit_lib.vunit_context;

entity dot_product_tb is
  generic (runner_cfg : string);
end dot_product_tb;

architecture dataflow of dot_product_tb is
  package new_vector_pkg is new work.vector_pkg generic map (data_width => 16);
  constant vector_length : positive := 8;
  signal a, b            : new_vector_pkg.vector_t(vector_length - 1 downto 0)
    := (others => (others => '0'));
  signal dot : new_vector_pkg.element_type;
begin
  abc : process is
  begin
    test_runner_setup(runner, runner_cfg);
    a(0) <= (1 => '1', others => '0');
    b(0) <= (1 => '1', others => '0');
    wait for 1 ns;
    dot  <= new_vector_pkg.inner_product(a, b);
    wait for 1 ns;
    assert signed(dot) = 4 report "Dot was " & integer'image(to_integer(signed(dot))) severity failure;
    wait for 100 ns;
    test_runner_cleanup(runner);
  end process;
end dataflow;
