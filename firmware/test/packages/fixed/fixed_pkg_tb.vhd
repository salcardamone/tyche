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
      variable parsed, expected : fixed_t;
    begin
      parsed   := parse_format("UQ3.5");
      expected := (is_signed => false, num_int_bits => 3, num_frac_bits => 5);
      check(parsed = expected,
            "Parsed Q-format does not match expected (" & to_string(parsed) &
            " != " & to_string(expected) & ").");

      parsed   := parse_format("Q7.4");
      expected := (is_signed => true, num_int_bits => 7, num_frac_bits => 4);
      check(parsed = expected,
            "Parsed Q-format does not match expected (" & to_string(parsed) &
            " != " & to_string(expected) & ").");
    end procedure test_parse_format;

-- =============================================================================

    -- @brief Test that we can convergently round an integer when discarding
    --        lower order bits.
    procedure test_round is
      -- Container for result and expectation
      type result_t is record
        rnd : integer range 0 to 32;    -- Number of bits to retain
        val : unsigned;                 -- Value to round
        exp : unsigned;                 -- Expected result
      end record result_t;

      -- @brief Wrapper for checking and reporting on rounding.
      procedure check(result : in result_t) is
        variable res : unsigned(result.rnd - 1 downto 0);
      begin
        res := round(result.val, result.rnd);
        check_equal(res, result.exp,
                    "Result (" & unsigned'image(res)
                    & ") and Expected (" & unsigned'image(result.exp)
                    & ") do not match.",
                    failure);
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
      variable not_byte : result_t(val(7 downto 0), exp(2 downto 0))
        := (rnd => 3, val => "01111111", exp => "100");
      variable newtest : result_t(val(8 downto 0), exp(6 downto 0))
        := (rnd => 7, val => "011111100", exp => "0111111");
    begin
      check(zero);
      check(rnd_down);
      check(rnd_up);
      check(rnd_neg_down);
      check(rnd_neg_up);
      check(not_byte);
      check(newtest);
    end procedure test_round;

-- =============================================================================

    -- @brief Test that we can convergently round an integer when discarding
    --        lower order bits.
    procedure test_multiply is
      variable lhs_fmt, rhs_fmt, res_fmt, exp_fmt : fixed_t;
    begin
      lhs_fmt := parse_format("Q8.8");
      rhs_fmt := parse_format("Q8.8");
      res_fmt := multiply_format(lhs_fmt, rhs_fmt);
      exp_fmt := (is_signed => true, num_int_bits => 16, num_frac_bits => 16);
      check(res_fmt = exp_fmt,
            to_string(lhs_fmt) & " * " & to_string(rhs_fmt) & " = " &
            to_string(exp_fmt) & ". Got: " & to_string(res_fmt));

      lhs_fmt := parse_format("UQ3.1");
      rhs_fmt := parse_format("Q7.8");
      res_fmt := multiply_format(lhs_fmt, rhs_fmt);
      exp_fmt := (is_signed => true, num_int_bits => 10, num_frac_bits => 9);
      check(res_fmt = exp_fmt,
            to_string(lhs_fmt) & " * " & to_string(rhs_fmt) & " = " &
            to_string(exp_fmt) & ". Got: " & to_string(res_fmt));
    end procedure test_multiply;

-- =============================================================================

    -- @brief Test that we can cast between Q-formats.
    procedure test_cast is
      -- Container for result and expectation
      type result_t is record
        start_fmt : fixed_t;
        end_fmt   : fixed_t;
        val       : std_logic_vector;   -- Value to round
        exp       : std_logic_vector;   -- Expected result
      end record result_t;

      -- @brief Wrapper for checking and reporting on casting.
      procedure check(result : in result_t) is
        variable res : std_logic_vector(num_bits(result.end_fmt) - 1 downto 0);
      begin
        res := cast(result.val, result.start_fmt, result.end_fmt);
        check_equal(res, result.exp,
                    "Casting " & std_logic_vector'image(result.val) & " from " &
                    to_string(result.start_fmt) & " to " &
                    to_string(result.end_fmt) & " should yield " &
                    std_logic_vector'image(result.exp) & ". Got " &
                    std_logic_vector'image(res));
      end procedure check;

      variable zero : result_t(val(7 downto 0), exp(4 downto 0))
        -- Zero in Q3.4 should stay at zero
        := (start_fmt => parse_format("Q3.4"), end_fmt => parse_format("Q2.2"),
            val       => "00000000", exp => "00000");
      -- 3.125 in Q3.4 should go to 3.0 in Q2.2
      variable rnd_down : result_t(val(7 downto 0), exp(4 downto 0))
        := (start_fmt => parse_format("Q3.4"), end_fmt => parse_format("Q2.2"),
            val       => "00110010", exp => "01100");
      -- 3.375 in Q3.4 should go to 3.5 in Q2.2
      variable rnd_up : result_t(val(7 downto 0), exp(4 downto 0))
        := (start_fmt => parse_format("Q3.4"), end_fmt => parse_format("Q2.2"),
            val       => "00110110", exp => "01110");
      -- -1.375 in Q4.4 should go to -1.5 in Q3.2 
      variable rnd_neg_down : result_t(val(8 downto 0), exp(5 downto 0))
        := (start_fmt => parse_format("Q4.4"), end_fmt => parse_format("Q3.2"),
            val       => "111101010", exp => "111010");
      -- -5.625 in Q4.4 should go to -5.5 in Q3.2
      variable rnd_neg_up : result_t(val(8 downto 0), exp(5 downto 0))
        := (start_fmt => parse_format("Q4.4"), end_fmt => parse_format("Q3.2"),
            val       => "110100110", exp => "101010");
    begin
      check(zero);
      check(rnd_down);
      check(rnd_up);
      check(rnd_neg_down);
      check(rnd_neg_up);
    end procedure test_cast;

-- =============================================================================

  begin
    test_runner_setup(runner, runner_cfg);

    test_parse_format;
    test_round;
    test_multiply;
    test_cast;

    test_runner_cleanup(runner);
  end process;
end testbench;
