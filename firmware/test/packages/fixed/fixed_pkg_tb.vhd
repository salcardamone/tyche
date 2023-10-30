library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.fixed_pkg.all;


library vunit_lib;
context vunit_lib.vunit_context;

entity fixed_pkg_tb is
  generic (runner_cfg : string);
end fixed_pkg_tb;

architecture testbench of fixed_pkg_tb is
begin
  main : process is

-- =============================================================================

    -- @brief Test that we can parse a Q-format from its corresponding string
    --        representation.
    procedure test_parse_format is
      variable fmt : fixed_t;
    begin
      fmt := parse_format("UQ3.5");
      assert fmt.is_signed = false
        report "Expected signedness to be unsigned."
        severity failure;
      assert fmt.num_int_bits = 3
        report "Number of integer bits was " & integer'image(fmt.num_int_bits)
        severity failure;
      assert fmt.num_frac_bits = 5
        report "Number of fractional bits was " & integer'image(fmt.num_frac_bits)
        severity failure;
      assert num_bits(fmt) = 8
        report "Number of total bits (" & integer'image(num_bits(fmt)) &
        ") did not match expectation."
        severity failure;

      fmt := parse_format("Q7.4");
      assert fmt.is_signed = true
        report "Expected signedness to be unsigned."
        severity failure;
      assert fmt.num_int_bits = 7
        report "Number of integer bits was " & integer'image(fmt.num_int_bits)
        severity failure;
      assert fmt.num_frac_bits = 4
        report "Number of fractional bits was " & integer'image(fmt.num_frac_bits)
        severity failure;
      assert num_bits(fmt) = 12
        report "Number of total bits (" & integer'image(num_bits(fmt)) &
        ") did not match expectation."
        severity failure;
    end procedure test_parse_format;

-- =============================================================================

    -- @brief Test that we can convergently round an integer when discarding
    --        lower order bits.
    procedure test_round is
      -- Container for result and expectation
      type result_t is record
        rnd : integer range 0 to 32; -- Number of bits to retain
        val : unsigned;              -- Value to round
        exp : unsigned;              -- Expected result
      end record result_t;

      -- @brief Wrapper for checking and reporting on rounding.
      procedure check(result : in result_t) is
        variable res : unsigned(result.rnd - 1 downto 0);
      begin
        res := round(result.val, result.rnd);
        assert res = result.exp
          report "Result (" & unsigned'image(res)
          & ") and Expected (" & unsigned'image(result.exp)
          & ") do not match."
          severity failure;
      end procedure check;

      variable zero : result_t(val(7 downto 0), exp(3 downto 0))
        := (rnd => 4, val => (others => '0'), exp => (others => '0'));
      variable rnd_down : result_t(val(7 downto 0), exp(3 downto 0))
        := (rnd => 4, val => "01101000", exp => "0110");
      variable rnd_up : result_t(val(7 downto 0), exp(3 downto 0))
        := (rnd => 4, val => "01111000", exp => "1000");
      variable rnd_neg_down : result_t(val(7 downto 0), exp(3 downto 0))
        := (rnd => 4, val => "10111100", exp => "1100");
      variable rnd_neg_up : result_t(val(7 downto 0), exp(3 downto 0))
        := (rnd => 4, val => "10101000", exp => "1010");
    begin
      check(zero);
      check(rnd_down);
      check(rnd_up);
      check(rnd_neg_down);
      check(rnd_neg_up);
    end procedure test_round;

-- =============================================================================

  begin
    test_runner_setup(runner, runner_cfg);

    test_parse_format;
    test_round;

    test_runner_cleanup(runner);
  end process;
end testbench;
