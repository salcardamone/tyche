library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package vector_pkg is
  generic(
    data_width : positive := 8;
    acc_width  : positive := 32
    );
  constant max_val     : integer := 2 ** (data_width - 1) - 1;
  constant min_val     : integer := -2 ** (data_width - 1);
  constant mask        : integer := (2 ** data_width) - 1;
  constant max_acc_val : integer := 2 ** (acc_width - 1) - 1;
  constant min_acc_val : integer := -2 ** (acc_width - 1);

  subtype element_t is std_logic_vector(data_width - 1 downto 0);
  type vector_t is array (natural range <>) of element_t;

  -- @brief Compute the inner product between two vectors.
  --
  --        Will accumulate all products to a register of width acc_width before
  --        discarding the upper (acc_width - data_width) bits and returning
  --        the bottom data_width bits.
  -- @param a The first vector.
  -- @param b The second vector.
  -- @return The inner product between the two, a \cdot b.
  pure function inner_product(a : vector_t;
                              b : vector_t) return element_t;
end package vector_pkg;
