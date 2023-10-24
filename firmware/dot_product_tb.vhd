library ieee;
use ieee.std_logic_1164.all;
use std.env.finish;
use work.vector_pkg.all;

library vunit_lib;
context vunit_lib.vunit_context;

entity dot_product_tb is
  generic (runner_cfg : string);
end dot_product_tb;

architecture dataflow of dot_product_tb is
  constant vector_length : positive                             := 8;
  signal a, b            : vector_t(vector_length - 1 downto 0) := (others => 0);
  signal dot             : integer;
begin
  dut : entity work.dot_product(behavioural)
    generic map (
      vector_length => vector_length
      )
    port map(
      a   => a,
      b   => b,
      dot => dot
      );
  abc : process is
  begin
    test_runner_setup(runner, runner_cfg);
    report "Hello, world!";
    wait for 1 ns;
    a(0) <= 1;
    b(0) <= 4;
    wait for 5 ns;
    assert dot = 4 report "Dot was " & integer'image(dot) severity failure;
    wait for 100 ns;
    test_runner_cleanup(runner);
  end process;
end dataflow;
