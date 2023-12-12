library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity axi_lite_multiply is
  generic(
    C_AXI_ADDR_WIDTH : positive := 4;
    C_AXI_DATA_WIDTH : positive := 32
    );
  port(
    S_AXI_ACLK    : in std_logic;
    S_AXI_ARESETN : in std_logic;

    -- Write address AXI signals
    S_AXI_AWVALID : in  std_logic;
    S_AXI_AWREADY : out std_logic := '0';
    S_AXI_AWADDR  : in  std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);

    -- Write data AXI signals
    S_AXI_WVALID : in  std_logic;
    S_AXI_WREADY : out std_logic := '0';
    S_AXI_WDATA  : in  std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    S_AXI_WSTRB  : in  std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0);

    -- Write return AXI signals
    S_AXI_BVALID : out std_logic := '0';
    S_AXI_BREADY : in  std_logic;

    -- Read address AXI signals
    S_AXI_ARVALID : in  std_logic;
    S_AXI_ARREADY : out std_logic := '0';
    S_AXI_ARADDR  : in  std_logic_vector(C_AXI_ADDR_WIDTH - 1 downto 0);

    -- Read data AXI signals
    S_AXI_RVALID : out std_logic                                       := '0';
    S_AXI_RREADY : in  std_logic;
    S_AXI_RDATA  : out std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0) := (others => '0')
    );
end entity axi_lite_multiply;

-- For a write, the sequence of signals should be as follows:
-- (1) Master asserts AWVALID, WVALID, WADDR and WDATA
-- (2) Slave deasserts AWREADY and WREADY. Strobes data on WDATA.
-- (3) Slave asserts BVALID. If master has asserted BREADY, handshake succeeds
architecture behavioural of axi_lite_multiply is
  signal arg_a, arg_b : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0) := (others => '0');
  signal result       : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0) := (others => '0');

  signal axil_awready                 : std_logic := '0';
  signal axil_arready                 : std_logic := '0';
  signal axil_bvalid                  : std_logic := '0';
  signal axil_read_enable             : std_logic := '0';
  signal axil_rvalid                  : std_logic := '0';
  signal axil_rdata                   : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
  signal strobed_arg_a, strobed_arg_b : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);

  function apply_strobe(
    constant reg   : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
    constant wstrb : std_logic_vector(C_AXI_DATA_WIDTH/8 - 1 downto 0);
    constant data  : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0))
    return std_logic_vector is
    variable msb_pos, lsb_pos : integer;
    variable result           : std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0);
  begin
    for ibyte in 0 to C_AXI_DATA_WIDTH/8 - 1 loop
      msb_pos := 8 * (ibyte + 1) - 1;
      lsb_pos := 8 * ibyte;
      if wstrb(ibyte) = '1' then
        result(msb_pos downto lsb_pos) := data(msb_pos downto lsb_pos);
      else
        result(msb_pos downto lsb_pos) := reg(msb_pos downto lsb_pos);
      end if;
    end loop;
    return result;
  end function apply_strobe;
begin

  -- We're ready to receive the next read address from the master if we're not
  -- currently holding valid read data we're waiting for the master to accept
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if (S_AXI_ARESETN = '0') then
        axil_arready <= '0';
      else
        axil_arready <= (not S_AXI_RVALID);
      end if;
    end if;
  end process;

  S_AXI_ARREADY    <= axil_arready;
  -- We can do a read if we've got a valid address and we're ready to read it
  axil_read_enable <= (S_AXI_ARVALID and S_AXI_ARREADY);

  -- We have valid read data when we've taken an address from the read address
  -- bus. We have to deassert RVALID if we're servicing another read at the
  -- moment, i.e. RREADY is asserted since we don't want to overwrite while the
  -- master is trying to take this data
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if (S_AXI_ARESETN = '0') then
        axil_rvalid <= '0';
      elsif (axil_read_enable) then
        axil_rvalid <= '1';
      elsif (S_AXI_RREADY) then
        axil_rvalid <= '0';
      end if;
    end if;
  end process;

  S_AXI_RVALID <= axil_rvalid;

  -- Retrieve contents of a register if we don't have valid data on the read
  -- bus without the master having asserted ready.
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if ((not S_AXI_RVALID) or S_AXI_RREADY) then
        case S_AXI_ARADDR is
          when "00" =>
            axil_rdata <= arg_a;
          when "01" =>
            axil_rdata <= arg_b;
          when "10" =>
            axil_rdata <= result;
          when others =>
            null;
        end case;
      end if;
    end if;
  end process;

  S_AXI_RDATA <= axil_rdata;

  -- Report on whether we managed to write successfully or not. Signal needs to
  -- be registered until master has acknowledged with ready
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if (S_AXI_ARESETN = '0') then
        axil_bvalid <= '0';
      else
        -- If an AWREADY/AWVALID handshake took place on the last cycle, then
        -- we can signal to the master that write was successful...
        if (axil_awready = '1') then
          axil_bvalid <= '1';
        -- ... otherwise we have to check whether the master has responded to
        -- the previous BVALID with a BREADY, allowing us to can deassert BVALID
        elsif (S_AXI_BREADY = '1') then
          axil_bvalid <= '0';
        end if;
      end if;
    end if;
  end process;

  S_AXI_BVALID <= axil_bvalid;

  -- Determine whether we're ready for address and data from the master
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if (S_AXI_ARESETN = '0') then
        axil_awready <= '0';
      else
        axil_awready <=
          -- (1) If the slave raised AWREADY on the last cycle, it will have to
          --     await the BVALID/BREADY handshake before proceeding
          (not axil_awready) and
          -- (2) If master has valid write address and data, then we can
          --     retrieve it
          (S_AXI_AWVALID and S_AXI_WVALID) and
          -- (3) If the slave has raised BVALID and BREADY hasn't yet been
          --     raised by the master, then we can't accept any more write
          --     requests
          ((not axil_bvalid) or S_AXI_BREADY);
      end if;
    end if;
  end process;

  S_AXI_AWREADY <= axil_awready;
  S_AXI_WREADY  <= axil_awready;

  -- Apply strobe for each register pre-emptively
  strobed_arg_a <= apply_strobe(arg_a, S_AXI_WSTRB, S_AXI_WDATA);
  strobed_arg_b <= apply_strobe(arg_b, S_AXI_WSTRB, S_AXI_WDATA);

  -- Handle setting of registers. Can be zero'd when reset is low, or can be
  -- set to whatever is on the write data bus if we're ready to receive that
  -- data from the master
  process(S_AXI_ACLK) is
  begin
    if rising_edge(S_AXI_ACLK) then
      if (S_AXI_ARESETN = '0') then
        arg_a  <= (others => '0');
        arg_b  <= (others => '0');
        result <= (others => '0');
      else
        -- If we performed an AWREADY/AWVALID handshake on the previous cycle,
        -- then we can register the write data
        if (axil_awready = '1') then
          case S_AXI_AWADDR is
            when "00" =>
              arg_a <= strobed_arg_a;
            when "01" =>
              arg_b <= strobed_arg_b;
            when others =>
              null;
          end case;
        end if;
        result <= std_logic_vector(unsigned(arg_a) + unsigned(arg_b));
      end if;
    end if;
  end process;
  
end architecture behavioural;

