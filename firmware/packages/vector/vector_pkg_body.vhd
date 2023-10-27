library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package body vector_pkg is

-- =============================================================================

  pure function distance(a : vector_t;
                         b : vector_t) return element_type is
    variable dist : integer range min_val to max_val := 0;
  begin
    assert a'length = b'length
      report "Argument vectors must be of same length." severity failure;
    for idx in a'length - 1 downto 0
    loop
      dist := dist + to_integer(signed(a(idx))) - to_integer(signed(b(idx)));
    end loop;
    return element_type(to_unsigned(dist, data_width));
  end function distance;

-- =============================================================================

  pure function inner_product(a : vector_t;
                              b : vector_t) return element_type is
    variable dot : integer range min_val to max_val := 0;
  begin
    assert a'length = b'length
      report "Argument vectors must be of same length." severity failure;
    for idx in a'length - 1 downto 0
    loop
      dot := dot + to_integer(signed(a(idx))) * to_integer(signed(b(idx)));
    end loop;
    return element_type(to_unsigned(dot, data_width));
  end function inner_product;

-- =============================================================================

end package body vector_pkg;
