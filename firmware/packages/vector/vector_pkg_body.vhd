library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package body vector_pkg is

-- =============================================================================

  pure function inner_product(a : vector_t;
                              b : vector_t) return element_t is
    variable dot : integer range min_val to max_val         := 0;
    variable acc : integer range min_acc_val to max_acc_val := 0;
  begin
    assert a'length = b'length
      report "Argument vectors must be of same length." severity failure;
    for idx in a'length - 1 downto 0
    loop
      acc := acc + to_integer(signed(a(idx))) * to_integer(signed(b(idx)));
    end loop;
    return element_t(to_unsigned(acc, data_width));
  end function inner_product;

-- =============================================================================

end package body vector_pkg;
