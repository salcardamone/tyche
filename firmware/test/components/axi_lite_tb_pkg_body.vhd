library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library vunit_lib;
context vunit_lib.vunit_context;

package body axi_lite_tb_pkg is
  procedure reset(
    constant clk_period  : in  time;
    signal S_AXI_ARESETN : out std_logic;
    signal S_AXI_WVALID  : out std_logic;
    signal S_AXI_AWVALID : out std_logic;
    signal S_AXI_RREADY  : out std_logic;
    signal S_AXI_ARVALID : out std_logic;
    signal S_AXI_BREADY  : out std_logic) is
  begin
    S_AXI_ARESETN <= '0';
    S_AXI_WVALID  <= '0';
    S_AXI_AWVALID <= '0';
    S_AXI_RREADY  <= '0';
    S_AXI_ARVALID <= '0';
    S_AXI_BREADY  <= '0';
    wait for clk_period;
    S_AXI_ARESETN <= '1';
    wait for clk_period;
  end procedure reset;
  
  procedure axil_write(
    constant clk_period  : in  time;
    variable addr        : in  std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    variable val         : in  std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    signal S_AXI_WVALID  : out std_logic;
    signal S_AXI_WREADY  : in  std_logic;
    signal S_AXI_WDATA   : out std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    signal S_AXI_WSTRB   : out std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0);
    signal S_AXI_AWVALID : out std_logic;
    signal S_AXI_AWREADY : in  std_logic;
    signal S_AXI_AWADDR  : out std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    signal S_AXI_BREADY  : out std_logic;
    signal S_AXI_BVALID  : in  std_logic) is
  begin
    -- Write address and data
    S_AXI_AWVALID <= '1';
    S_AXI_WVALID  <= '1';
    S_AXI_AWADDR  <= addr;
    S_AXI_WDATA   <= val;
    S_AXI_WSTRB   <= (others => '1');
    S_AXI_BREADY  <= '1';
    wait for clk_period;
    -- Deassert write address and data; ensure slave has taken them.
    -- Assert BREADY to indicate we're ready to acknowledge a write success
    S_AXI_AWVALID <= '0';
    S_AXI_WVALID  <= '0';
    check(S_AXI_AWREADY = '1');
    check(S_AXI_WREADY = '1');
    wait for clk_period;
    -- Check slave has successfully written
    check(S_AXI_BVALID = '1');
    wait for clk_period;
    -- Deassert BREADY and ensure slave deasserted BVALID. Ready for next
    -- write
    check(S_AXI_BVALID = '0');
  end procedure axil_write;

  procedure test_axil_write(
    constant clk_period  : in  time;
    signal S_AXI_ARESETN : out std_logic;
    signal S_AXI_RREADY  : out std_logic;
    signal S_AXI_ARVALID : out std_logic;
    signal S_AXI_WVALID  : out std_logic;
    signal S_AXI_WREADY  : in  std_logic;
    signal S_AXI_WDATA   : out std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    signal S_AXI_WSTRB   : out std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0);
    signal S_AXI_AWVALID : out std_logic;
    signal S_AXI_AWREADY : in  std_logic;
    signal S_AXI_AWADDR  : out std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    signal S_AXI_BREADY  : out std_logic;
    signal S_AXI_BVALID  : in  std_logic) is
    constant delay            : time := 100 ps;
    constant remaining_period : time := clk_period - delay;
  begin
    reset(
      clk_period,
      S_AXI_ARESETN, S_AXI_WVALID, S_AXI_AWVALID,
      S_AXI_RREADY, S_AXI_ARVALID, S_AXI_BREADY
      );
    
    wait for clk_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_AWVALID <= '1';
    S_AXI_AWADDR  <= std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
    S_AXI_BREADY <= '1';

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_WVALID <= '1';
    S_AXI_WDATA  <= std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH));

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '0');

    wait for clk_period;

    check(S_AXI_AWREADY = '1');
    check(S_AXI_WREADY = '1');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_AWADDR <= std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
    S_AXI_WDATA <= std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH));

    wait for remaining_period;
    
    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for clk_period;

    check(S_AXI_AWREADY = '1');
    check(S_AXI_WREADY = '1');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_AWADDR <= std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
    S_AXI_WDATA <= std_logic_vector(to_unsigned(421, C_AXI_DATA_WIDTH));
    S_AXI_BREADY <= '0';

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for clk_period;
    
    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');
    
    wait for clk_period;
    
    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for delay;

    S_AXI_BREADY <= '1';

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for clk_period;
    
    check(S_AXI_AWREADY = '1');
    check(S_AXI_WREADY = '1');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_AWADDR <= std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
    S_AXI_WDATA <= std_logic_vector(to_unsigned(70, C_AXI_DATA_WIDTH));

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for clk_period;

    check(S_AXI_AWREADY = '1');
    check(S_AXI_WREADY = '1');
    check(S_AXI_BVALID = '0');

    wait for delay;

    S_AXI_AWVALID <= '0';
    S_AXI_WVALID <= '0';

    wait for remaining_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '1');

    wait for clk_period;

    check(S_AXI_AWREADY = '0');
    check(S_AXI_WREADY = '0');
    check(S_AXI_BVALID = '0');

  end procedure test_axil_write;

  procedure test_axil_read(
    constant clk_period  : in  time;
    signal S_AXI_ARESETN : out std_logic;
    signal S_AXI_WVALID  : out std_logic;
    signal S_AXI_WREADY  : in  std_logic;
    signal S_AXI_WDATA   : out std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    signal S_AXI_WSTRB   : out std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0);
    signal S_AXI_AWVALID : out std_logic;
    signal S_AXI_AWREADY : in  std_logic;
    signal S_AXI_AWADDR  : out std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    signal S_AXI_BREADY  : out std_logic;
    signal S_AXI_BVALID  : in  std_logic;
    signal S_AXI_ARVALID : out std_logic;
    signal S_AXI_ARREADY : in  std_logic;
    signal S_AXI_ARADDR  : out std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    signal S_AXI_RVALID  : in  std_logic;
    signal S_AXI_RREADY  : out std_logic;
    signal S_AXI_RDATA   : in  std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0))
  is
    variable addr             : std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    variable val              : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    constant delay            : time := 100 ps;
    constant remaining_period : time := clk_period - delay;
  begin
    reset(
      clk_period,
      S_AXI_ARESETN, S_AXI_WVALID, S_AXI_AWVALID,
      S_AXI_RREADY, S_AXI_ARVALID, S_AXI_BREADY);

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');
    wait for clk_period;

    addr := std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
    val  := std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH));
    axil_write(
      clk_period, addr, val,
      S_AXI_WVALID, S_AXI_WREADY, S_AXI_WDATA, S_AXI_WSTRB,
      S_AXI_AWVALID, S_AXI_AWREADY, S_AXI_AWADDR,
      S_AXI_BREADY, S_AXI_BVALID);

    addr := std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
    val  := std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH));
    axil_write(
      clk_period, addr, val,
      S_AXI_WVALID, S_AXI_WREADY, S_AXI_WDATA, S_AXI_WSTRB,
      S_AXI_AWVALID, S_AXI_AWREADY, S_AXI_AWADDR,
      S_AXI_BREADY, S_AXI_BVALID);
    wait for clk_period;

    S_AXI_ARVALID <= '0';
    S_AXI_RREADY  <= '0';

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

    wait for delay;

    S_AXI_ARADDR  <= std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
    S_AXI_ARVALID <= '1';
    S_AXI_RREADY  <= '1';

    wait for remaining_period;

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

    wait for delay;

    S_AXI_ARADDR <= std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));

    wait for remaining_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH)));

    wait for delay;
    wait for remaining_period;

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

    wait for delay;

    S_AXI_RREADY <= '0';
    S_AXI_ARADDR <= std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));

    wait for remaining_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH)));

    wait for clk_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH)));

    wait for clk_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH)));

    wait for delay;

    S_AXI_RREADY <= '1';

    wait for remaining_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH)));

    wait for clk_period;

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

    wait for delay;

    S_AXI_ARADDR <= std_logic_vector(to_unsigned(8, C_AXI_ADDR_WIDTH));

    wait for remaining_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH)));

    wait for clk_period;

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

    wait for delay;

    S_AXI_ARVALID <= '0';

    wait for remaining_period;

    check(S_AXI_ARREADY = '0');
    check(S_AXI_RVALID = '1');
    check(S_AXI_RDATA = std_logic_vector(to_unsigned(28980, C_AXI_DATA_WIDTH)));

    wait for delay;

    S_AXI_RREADY <= '0';

    wait for remaining_period;

    check(S_AXI_ARREADY = '1');
    check(S_AXI_RVALID = '0');

  end procedure test_axil_read;

end package body axi_lite_tb_pkg;
