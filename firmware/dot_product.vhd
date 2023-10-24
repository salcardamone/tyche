library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.vector_pkg.all;

entity dot_product is
  generic(
    vector_length : positive
    );
  port(
    a, b : in  vector_t(vector_length - 1 downto 0);
    dot  : out integer
    );
end entity dot_product;

architecture behavioural of dot_product is
begin
  dot_process : process(a, b) is
    variable square : integer := 0;
    variable sum    : integer := 0;
  begin
    for idx in vector_length - 1 downto 0
    loop
      square := a(idx) * b(idx);
      sum    := sum + square;
    end loop;
    dot <= sum;
  end process;
end architecture behavioural;
