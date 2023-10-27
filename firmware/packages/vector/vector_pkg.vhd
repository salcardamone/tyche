library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package vector_pkg is
  generic(
    data_width : positive := 8
    );
  subtype element_type is std_logic_vector(data_width - 1 downto 0);
  constant max_val : integer := 2 ** (data_width - 1) - 1;
  constant min_val : integer := -2 ** (data_width - 1);

  type vector_t is array (natural range <>) of element_type;

  pure function distance(a : vector_t;
                         b : vector_t) return element_type;
  pure function inner_product(a : vector_t;
                              b : vector_t) return element_type;
end package vector_pkg;
