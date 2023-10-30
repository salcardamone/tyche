library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;

package body fixed_pkg is

-- =============================================================================

  pure function parse_format(fmt : string) return fixed_t is
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

  pure function num_bits(fmt : fixed_t) return integer is
    variable num_bits_tot : integer range 0 to 32 := 0;
  begin
    if fmt.is_signed then
      num_bits_tot := num_bits_tot + 1;
    end if;
    num_bits_tot := num_bits_tot + fmt.num_frac_bits + fmt.num_int_bits;
    return num_bits_tot;
  end function num_bits;

-- =============================================================================

  pure function num_int_bits(fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return format.num_int_bits;
  end function num_int_bits;

-- =============================================================================

  pure function num_frac_bits(fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return format.num_frac_bits;
  end function num_frac_bits;

-- =============================================================================

  pure function num_bits(fmt : string) return integer is
    variable format : fixed_t;
  begin
    format := parse_format(fmt);
    return num_bits(format);
  end function num_bits;

-- =============================================================================

  pure function round(val                       : unsigned;
                      constant num_bits_rounded : integer range 0 to 63)
    return unsigned is
    constant num_bits_discard       : integer := val'length - num_bits_rounded;
    constant final_bit_pos          : integer := num_bits_rounded;
    constant first_bit_discard_pos  : integer := num_bits_rounded - 1;
    constant second_bit_discard_pos : integer := num_bits_rounded - 2;

    variable bits     : std_logic_vector(val'length - 1 downto 0) := std_logic_vector(val);
    variable half_add : unsigned(num_bits_discard - 1 downto 0)   := (others => '0');
    variable result   : unsigned(val'length - 1 downto num_bits_discard);
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
    -- * If any bits below the first discarded bit is set, are don't need to
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
    result := val(val'length - 1 downto final_bit_pos);
    -- Add the rounding bit
    result := result + half_add;
    return result;
  end function round;

-- =============================================================================

  pure function position(str : string; chars : string) return integer is
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

  pure function substring(str : string; substr : string) return boolean is
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

end package body fixed_pkg;
