library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library tyche_lib;
use tyche_lib.fixed_pkg.all;

--! @brief Multiplication of two complex numbers.
entity complex_complex_multiply is
  generic(
    --! Format of input real/ imaginary components
    fmt_arg    : fixed_t  := parse_format("Q8.8");
    --! Format of output real/ imaginary components
    fmt_result : fixed_t  := parse_format("Q8.8");
    --! Latency of the block in number of clock cycles. This is provided as a
    --! generic by the user and then checked within the latency defined within
    --! the architecture to guarantee agreement on how the block is used in
    --! structural design
    latency_   : positive := 4
    );
  port(
    --! Clock and reset signals
    clk, reset : in  std_logic;
    --! AXI handshake signals for upstream and downstream
    us_vld     : in  std_logic;
    us_rdy     : out std_logic;
    ds_rdy     : in  std_logic;
    ds_vld     : out std_logic;
    --! Real part of first number
    arg_a_re   : in  std_logic_vector(num_bits(fmt_arg) - 1 downto 0);
    --! Imaginary part of first number
    arg_a_im   : in  std_logic_vector(num_bits(fmt_arg) - 1 downto 0);
    --! Real part of second number
    arg_b_re   : in  std_logic_vector(num_bits(fmt_arg) - 1 downto 0);
    --! Imaginary part of first number
    arg_b_im   : in  std_logic_vector(num_bits(fmt_arg) - 1 downto 0);
    --! Real part of result
    result_re  : out std_logic_vector(num_bits(fmt_result) - 1 downto 0);
    --! Imaginary part of result
    result_im  : out std_logic_vector(num_bits(fmt_result) - 1 downto 0)
    );
end complex_complex_multiply;

--! @brief Pipelined architecture for complex-complex multiplication.
--!
--! Split into four pipeline stages:
--!   (1) Read inputs into registers.
--!   (2) Compute cross terms.
--!   (3) Form sum and difference between cross terms.
--!   (4) Truncate and round to output format.
architecture rtl of complex_complex_multiply is
  -- Latency of the pipeline
  constant latency : positive := 4;
  -- Format of product when forming cross terms from input
  constant fmt_inter : fixed_t := (
    is_signed     => fmt_arg.is_signed, num_int_bits => 2 * fmt_arg.num_int_bits,
    num_frac_bits => 2 * fmt_arg.num_frac_bits);

  subtype arg_t is std_logic_vector(num_bits(fmt_arg) - 1 downto 0);
  subtype inter_t is std_logic_vector(num_bits(fmt_inter) - 1 downto 0);
  subtype result_t is std_logic_vector(num_bits(fmt_result) - 1 downto 0);

  signal a_re_reg, a_im_reg     : arg_t    := (others => '0');
  signal b_re_reg, b_im_reg     : arg_t    := (others => '0');
  signal a_re_b_re, a_re_b_im   : inter_t  := (others => '0');
  signal a_im_b_re, a_im_b_im   : inter_t  := (others => '0');
  signal real_part, imag_part   : inter_t  := (others => '0');
  signal rounded_re, rounded_im : result_t := (others => '0');

  -- One element for each stage of the pipeline flagging whether it has valid
  -- output or not
  signal valid : std_logic_vector(latency - 1 downto 0) := (others => '0');
begin

  assert (latency_ = latency)
    report "Latency mismatch! Actual latency (" & positive'image(latency) &
    " cycles) differs from provided generic latency (" &
    positive'image(latency_) & " cycles)."
                     severity failure;

  -- We have valid data to output when the final pipeline stage is holding
  -- valid data
  ds_vld <= valid(latency - 1);
  -- When we don't have anything in the final stage of the pipeline, we can't
  -- overwrite any data, hence we can advance the pipeline regardless.
  -- Otherwise, if we do have data in the final stage of the pipeline, we check
  -- whether the consumer will be taking it on the next cycle or not
  us_rdy <= '1' when not valid(latency - 1) else ds_rdy;

  -- Register the data from input ports
  stage_one : process(clk) is
  begin
    if rising_edge(clk) then
      if reset = '1' then
        a_re_reg <= (others => '0');
        a_im_reg <= (others => '0');
        b_re_reg <= (others => '0');
        b_im_reg <= (others => '0');
        valid(0) <= '0';
      else
        a_re_reg <= arg_a_re;
        a_im_reg <= arg_a_im;
        b_re_reg <= arg_b_re;
        b_im_reg <= arg_b_im;
        if us_rdy = '1' then
          valid(0) <= us_vld;
        end if;
      end if;
    end if;
  end process stage_one;

  -- Form cross terms between real and imaginary parts of inputs
  stage_two : process(clk) is
  begin
    if rising_edge(clk) then
      if reset = '1' then
        a_re_b_re <= (others => '0');
        a_re_b_im <= (others => '0');
        a_im_b_re <= (others => '0');
        a_im_b_im <= (others => '0');
        valid(1)  <= '0';
      else
        if us_rdy = '1' then
          a_re_b_re <= multiply(a_re_reg, b_re_reg, fmt_arg, fmt_inter);
          a_re_b_im <= multiply(a_re_reg, b_im_reg, fmt_arg, fmt_inter);
          a_im_b_re <= multiply(a_im_reg, b_re_reg, fmt_arg, fmt_inter);
          a_im_b_im <= multiply(a_im_reg, b_im_reg, fmt_arg, fmt_inter);
          valid(1)  <= valid(0);
        end if;
      end if;
    end if;
  end process;

  -- Find sum and difference of cross terms
  stage_three : process(clk) is
  begin
    if rising_edge(clk) then
      if reset = '1' then
        real_part <= (others => '0');
        imag_part <= (others => '0');
        valid(2)  <= '0';
      else
        if us_rdy = '1' then
          real_part <= sub(a_re_b_re, a_im_b_im, fmt_inter, fmt_inter);
          imag_part <= add(a_re_b_im, a_im_b_re, fmt_inter, fmt_inter);
          valid(2)  <= valid(1);
        end if;
      end if;
    end if;
  end process;

  -- Truncate and round from intermediary format to result format
  stage_four : process(clk) is
  begin
    if rising_edge(clk) then
      if reset = '1' then
        rounded_re <= (others => '0');
        rounded_im <= (others => '0');
        valid(3)   <= '0';
      else
        if us_rdy = '1' then
          rounded_re <= cast(real_part, fmt_inter, fmt_result);
          rounded_im <= cast(imag_part, fmt_inter, fmt_result);
          valid(3)   <= valid(2);
        end if;
      end if;
    end if;
  end process;

  result_re <= rounded_re;
  result_im <= rounded_im;

end architecture rtl;
