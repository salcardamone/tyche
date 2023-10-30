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

  -- @brief Find the position in a string where one of the characters appears.
  -- @param str The string to search for the character.
  -- @param chars A list of characters; will return the position of the first
  --              "hit" that's found.
  -- @return The position in the input string where the character was found.
  --         Will return -1 if no hits were found.
  pure function position(str : string; chars : string) return integer;

  -- @brief Determine whether a substring is contained within a string.
  -- @param str The string to search for the substring.
  -- @param substr The substring to search for within the string.
  -- @return True if the substring was found, else false.
  pure function substring(str : string; substr : string) return boolean;

  -- @brief Parse a stringified Q-format into a fixed_t record.
  -- @param fmt The stringified Q-format. Note that this requires either a "Q"
  --            or "UQ" prefix, all in uppercase, along with a "." separating
  --            the integer and fractional bits.
  -- @return The fixed_t record of the Q-format.
  pure function parse_format(fmt : string) return fixed_t;

  -- @brief Compute the total number of bits contained within a fixed_t record.
  -- @param fmt The stringified Q-format.
  -- @return The total number of bits in the Q-format.
  pure function num_bits(fmt : fixed_t) return integer;

  -- @brief Retrieve the number of integer bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The number of integer bits in the Q-format.
  pure function num_int_bits(fmt : string) return integer;

  -- @brief Retrieve the number of fractional bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The number of fractional bits in the Q-format.
  pure function num_frac_bits(fmt : string) return integer;

  -- @brief Retrieve the total number of bits in a stringified Q-format.
  -- @param fmt The stringified Q-format.
  -- @return The total number of bits in the Q-format.
  pure function num_bits(fmt : string) return integer;

  -- @brief Perform convergent rounding of an integer when discarding lower
  --        order bits.
  -- @param val The value we're rounding.
  -- @param num_bits_rounded The number of bits we should be left with after
  --        rounding.
  -- @return The rounded value.
  pure function round(val                       : unsigned;
                      constant num_bits_rounded : integer range 0 to 63)
    return unsigned;

end package fixed_pkg;
