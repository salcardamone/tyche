library tyche_tb_lib;
package axi_lite_tb_pkg_inst is
  new tyche_tb_lib.axi_lite_tb_pkg generic map(
    C_AXI_ADDR_WIDTH => 4, C_AXI_DATA_WIDTH => 32
  );
use work.axi_lite_tb_pkg_inst.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library tyche_lib;

library vunit_lib;
context vunit_lib.vunit_context;

entity axi_lite_multiply_tb is
  generic (runner_cfg : string);
end axi_lite_multiply_tb;

architecture testbench of axi_lite_multiply_tb is
  constant clk_period       : time     := 1 ns;
  constant C_AXI_ADDR_WIDTH : positive := 4;
  constant C_AXI_DATA_WIDTH : positive := 32;

  signal S_AXI_ACLK    : std_logic := '0';
  signal S_AXI_ARESETN : std_logic := '0';

  -- Write address AXI signals
  signal S_AXI_AWVALID : std_logic                                       := '0';
  signal S_AXI_AWREADY : std_logic;
  signal S_AXI_AWADDR  : std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0) := (others => '0');
  signal S_AXI_AWPROT  : std_logic_vector(2 downto 0)                    := (others => '0');

  -- Write data AXI signals
  signal S_AXI_WVALID : std_logic                                         := '0';
  signal S_AXI_WREADY : std_logic;
  signal S_AXI_WDATA  : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0)   := (others => '0');
  signal S_AXI_WSTRB  : std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0) := (others => '0');

  -- Write return AXI signals
  signal S_AXI_BVALID : std_logic;
  signal S_AXI_BREADY : std_logic := '0';
  signal S_AXI_BRESP  : std_logic_vector(1 downto 0);

  -- Read address AXI signals
  signal S_AXI_ARVALID : std_logic                                       := '0';
  signal S_AXI_ARREADY : std_logic;
  signal S_AXI_ARADDR  : std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0) := (others => '0');
  signal S_AXI_ARPROT  : std_logic_vector(2 downto 0)                    := (others => '0');

  -- Read data AXI signals
  signal S_AXI_RVALID : std_logic;
  signal S_AXI_RREADY : std_logic := '0';
  signal S_AXI_RDATA  : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);

begin

  clock : process is
  begin
    S_AXI_ACLK <= not S_AXI_ACLK;
    wait for clk_period / 2;
  end process;

  multiplier : entity tyche_lib.axi_lite_multiply(behavioural)
    generic map(
      C_AXI_ADDR_WIDTH => C_AXI_ADDR_WIDTH,
      C_AXI_DATA_WIDTH => C_AXI_DATA_WIDTH
      )
    port map(
      S_AXI_ACLK    => S_AXI_ACLK, S_AXI_ARESETN => S_AXI_ARESETN,
      S_AXI_AWVALID => S_AXI_AWVALID, S_AXI_AWREADY => S_AXI_AWREADY,
      S_AXI_AWADDR  => S_AXI_AWADDR, S_AXI_AWPROT => S_AXI_AWPROT,
      S_AXI_WVALID  => S_AXI_WVALID, S_AXI_WREADY => S_AXI_WREADY,
      S_AXI_WDATA   => S_AXI_WDATA, S_AXI_WSTRB => S_AXI_WSTRB,
      S_AXI_BVALID  => S_AXI_BVALID, S_AXI_BREADY => S_AXI_BREADY,
      S_AXI_BRESP   => S_AXI_BRESP,
      S_AXI_ARVALID => S_AXI_ARVALID, S_AXI_ARREADY => S_AXI_ARREADY,
      S_AXI_ARADDR  => S_AXI_ARADDR, S_AXI_ARPROT => S_AXI_ARPROT,
      S_AXI_RVALID  => S_AXI_RVALID, S_AXI_RREADY => S_AXI_RREADY,
      S_AXI_RDATA   => S_AXI_RDATA
      );

  -- Test both the AXI interface to the complex-complex multiplier, along with
  -- its ability to perform multiplications
  main : process is
  begin

    test_runner_setup(runner, runner_cfg);

    test_axil_write(
      clk_period, S_AXI_ARESETN,
      S_AXI_RREADY, S_AXI_ARVALID,
      S_AXI_WVALID, S_AXI_WREADY, S_AXI_WDATA, S_AXI_WSTRB,
      S_AXI_AWVALID, S_AXI_AWREADY, S_AXI_AWADDR,
      S_AXI_BREADY, S_AXI_BVALID);

    test_axil_read(
      clk_period, S_AXI_ARESETN,
      S_AXI_WVALID, S_AXI_WREADY, S_AXI_WDATA, S_AXI_WSTRB,
      S_AXI_AWVALID, S_AXI_AWREADY, S_AXI_AWADDR,
      S_AXI_BREADY, S_AXI_BVALID,
      S_AXI_ARVALID, S_AXI_ARREADY, S_AXI_ARADDR,
      S_AXI_RVALID, S_AXI_RREADY, S_AXI_RDATA);
    
    test_runner_cleanup(runner);
  end process;
end testbench;
