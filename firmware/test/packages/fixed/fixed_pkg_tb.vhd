library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library tyche_lib;
use tyche_lib.fixed_pkg.all;

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

    procedure test_addition is
      constant Q8_8    : fixed_t := parse_format("Q8.8");
      variable a, b, c : std_logic_vector(num_bits(Q8_8) - 1 downto 0);

      constant UQ4_4   : fixed_t := parse_format("UQ4.4");
      variable d, e, f : std_logic_vector(num_bits(UQ4_4) - 1 downto 0);
    begin
      -- 14.0 = 0'0000,1110.0000,0000 = 0xE00 = 3584
      a := std_logic_vector(to_signed(14, a'length) sll Q8_8.num_frac_bits);
      -- 6.0  = 0'0000,0110.0000,0000 = 0x60 = 96 
      b := std_logic_vector(to_signed(6, b'length) sll Q8_8.num_frac_bits);
      -- 14.0 + 6.0 = 20.0
      --      = 0'0001,0100.0000,0000 = 0x1400 = 5120
      c := add(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(20, c'length) sll Q8_8.num_frac_bits),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(5120, c'length)));

      -- 3.625 = 0'0000,0011.1010,0000 = 0x3A0 = 928
      a := std_logic_vector(to_signed(928, a'length));
      -- 16.75 = 0'0001,0000.1100,0000 = 0x10C0 = 4288
      b := std_logic_vector(to_signed(4288, b'length));
      -- 3.625 + 16.75 = 20.375
      --       = 0'0001,0100.0110,0000 = 0x1460 = 5216
      c := add(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(5216, c'length)),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(5216, c'length)));

      --   5.25 = 0'0000,0101.0100,0000 = 0x540 = 1344
      a := std_logic_vector(to_signed(1344, a'length));
      -- -12.75 = ¬(0'0000,1100.1100,0000) + 1
      --        = 1'1111,0011.0100,0000 = 0x1F340 = 127808
      b := std_logic_vector(to_signed(127808, b'length));
      -- 5.25 - 12.75 = -7.5
      --       = ¬(0'0000,0111.1000,0000) + 1
      --       = 1'1111,1000.1000,0000 = 0x1F880 = 129152
      c := add(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(129152, c'length)),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(129152, c'length)));
      
      -- 7.0 = 0111.0000 = 0x70 = 112
      d := std_logic_vector(to_unsigned(7, d'length) sll UQ4_4.num_frac_bits);
      -- 2.0 = 0010.0000 = 0x20 = 32
      e := std_logic_vector(to_unsigned(2, e'length) sll UQ4_4.num_frac_bits);
      -- 7.0 + 2.0 = 9.0
      --     = 1001.0000 = 0x90 = 144
      f := add(d, e, UQ4_4, UQ4_4);
      check(unsigned(f) = (to_unsigned(144, f'length)),
            "Computed value " & unsigned'image(unsigned(f)) &
            " does not match expected value of " &
            unsigned'image(to_unsigned(144, f'length)));
    end procedure test_addition;

-- =============================================================================

    -- @brief Test that we can convergently round an integer when discarding
    --        lower order bits.
    procedure test_multiply is
      constant Q8_8    : fixed_t := parse_format("Q8.8");
      variable a, b, c : std_logic_vector(num_bits(Q8_8) - 1 downto 0);

      constant UQ4_4   : fixed_t := parse_format("UQ4.4");
      variable d, e, f : std_logic_vector(num_bits(UQ4_4) - 1 downto 0);
    begin
      -- 14.0 = 0'0000,1110.0000,0000 = 0xE00 = 3584
      a := std_logic_vector(to_signed(14, a'length) sll Q8_8.num_frac_bits);
      -- 6.0  = 0'0000,0110.0000,0000 = 0x60 = 96 
      b := std_logic_vector(to_signed(6, b'length) sll Q8_8.num_frac_bits);
      -- 14.0 * 6.0 = 84.0
      --      = 0'0101,0100.0000,0000 = 0x5400 = 21504
      c := multiply(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(84, c'length) sll Q8_8.num_frac_bits),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(84, c'length)));

      -- 3.625 = 0'0000,0011.1010,0000 = 0x3A0 = 928
      a := std_logic_vector(to_signed(928, a'length));
      -- 16.75 = 0'0001,0000.1100,0000 = 0x10C0 = 4288
      b := std_logic_vector(to_signed(4288, b'length));
      -- 3.625 * 16.75 = 60.71875
      --       = 0'0011,1100.1011,1000 = 0x3CB8 = 15544
      c := multiply(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(15544, c'length)),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(15544, c'length)));

      -- -5.25 = ¬(0'0000,0101.0100,0000) + 1
      --       = 1'1111,1010.1100,0000 = 0x1FAC0 = 129728
      a := std_logic_vector(to_signed(129728, a'length));
      -- 12.75 = 0'0000,1100.1100,0000 = 0xCC0 = 3264
      b := std_logic_vector(to_signed(3264, b'length));
      -- -5.25 * 12.75 = -66.9375
      --       = ¬(0'0100,0010.1111,0000) + 1
      --       = 1'1011,1101.0001,0000 = 0x1BD10 = 113936
      c := multiply(a, b, Q8_8, Q8_8);
      check(signed(c) = (to_signed(113936, c'length)),
            "Computed value " & signed'image(signed(c)) &
            " does not match expected value of " &
            signed'image(to_signed(113936, c'length)));

      -- 7.0 = 0111.0000 = 0x70 = 112
      d := std_logic_vector(to_unsigned(7, d'length) sll UQ4_4.num_frac_bits);
      -- 2.0 = 0010.0000 = 0x20 = 32
      e := std_logic_vector(to_unsigned(2, e'length) sll UQ4_4.num_frac_bits);
      -- 7.0 * 2.0 = 14.0
      --     = 1110.0000 = 0xE0 = 224
      f := multiply(d, e, UQ4_4, UQ4_4);
      check(unsigned(f) = (to_unsigned(224, f'length)),
            "Computed value " & unsigned'image(unsigned(f)) &
            " does not match expected value of " &
            unsigned'image(to_unsigned(224, f'length)));
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
    test_addition;
    test_multiply;
    test_cast;

    test_runner_cleanup(runner);
  end process;
end testbench;
