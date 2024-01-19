library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library tyche_lib;
use tyche_lib.fixed_pkg.all;

library vunit_lib;
context vunit_lib.vunit_context;

entity complex_complex_multiply_tb is
  generic (runner_cfg : string);
end complex_complex_multiply_tb;

architecture testbench of complex_complex_multiply_tb is
  constant Q4_4       : fixed_t  := parse_format("Q4.4");
  constant clk_period : time     := 1 ns;
  constant latency    : positive := 4;

  subtype val_t is std_logic_vector(num_bits(Q4_4) - 1 downto 0);

  signal clk, reset     : std_ulogic := '0';
  signal ds_vld, ds_rdy : std_ulogic := '0';
  signal us_vld, us_rdy : std_ulogic := '0';

  signal a_re, a_im : val_t := (others => '0');
  signal b_re, b_im : val_t := (others => '0');
  signal c_re, c_im : val_t := (others => '0');
begin

  clock : process is
  begin
    clk <= not clk;
    wait for clk_period / 2;
  end process;

  multiplier : entity tyche_lib.complex_complex_multiply(rtl)
    generic map(
      fmt_arg    => Q4_4,
      fmt_result => Q4_4,
      latency_   => latency
      )
    port map(
      clk       => clk,
      reset     => reset,
      us_vld    => ds_vld, us_rdy => ds_rdy,
      ds_vld    => us_vld, ds_rdy => us_rdy,
      arg_a_re  => a_re, arg_a_im => a_im,
      arg_b_re  => b_re, arg_b_im => b_im,
      result_re => c_re, result_im => c_im
      );

  -- Test both the AXI interface to the complex-complex multiplier, along with
  -- its ability to perform multiplications
  main : process is
    variable expected : std_logic_vector(num_bits(Q4_4) - 1 downto 0);
  begin

    test_runner_setup(runner, runner_cfg);

    wait for clk_period;

    ds_vld <= '1';
    us_rdy <= '1';

    -- 1.0 = 0x10 = 0'0001.0000 = 16
    a_re <= std_logic_vector(to_signed(16, a_re'length));
    -- 2.0 = 0x20 = 0'0010.0000 = 32
    a_im <= std_logic_vector(to_signed(32, a_im'length));

    -- 3.0 = 0x30 = 0'0011.0000 = 48
    b_re <= std_logic_vector(to_signed(48, b_re'length));
    -- 4.0 = 0x40 = 0'0100.0000 = 64
    b_im <= std_logic_vector(to_signed(64, b_im'length));

    wait for clk_period;

    -- 1.5 = 0x18 = 0'0001.1000 = 24
    a_re <= std_logic_vector(to_signed(24, a_re'length));
    -- 2.25 = 0x24 = 0'0010.0100 = 36
    a_im <= std_logic_vector(to_signed(36, a_im'length));

    -- 0.75 = 0xC = 0'0000.1100 = 12
    b_re <= std_logic_vector(to_signed(12, b_re'length));
    -- -3.125 = 0x1CE = 1'1100.1110 = 462
    b_im <= std_logic_vector(to_signed(462, b_im'length));

    wait for clk_period;

    -- No more data to give the multiplier
    ds_vld <= '0';
    -- Stall the pipeline; we're not ready to accept any more data
    us_rdy <= '0';

    -- The pipeline should continue to advance until valid data reaches the
    -- final stage, at which point is should stall
    wait for 10 * clk_period;

    -- Subsequent clock cycles should give us valid data
    us_rdy <= '1';

    wait for clk_period;

    -- (1.0 + 2.0i) * (3.0 + 4.0i)
    -- = 3.0 + 4.0i + 6.0i - 8.0
    -- = -5.0 + 10.0i
    -- 0'0101.0000 -> 1'1011.0000 = 0x1B0
    -- 0'1010.0000 -> 0xA0
    expected := std_logic_vector(to_signed(432, c_re'length));
    check(c_re = expected,
          "Expected equality between " & std_logic_vector'image(c_re) & " and " &
          std_logic_vector'image(expected));
    expected := std_logic_vector(to_signed(160, c_re'length));
    check(c_im = expected,
          "Expected equality between " & std_logic_vector'image(c_im) & " and " &
          std_logic_vector'image(expected));

    wait for clk_period;

    -- (1.5 + 2.25i) * (0.75 - 3.125i)
    -- = 1.125 - 4.6875i + 1.6875i + 7.03125
    -- = 8.15625 - 3i
    expected := std_logic_vector(to_signed(130, c_im'length));
    check(c_re = expected,
          "Expected equality between " & std_logic_vector'image(c_re) & " and " &
          std_logic_vector'image(expected));
    expected := std_logic_vector(to_signed(464, c_im'length));
    check(c_im = expected,
          "Expected equality between " & std_logic_vector'image(c_im) & " and " &
          std_logic_vector'image(expected));

    -- Reset the multiplier
    reset <= '1';

    wait for clk_period;

    expected := std_logic_vector(to_signed(0, c_re'length));
    check(c_re = expected,
          "Expected reset to have zeroed real output. Instead got " &
          std_logic_vector'image(c_re));
    check(c_im = expected,
          "Expected reset to have zeroed imaginary output. Instead got " &
          std_logic_vector'image(c_im));

    test_runner_cleanup(runner);
  end process;
end testbench;
