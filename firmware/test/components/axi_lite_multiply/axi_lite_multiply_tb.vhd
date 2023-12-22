library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

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

  multiplier : entity work.axi_lite_multiply(behavioural)
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

    -- @brief Reset the core and initialise all master signals to zero. At the
    -- end of this procedure, the core should be ready to work as expected
    -- without further delays.
    procedure reset_core is
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
    end procedure reset_core;

    -- @brief Perform an AXI-lite write to designated register with specified
    -- value. At the end of this procedure, the core should be ready to either
    -- read/ write without further delays.
    -- @param addr Address of register to write.
    -- @param val Value to write to register.
    procedure write_axil(
      variable addr : in std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
      variable val  : in std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0)) is
    begin
      S_AXI_AWVALID <= '1';
      S_AXI_WVALID  <= '1';
      S_AXI_AWADDR  <= addr;
      S_AXI_WDATA   <= val;
      S_AXI_WSTRB   <= X"F";
      wait for clk_period;
      S_AXI_AWVALID <= '0';
      S_AXI_WVALID  <= '0';
      check(S_AXI_AWREADY = '1');
      check(S_AXI_WREADY = '1');
      wait for clk_period;
      check(S_AXI_BVALID = '1');
      S_AXI_BREADY  <= '1';
      wait for clk_period;
      S_AXI_BREADY  <= '0';
      check(S_AXI_BVALID = '0');
    end procedure write_axil;

    type array_of_addr is array(1 downto 0) of std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
    type array_of_val is array(1 downto 0) of std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);

    procedure write_axil_continuous(
      variable addrs : in array_of_addr;
      variable vals  : in array_of_val) is
    begin
      S_AXI_WSTRB  <= X"F";
      S_AXI_BREADY <= '1';
      for idx in 0 to 1 loop
        S_AXI_AWVALID <= '1';
        S_AXI_WVALID  <= '1';
        S_AXI_AWADDR  <= addrs(idx);
        S_AXI_WDATA   <= vals(idx);
        wait for clk_period;
        S_AXI_AWVALID <= '0';
        S_AXI_WVALID  <= '0';
        check(S_AXI_AWREADY = '1');
        check(S_AXI_WREADY = '1');
        check(S_AXI_BVALID = '0');
        wait for clk_period;
        check(S_AXI_BVALID = '1');
      end loop;
    end procedure write_axil_continuous;

    -- @brief Perform an AXI-lite read from designated register and check with
    -- expected value at that register. At the end of this procedure, the core
    -- should be ready to either read/ write without further delays.
    -- @param addr Address of register to write.
    -- @param val Value expected to be read from register.
    procedure read_axil(
      variable addr : in std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);
      variable exp  : in std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0)) is
    begin
      S_AXI_ARVALID <= '1';
      S_AXI_ARADDR  <= addr;
      S_AXI_RREADY  <= '1';
      check(S_AXI_ARREADY = '1');
      check(S_AXI_RVALID = '0');
      wait for clk_period;
      S_AXI_ARVALID <= '0';
      wait for clk_period;
      check(S_AXI_RVALID = '1');
      check(S_AXI_RDATA = exp);
      wait for clk_period;
      -- Note that RVALID is deasserted the cycle after RVALID/RREADY handshake.
      -- This will then trigger the assertion of ARREADY on the next cycle
      check(S_AXI_RVALID = '0');
      wait for clk_period;
    end procedure read_axil;

    -- @brief Perform sequential reads and writes on core and make sure all
    -- signals are in expected states.
    procedure test_sequential is
      variable addr : std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0) := (others => '0');
      variable val  : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0) := (others => '0');
    begin
      reset_core;

      -- ====================================
      -- Try to write something to register 1
      -- ====================================
      addr := std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
      val  := std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH));
      write_axil(addr, val);

      -- =========================================
      -- Try to write something else to register 0
      -- =========================================
      addr := std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
      val  := std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH));
      write_axil(addr, val);

      -- ===========================================
      -- Read whatever has been stored in register 0
      -- ===========================================
      addr := std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
      val  := std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH));
      read_axil(addr, val);

      -- ===========================================
      -- Read whatever has been stored in register 0
      -- ===========================================
      addr := std_logic_vector(to_unsigned(4, C_AXI_ADDR_WIDTH));
      val  := std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH));
      read_axil(addr, val);

      -- ===========================================
      -- Read whatever has been stored in register 0
      -- ===========================================
      addr := std_logic_vector(to_unsigned(8, C_AXI_ADDR_WIDTH));
      val  := std_logic_vector(to_unsigned(489, C_AXI_DATA_WIDTH));
      read_axil(addr, val);

    end procedure test_sequential;

    procedure test_continuous is
      variable addrs : array_of_addr;
      variable vals  : array_of_val;
    begin
      reset_core;

      addrs(0) := std_logic_vector(to_unsigned(0, C_AXI_ADDR_WIDTH));
      addrs(1) := std_logic_vector(to_unsigned(1, C_AXI_ADDR_WIDTH));

      vals(0) := std_logic_vector(to_unsigned(69, C_AXI_DATA_WIDTH));
      vals(1) := std_logic_vector(to_unsigned(420, C_AXI_DATA_WIDTH));

      write_axil_continuous(addrs, vals);

    end procedure test_continuous;

  begin

    test_runner_setup(runner, runner_cfg);

    test_sequential;
    --test_continuous;

    test_runner_cleanup(runner);
  end process;
end testbench;
