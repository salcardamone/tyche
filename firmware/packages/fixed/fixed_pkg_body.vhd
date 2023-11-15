library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;

package body fixed_pkg is

  -- =============================================================================

  -- @brief Find the position in a string where one of the characters appears.
  -- @param str The string to search for the character.
  -- @param chars A list of characters; will return the position of the first
  --              "hit" that's found.
  -- @return The position in the input string where the character was found.
  --         Will return -1 if no hits were found.
  pure function position(constant str : string; constant chars : string)
    return integer is
  begin
    for idx in str'range loop
      for jdx in chars'range loop
        if str(idx) = chars(jdx) then
          return idx;
        end if;
      end loop;
    end loop;
    return -1;
  end function position;

  -- =============================================================================

  -- @brief Determine whether a substring is contained within a string.
  -- @param str The string to search for the substring.
  -- @param substr The substring to search for within the string.
  -- @return True if the substring was found, else false.
  pure function substring(constant str : string; constant substr : string)
    return boolean is
  begin
    -- Iterate through each character in the substring
    str_loop : for idx in str'range loop
      -- If the first character in the substring is in the string, look forwards
      if substr(1) = str(idx) then
        substr_loop : for jdx in 2 to substr'length - 1 loop
          if substr(jdx) = str(idx + jdx) then
            next substr_loop;
          else
            next str_loop;
          end if;
        end loop;
        -- If we make it to here, we didn't move onto the next index in the
        -- str_loop, so the substring was found in its entirety 
        return true;
      end if;
    end loop;
    -- If we made it to here, nothing matched so the substring wasn't found
    return false;
  end function substring;

  -- =============================================================================

  pure function parse_format(constant fmt : string) return fixed_t is
    variable format     : fixed_t;
    variable end_prefix : integer := -1;
    variable sep_prefix : integer := -1;
  begin
    if not substring(fmt, "UQ") then
      if not substring(fmt, "Q") then
        report "Prefix UQ/Q was not found in Q-format specification: " & fmt
          severity failure;
      else
        format.is_signed := true;
      end if;
    else
      format.is_signed := false;
    end if;

    end_prefix := position(fmt, "Q");
    sep_prefix := position(fmt, ".");
    if (end_prefix = -1) or (sep_prefix = -1) then
      report "Couldn't identify valid Q-format in specification: " & fmt
        severity failure;
    end if;

    format.num_int_bits  := integer'value(fmt(end_prefix + 1 to sep_prefix - 1));
    format.num_frac_bits := integer'value(fmt(sep_prefix + 1 to fmt'length));

    return format;
  end function parse_format;

  -- =============================================================================

  pure function "="(constant lhs : fixed_t; constant rhs : fixed_t)
    return boolean is
  begin
    if (lhs.is_signed = rhs.is_signed) and
      (lhs.num_int_bits = rhs.num_int_bits) and
      (lhs.num_frac_bits = rhs.num_frac_bits) then
      return true;
    else
      return false;
    end if;
  end function "=";

  -- =============================================================================

  pure function to_string(constant fmt : fixed_t) return string is
    constant signed_prefix   : string := "Q";
    constant unsigned_prefix : string := "UQ";
  begin
    if fmt.is_signed then
      return signed_prefix & integer'image(fmt.num_int_bits) & "." &
        integer'image(fmt.num_frac_bits);
    else
      return unsigned_prefix & integer'image(fmt.num_int_bits) & "." &
        integer'image(fmt.num_frac_bits);
    end if;
  end function to_string;

  -- =============================================================================

  pure function num_bits(constant fmt : fixed_t) return integer is
    variable num_bits_tot : integer range 0 to 64 := 0;
  begin
    if fmt.is_signed then
      num_bits_tot := num_bits_tot + 1;
    end if;
    num_bits_tot := num_bits_tot + fmt.num_frac_bits + fmt.num_int_bits;
    return num_bits_tot;
  end function num_bits;

  -- =============================================================================

  pure function num_int_bits(constant fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return format.num_int_bits;
  end function num_int_bits;

  -- =============================================================================

  pure function num_frac_bits(constant fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return format.num_frac_bits;
  end function num_frac_bits;

  -- =============================================================================

  pure function num_bits(constant fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return num_bits(format);
  end function num_bits;

  -- =============================================================================

  pure function min_val(constant fmt : fixed_t) return std_logic_vector is
    variable result : std_logic_vector(num_bits(fmt) - 1 downto 0)
      := (others => '0');
  begin
    if fmt.is_signed = true then
      result(num_bits(fmt) - 1) := '1';
    end if;
    return result;
  end function min_val;

  -- =============================================================================

  pure function max_val(constant fmt : fixed_t) return std_logic_vector is
    variable result : std_logic_vector(num_bits(fmt) - 1 downto 0)
      := (others => '1');
  begin
    if fmt.is_signed = true then
      result(num_bits(fmt) - 1) := '0';
    end if;
    return result;
  end function max_val;

  -- =============================================================================

  pure function round(constant val              : unsigned;
                      constant num_bits_rounded : integer range 0 to 63)
    return unsigned is
    constant num_bits_discard       : integer := val'length - num_bits_rounded;
    constant final_bit_pos          : integer := num_bits_discard;
    constant first_bit_discard_pos  : integer := final_bit_pos - 1;
    constant second_bit_discard_pos : integer := final_bit_pos - 2;

    variable bits : std_logic_vector(val'length - 1 downto 0)
      := std_logic_vector(val);
    variable half_add : unsigned(num_bits_discard - 1 downto 0)
      := (others => '0');
    variable result : unsigned(val'length - 1 downto num_bits_discard);
  begin
    assert (num_bits_rounded <= val'length)
      report "Can't round a number to a larger length ("
      & integer'image(val'length) & " to "
      & integer'image(num_bits_rounded) & ")."
      severity failure;

    -- Don't need to do anything
    if (num_bits_discard = 0) then
      return val;
    end if;

    -- If we're just getting rid of one bit, then we need to see whether the
    -- value is even or odd, then check the discarded bit.
    --
    -- * If bits(final_bit_pos) = 1, then it's odd (so we'll have to round to
    --   even), else it's even (and we don't have to do anything).
    -- * If bits(first_bit_discard_pos) = 1, then we're going to have to round.
    --
    -- Hence...
    if (num_bits_discard = 1) then
      half_add(0) := bits(final_bit_pos) and bits(first_bit_discard_pos);

    -- Otherwise we have an additional stage...
    --
    -- * If any bits below the first discarded bit is set, we don't need to
    --   worry about rounding to even since we just round up normally.
    -- * If bits(final_bit_pos) = 1, then it's odd (so we'll have to round to
    --   even), else it's even (and we don't have to do anything). Combine this
    --   with the previous step.
    -- * If bits(first_bit_discard_pos) = 1, then we're going to have to round.
    else
      half_add(0) := or_reduce(bits(second_bit_discard_pos downto 0));
      half_add(0) := half_add(0) or bits(final_bit_pos);
      half_add(0) := half_add(0) and bits(first_bit_discard_pos);
    end if;

    -- Extract the bits we're keeping from the original value
    result := val(val'length - 1 downto num_bits_discard);
    -- Add the rounding bit
    result := result + half_add(0);
    return result;
  end function round;

  -- =============================================================================

  pure function addition_format(constant fmt_a : fixed_t; constant fmt_b : fixed_t)
    return fixed_t is
    variable result_fmt : fixed_t;
  begin
    if (fmt_a.is_signed or fmt_b.is_signed) then
      result_fmt.is_signed := true;
    else
      result_fmt.is_signed := false;
    end if;
    result_fmt.num_int_bits  := maximum(fmt_a.num_int_bits, fmt_b.num_int_bits) + 1;
    result_fmt.num_frac_bits := maximum(fmt_a.num_frac_bits, fmt_b.num_frac_bits);
    return result_fmt;
  end function addition_format;

-- =============================================================================

  pure function add(
    constant a      : std_logic_vector; constant b : std_logic_vector;
    constant fmt_in : fixed_t; constant fmt_out : fixed_t)
    return std_logic_vector is

    variable tmp : std_logic_vector(num_bits(fmt_in) - 1 downto 0);
    variable res : std_logic_vector(num_bits(fmt_out) - 1 downto 0);
  begin
    if (fmt_in.is_signed) then
      tmp := std_logic_vector(signed(a) + signed(b));
    else
      tmp := std_logic_vector(unsigned(a) + unsigned(b));
    end if;
    return cast(tmp, fmt_in, fmt_out);
  end function add;

-- =============================================================================

  pure function sub(
    constant a      : std_logic_vector; constant b : std_logic_vector;
    constant fmt_in : fixed_t; constant fmt_out : fixed_t)
    return std_logic_vector is

    variable tmp : std_logic_vector(num_bits(fmt_in) - 1 downto 0);
    variable res : std_logic_vector(num_bits(fmt_out) - 1 downto 0);
  begin
    if (fmt_in.is_signed) then
      tmp := std_logic_vector(signed(a) - signed(b));
    else
      tmp := std_logic_vector(unsigned(a) - unsigned(b));
    end if;
    return cast(tmp, fmt_in, fmt_out);
  end function sub;

-- =============================================================================

  pure function multiply(
    constant a      : std_logic_vector; constant b : std_logic_vector;
    constant fmt_in : fixed_t; constant fmt_out : fixed_t)
    return std_logic_vector is

    constant multiply_format : fixed_t := (
      is_signed     => fmt_in.is_signed, num_int_bits => 2 * fmt_in.num_int_bits,
      num_frac_bits => 2 * fmt_in.num_frac_bits
      );
    variable tmp : std_logic_vector(num_bits(multiply_format) - 1 downto 0);
    variable res : std_logic_vector(num_bits(fmt_out) - 1 downto 0);
  begin
    if (fmt_in.is_signed) then
      -- Multiplication of two signed values gives a result that's twice the
      -- original length. However, we don't can about doubling the sign bit, so
      -- we have to resize down by one here
      tmp := std_logic_vector(
        resize(signed(a) * signed(b), num_bits(multiply_format)));
    else
      tmp := std_logic_vector(unsigned(a) * unsigned(b));
    end if;
    return cast(tmp, multiply_format, fmt_out);
  end function multiply;

-- =============================================================================

  function cast(constant val     : std_logic_vector;
                constant old_fmt : fixed_t; constant new_fmt : fixed_t)
    return std_logic_vector is
    -- Remove the number of fractional bits from the original format and dump
    -- on the fractional bits from the new format
    constant num_bits_rounded : positive
      := num_bits(old_fmt) - old_fmt.num_frac_bits + new_fmt.num_frac_bits;

    variable rounded : unsigned(num_bits_rounded - 1 downto 0);
    variable result  : unsigned(num_bits(new_fmt) - 1 downto 0);
  begin
    assert old_fmt.is_signed = new_fmt.is_signed
      report "Both original and new Q-formats must have the same signedness."
      severity failure;

    rounded := round(unsigned(val), num_bits_rounded);

    -- Do some checks to make sure that we're not chopping off bits that have
    -- been set from the integer part of the number
    if old_fmt.is_signed = true then
      assert
        signed(rounded) <= signed(max_val(new_fmt))
        report "Can't cast " & std_logic_vector'image(val) & " from " &
        to_string(old_fmt) & " to " & to_string(new_fmt)
        severity failure;
      assert
        signed(rounded) >= signed(min_val(new_fmt))
        report "Can't cast " & std_logic_vector'image(val) & " from " &
        to_string(old_fmt) & " to " & to_string(new_fmt)
        severity failure;
    else
      assert
        unsigned(rounded) <= unsigned(max_val(new_fmt))
        report "Can't cast " & std_logic_vector'image(val) & " from " &
        to_string(old_fmt) & " to " & to_string(new_fmt)
        severity failure;
    end if;

    result := resize(rounded, num_bits(new_fmt));
    return std_logic_vector(result);
  end function cast;

-- =============================================================================

end package body fixed_pkg;
