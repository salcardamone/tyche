library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package new_vector_pkg is new work.vector_pkg generic map (data_width => 16);
use work.new_vector_pkg;
                          
entity dot_product is
  generic(
    vector_length : positive
    );
  port(
    a, b : in  new_vector_pkg.vector_t(vector_length - 1 downto 0);
    dot  : out new_vector_pkg.element_type
    );
end entity dot_product;

architecture behavioural of dot_product is
begin
  dot_process : process(a, b) is
  begin
    dot <= new_vector_pkg.inner_product(a, b);
  end process;
end architecture behavioural;
