library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- @brief Fixed-point arithmetic utilities.
--
--        Note that we'll use the TI convention when stringifying the
--        Q-format. So:
--
--        * QM.N is a signed value with M integer bits and N fractional bits,
--          with and implied sign bit, making M + N + 1 bits in total.
--        * UQM.N is an unsigned value with M integer bits and N fractional
--          bits, making M + N bits in total.
package fixed_pkg is

  -- @brief Container to hold all the information we need for a Q-format.
  type fixed_t is record
    is_signed     : boolean;
    num_int_bits  : integer range 0 to 32;
    num_frac_bits : integer range 0 to 32;
  end record fixed_t;

  -- @brief Parse a stringified Q-format into a fixed_t record.
  -- @param fmt The stringified Q-format. Note that this requires either a "Q"
  --            or "UQ" prefix, all in uppercase, along with a "." separating
  --            the integer and fractional bits.
  -- @return The fixed_t record of the Q-format.
  pure function parse_format(constant fmt : string) return fixed_t;

  -- @brief Equality for fixed_t formats.
  -- @param lhs The LHS format.
  -- @param rhs The RHS format.
  -- @return True if equal, false otherwise.
  pure function "="(constant lhs : fixed_t; constant rhs : fixed_t)
    return boolean;

  -- @brief Convert a fixed_t into its stringified Q-format.
  -- @param fmt The Q-format.
  -- @return The stringified Q-format.
  pure function to_string(constant fmt : fixed_t) return string;

  -- @brief Compute the total number of bits contained within a fixed_t record.
  -- @param fmt The stringified Q-format.
  -- @return The total number of bits in the Q-format.
  pure function num_bits(constant fmt : fixed_t) return integer;

  -- @brief Retrieve the number of integer bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The number of integer bits in the Q-format.
  pure function num_int_bits(constant fmt : string) return integer;

  -- @brief Retrieve the number of fractional bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The number of fractional bits in the Q-format.
  pure function num_frac_bits(constant fmt : string) return integer;

  -- @brief Retrieve the total number of bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The total number of bits in the Q-format.
  pure function num_bits(constant fmt : string) return integer;

  -- @brief Return the minimum value that can be represented by the given Q-format.
  -- @param fmt The Q-format.
  -- @return The minimum value representable by the Q-format. Takes into
  --         account signedness of the format.
  pure function min_val(constant fmt : fixed_t) return std_logic_vector;

  -- @brief Return the maximum value that can be represented by the given Q-format.
  -- @param fmt The Q-format.
  -- @return The maximum value representable by the Q-format. Takes into
  --         account signedness of the format.
  pure function max_val(constant fmt : fixed_t) return std_logic_vector;

  -- @brief Determine the format of a number that results from the multiplication
  --        of two other numbers.
  -- @param fmt_a First argument format.
  -- @param fmt_b Second argument format.
  -- @return The format of the product.
  pure function multiply_format(constant fmt_a : fixed_t; constant fmt_b : fixed_t)
    return fixed_t;

  -- @brief Perform convergent rounding of an integer when discarding lower
  --        order bits.
  -- @param val The value we're rounding.
  -- @param num_bits_rounded The number of bits we should be left with after
  --        rounding.
  -- @return The rounded value.
  pure function round(constant val              : unsigned;
                      constant num_bits_rounded : integer range 0 to 63)
    return unsigned;

  -- @brief Cast a value from some original Q-format to some new Q-format.
  --
  --        This proceeds by first truncating the fractional part and
  --        convergently rounding the truncated bits, and second either
  --        truncating the upper bits or extending them.
  --
  --        This should only be used to cast from a larger format (both integer
  --        and fractional parts) to a smaller format. Rounding will fail if we
  --        request more fractional bits than were started with, and no
  --        sign-extension is performed on the integer part, hence all hell
  --        could break loose if we have a negative value. This is only
  --        really intended for casting from some intermediate arithmetic
  --        result to a desired precision.
  -- @param val The value to cast.
  -- @param original_fmt The Q-format of the value on entry.
  -- @param new_fmt The Q-format of the result value.
  -- @return The original value in the new Q-format.
  function cast(constant val     : std_logic_vector;
                constant old_fmt : fixed_t; constant new_fmt : fixed_t)
    return std_logic_vector;

end package fixed_pkg;
