library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library vunit_lib;
context vunit_lib.vunit_context;

entity vector_pkg_tb is
  generic (runner_cfg : string);
end vector_pkg_tb;

architecture testbench of vector_pkg_tb is
  package vector_8_bit is new work.vector_pkg generic map (data_width => 8);
  package vector_16_bit is new work.vector_pkg generic map (data_width => 16);

  constant vector_length  : positive := 8;
  signal a_16bit, b_16bit : vector_16_bit.vector_t(0 to vector_length - 1);
  signal a_8bit, b_8bit   : vector_8_bit.vector_t(0 to vector_length - 1);
  signal result_8bit : vector_8_bit.element_t;
  signal result_16bit : vector_16_bit.element_t;
begin
  main : process is

-- =============================================================================

    -- @brief Test that the inner product function works as expected. 
    procedure test_inner_product is
    begin
      -- Single non-zero pair in vectors
      a_8bit(0) <= (1 => '1', others => '0');
      b_8bit(0) <= (1 => '1', others => '0');
      wait for 1 ns;
      result_8bit <= vector_8_bit.inner_product(a_8bit, b_8bit);
      wait for 1 ns;
      assert signed(result_8bit) = 4
       report "Inner product was " &
         integer'image(to_integer(signed(result_8bit)))
         severity failure;

      -- Second non-zero pair in vectors
      a_8bit(1) <= (1 => '1', others => '0');
      b_8bit(1) <= (1 => '1', others => '0');
      wait for 1 ns;
      result_8bit <= vector_8_bit.inner_product(a_8bit, b_8bit);
      wait for 1 ns;
      assert signed(result_8bit) = 8
       report "Inner product was " &
         integer'image(to_integer(signed(result_8bit)))
         severity failure;

      -- Third non-zero pair in vectors, non-trivial product
      a_8bit(vector_length - 1) <= (2 => '1', 3 => '1', others => '0');
      b_8bit(vector_length - 1) <= (0 => '1', 3 => '1', others => '0');
      wait for 1 ns;
      result_8bit <= vector_8_bit.inner_product(a_8bit, b_8bit);
      wait for 1ns;
      assert signed(result_8bit) = 116
       report "Inner product was " &
         integer'image(to_integer(signed(result_8bit)))
         severity failure;

      -- Overflow of representation to 0x80; wraps back to negative
      a_8bit(vector_length - 2) <= (0 => '1', others => '0');
      b_8bit(vector_length - 2) <= (2 => '1', 3 => '1', others => '0');
      wait for 1 ns;
      result_8bit <= vector_8_bit.inner_product(a_8bit, b_8bit);
      wait for 1 ns;
      assert signed(result_8bit) = -128
       report "Inner product was " &
         integer'image(to_integer(signed(result_8bit)))
         severity failure;

    end procedure test_inner_product;
      
-- =============================================================================

  begin
    test_runner_setup(runner, runner_cfg);

    test_inner_product;

    test_runner_cleanup(runner);
  end process;
end testbench;
