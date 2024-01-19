library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- @brief Package for the testbenching of blocks that have AXI-Lite interfaces.
--        Procedures to verify that the handshaking works as expected.
-- @param C_AXI_ADDR_WIDTH Address bus width in bits.
-- @param C_AXI_DATA_WIDTH Data bus width in bits.
package axi_lite_tb_pkg is
  generic(
    C_AXI_ADDR_WIDTH : positive;
    C_AXI_DATA_WIDTH : positive
    );

  -- @brief Reset the AXI slave; upon exit, the slave should be in a state to
  --        receive data signals from the master without further delay.
  --
  --                                                             Signal Driver
  -- @param clk_period    Clock period.
  -- @param S_AXI_ARESETN Active-low reset                       (master)
  -- @param S_AXI_WVALID  Write data valid                       (master)
  -- @param S_AXI_AWVALID Address-write valid                    (master)
  -- @param S_AXI_RREADY  Read data ready                        (master)
  -- @param S_AXI_ARVALID Address-read valid                     (master)
  -- @param S_AXI_BREADY  Write acknowledge ready                (master)
  procedure reset(
    constant clk_period  : in  time;
    signal S_AXI_ARESETN : out std_logic;
    signal S_AXI_WVALID  : out std_logic;
    signal S_AXI_AWVALID : out std_logic;
    signal S_AXI_RREADY  : out std_logic;
    signal S_AXI_ARVALID : out std_logic;
    signal S_AXI_BREADY  : out std_logic);

  -- @brief Write data to an AXI slave. Will also check the VALID/READY
  --        handshakes proceed as expected.
  --
  --                                                             Signal Driver
  -- @param clk_period    Clock period.
  -- @param addr          Write address.
  -- @param val           Write data.
  -- @param S_AXI_WVALID  Write data valid                       (master)
  -- @param S_AXI_WREADY  Write data ready                       (slave)
  -- @param S_AXI_WDATA   Write data                             (master)
  -- @param S_AXI_WSTRB   Write strobe                           (master)
  -- @param S_AXI_AWVALID Address-write valid                    (master)
  -- @param S_AXI_AWREADY Address-write ready                    (slave)
  -- @param S_AXI_AWADDR  Address                                (master)
  -- @param S_AXI_BREADY  Write acknowledge ready                (master)
  -- @param S_AXI_BVALID  Write acknowledge valid                (slave)
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
    signal S_AXI_BVALID  : in  std_logic);

  -- @brief Verify the AXI-lite handshaking for writes conforms with the
  --        waveforms given in Figure 5 of
  --        https://zipcpu.com/blog/2020/03/08/easyaxil.html
  --
  --        Four transactions will take place:
  --        (1) AWVALID asserted 1 cycle before WVALID
  --        (2) AWVALID and WVALID asserted on the same cycle, BREADY
  --            deasserted forcing the slave to wait and hold BVALID.
  --        (3) BREADY reasserted completing previous transaction and a new
  --            transaction completes without delay.
  --        (4) Regular transaction.
  --
  --                                                             Signal Driver
  -- @param clk_period    Clock period.
  -- @param S_AXI_ARESETN Active-low reset                       (master)
  -- @param S_AXI_RREADY  Read data ready -- needed for reset    (master)
  -- @param S_AXI_ARVALID Address-read valid -- needed for reset (master)
  -- @param S_AXI_WVALID  Write data valid                       (master)
  -- @param S_AXI_WREADY  Write data ready                       (slave)
  -- @param S_AXI_WDATA   Write data                             (master)
  -- @param S_AXI_WSTRB   Write strobe                           (master)
  -- @param S_AXI_AWVALID Address-write valid                    (master)
  -- @param S_AXI_AWREADY Address-write ready                    (slave)
  -- @param S_AXI_AWADDR  Address                                (master)
  -- @param S_AXI_BREADY  Write acknowledge ready                (master)
  -- @param S_AXI_BVALID  Write acknowledge valid                (slave)
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
    signal S_AXI_BVALID  : in  std_logic);

  -- @brief Verify the AXI-lite handshaking for reads conforms with the
  --        waveforms given in Figure 6 of
  --        https://zipcpu.com/blog/2020/03/08/easyaxil.html
  --
  --        First of all we'll write values to two registers, the continue with
  --        the reading (we'll have RDATA to verify then).
  --
  --        Four transactions will take place:
  --        (1) ARVALID and RREADY asserted on the same cycle.
  --        (2) ARVALID held while RREADY is deasserted, forcing the slave to
  --            wait and hold RVALID. Next ARADDR is loaded.
  --        (3) RREADY re-asserted, allowing progress of next read.
  --        (4) ARVALID and RREADY held high, normal transaction.
  --
  --                                                             Signal Driver
  -- @param clk_period    Clock period.
  -- @param S_AXI_ARESETN Active-low reset                       (master)
  -- @param S_AXI_WVALID  Write data valid                       (master)
  -- @param S_AXI_WREADY  Write data ready                       (slave)
  -- @param S_AXI_WDATA   Write data                             (master)
  -- @param S_AXI_WSTRB   Write strobe                           (master)
  -- @param S_AXI_AWVALID Address-write valid                    (master)
  -- @param S_AXI_AWREADY Address-write ready                    (slave)
  -- @param S_AXI_AWADDR  Address                                (master)
  -- @param S_AXI_BREADY  Write acknowledge ready                (master)
  -- @param S_AXI_BVALID  Write acknowledge valid                (slave)
  -- @param S_AXI_ARVALID Address-read valid                     (master)
  -- @param S_AXI_ARREADY Address-read ready                     (slave)
  -- @param S_AXI_ARADDR  Address data for read                  (master)
  -- @param S_AXI_RVALID  Read data valid                        (slave)
  -- @param S_AXI_RREADY  Read data ready                        (master)
  -- @param S_AXI_RDATA   Read data                              (slave)
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
    signal S_AXI_RDATA   : in  std_logic_vector(C_AXI_DATA_WIDTH - 1 downto 0));
end package axi_lite_tb_pkg;
