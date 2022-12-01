--
--  Pseudo Random Number Generator based on Mersenne Twister MT19937.
--
--  Author: Joris van Rantwijk <joris@jorisvr.nl>
--
--  This is a 32-bit random number generator in synthesizable VHDL.
--  The generator can produce 32 new random bits on every clock cycle.
--
--  See also M. Matsumoto, T. Nishimura, "Mersenne Twister:
--  a 623-dimensionally equidistributed uniform pseudorandom number generator",
--  ACM TOMACS, vol. 8, no. 1, 1998.
--
--  The generator requires a 32-bit seed value.
--  A default seed must be supplied at compile time and will be used
--  to initialize the generator at reset. The generator also supports
--  re-seeded at run time.
--
--  After reset, and after re-seeding, the generator needs 4 * 624 clock
--  cycles to initialize its internal state. During this time, the generator
--  is unable to provide correct output.
--
--  NOTE: This is not a cryptographic random number generator.
--

--
--  Copyright (C) 2016 Joris van Rantwijk
--
--  This code is free software; you can redistribute it and/or
--  modify it under the terms of the GNU Lesser General Public
--  License as published by the Free Software Foundation; either
--  version 2.1 of the License, or (at your option) any later version.
--
--  See <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity rng_mt19937 is

    generic (
        -- Default seed value.
        init_seed:  std_logic_vector(31 downto 0);

        -- Set to TRUE to force implementation of the constant multiplier
        -- as a fixed adder tree; set to FALSE to allow the synthesizer
        -- to choose an implementation.
        force_const_mul: boolean );

    port (

        -- Clock, rising edge active.
        clk:        in  std_logic;

        -- Synchronous reset, active high.
        rst:        in  std_logic;

        -- High to re-seed the generator (works regardless of enable signal).
        reseed:     in  std_logic;

        -- New seed value (must be valid when reseed = '1').
        newseed:    in  std_logic_vector(31 downto 0);

        -- High when the user accepts the current random data word
        -- and requests new random data for the next clock cycle.
        out_ready:  in  std_logic;

        -- High when valid random data is available on the output.
        -- This signal is low during the first 4*624 clock cycles after
        -- reset and after re-seeding, and high in all other cases.
        out_valid:  out std_logic;

        -- Random output data (valid when out_valid = '1').
        -- A new random word appears after every rising clock edge
        -- where out_ready = '1'.
        out_data:   out std_logic_vector(31 downto 0) );

end entity;


architecture rng_mt19937_arch of rng_mt19937 is

    -- Constants.
    constant const_a: std_logic_vector(31 downto 0) := x"9908b0df";
    constant const_b: std_logic_vector(31 downto 0) := x"9d2c5680";
    constant const_c: std_logic_vector(31 downto 0) := x"efc60000";
    constant const_f: natural                       := 1812433253;
    constant addr_offset: natural                   := 396;

    -- Block RAM for generator state.
    type mem_t is array(0 to 620) of std_logic_vector(31 downto 0);
    signal mem: mem_t;

    -- RAM access registers.
    signal reg_a_addr:  std_logic_vector(9 downto 0) := (others => '0');
    signal reg_b_addr:  std_logic_vector(9 downto 0) := std_logic_vector(
        to_unsigned(addr_offset, 10));
    signal reg_a_wdata: std_logic_vector(31 downto 0);
    signal reg_a_rdata: std_logic_vector(31 downto 0);
    signal reg_b_rdata: std_logic_vector(31 downto 0);

    -- Internal registers.
    signal reg_enable:      std_logic                       := '1';
    signal reg_reseeding:   std_logic                       := '1';
    signal reg_reseedstate: std_logic_vector(3 downto 0)    := "0001";
    signal reg_validwait:   std_logic;
    signal reg_a_rdata_p:   std_logic_vector(31 downto 0);
    signal reg_reseed_cnt:  std_logic_vector(9 downto 0)    := (others => '0');
    signal reg_output_buf:  std_logic_vector(31 downto 0);
    signal reg_seed_a:      std_logic_vector(31 downto 0);
    signal reg_seed_b:      std_logic_vector(31 downto 0);
    signal reg_seed_b2:     std_logic_vector(31 downto 0);
    signal reg_seed_c:      std_logic_vector(31 downto 0);
    signal reg_seed_c2:     std_logic_vector(31 downto 0);
    signal reg_seed_d:      std_logic_vector(31 downto 0)   := init_seed;

    -- Output register.
    signal reg_valid:       std_logic                       := '0';
    signal reg_output:      std_logic_vector(31 downto 0)   := (others => '0');

    -- Multiply unsigned number with constant and discard overflowing bits.
    function mulconst(x: unsigned)
        return unsigned
    is
        variable t: unsigned(2*x'length-1 downto 0);
    begin
        t := x * const_f;
        return t(x'length-1 downto 0);
    end function;

begin

    --
    -- Drive output signal.
    --
    out_valid   <= reg_valid;
    out_data    <= reg_output;

    --
    -- Main synchronous process.
    --
    process (clk) is
        variable y: std_logic_vector(31 downto 0);
    begin
        if rising_edge(clk) then

            -- Update memory pointers.
            if reg_enable = '1' then

                if unsigned(reg_a_addr) = 620 then
                    reg_a_addr <= (others => '0');
                else
                    reg_a_addr <= std_logic_vector(unsigned(reg_a_addr) + 1);
                end if;

                if unsigned(reg_b_addr) = 620 then
                    reg_b_addr <= (others => '0');
                else
                    reg_b_addr <= std_logic_vector(unsigned(reg_b_addr) + 1);
                end if;

            end if;

            -- Keep previous value from read port A.
            if reg_enable = '1' then
                reg_a_rdata_p   <= reg_a_rdata;
            end if;

            -- Update reseeding state (4 cycles per address step).
            reg_reseedstate(3 downto 1) <= reg_reseedstate(2 downto 0);
            reg_reseedstate(0) <= reg_reseedstate(3) and reg_reseeding;

            -- Update reseeding counter.
            if reg_enable = '1' then
                reg_reseed_cnt  <=
                    std_logic_vector(unsigned(reg_reseed_cnt) + 1);
            end if;

            -- Determine end of reseeding.
            if unsigned(reg_reseed_cnt) = 624 then
                reg_reseeding   <= '0';
            end if;

            -- Enable state machine on next cycle
            --  a) every 1st out of 4 cycles during reseeding, and
            --  b) on-demand for new output.
            reg_enable  <= (reg_reseeding and reg_reseedstate(3)) or
                           (not reg_reseeding and
                            (out_ready or not reg_valid));

            -- Reseed state 1: XOR and shift previous state element.
            y := reg_seed_d;
            y(1 downto 0) := y(1 downto 0) xor y(31 downto 30);
            reg_seed_a <= y;

            -- Reseed state 2: Multiply by constant.
            if force_const_mul then
                -- Compute 37 * Mprev.
                reg_seed_b  <= std_logic_vector(
                      unsigned(reg_seed_a)
                    + shift_left(unsigned(reg_seed_a), 2)
                    + shift_left(unsigned(reg_seed_a), 5));
                -- Compute (2**19 - 2**15) * Mprev.
                reg_seed_b2 <= std_logic_vector(
                      shift_left(unsigned(reg_seed_a), 19)
                    - shift_left(unsigned(reg_seed_a), 15));
            else
                -- Compute 1812433253 * Mprev.
                -- Let synthesizer choose a multiplier implementation.
                reg_seed_b  <= std_logic_vector(
                    mulconst(unsigned(reg_seed_a)));
            end if;

            -- Reseed state 3: Continue multiplication by constant.
            if force_const_mul then
                -- Compute (37 + 2**6 * 37 + 2**19 - 2**15) * Mprev.
                -- Finalize multiplication by 1812433253 =
                -- (37 + 2**6*37 - 2**15 + 2**19 - 2**26*37)
                reg_seed_c  <= std_logic_vector(
                      unsigned(reg_seed_b)
                    + shift_left(unsigned(reg_seed_b), 6)
                    + unsigned(reg_seed_b2));
                -- Compute (2**32 - 2**26 * 37) * Mprev + reseed_cnt.
                reg_seed_c2 <= std_logic_vector(
                      unsigned(reg_reseed_cnt)
                    - shift_left(unsigned(reg_seed_b), 26));
            else
                reg_seed_c  <= reg_seed_b;
            end if;

            -- Reseed state 4: Prepare next element of initial state.
            if reg_reseeding = '1' then
                if force_const_mul then
                    -- Compute   (37 + 2**6 * 37 + 2**19 - 2**15) * Mprev
                    --         + (2**32 - 2**26 * 37) * Mprev + reseed_cnt
                    --         = 1812433253 * Mprev + reseed_cnt.
                    reg_seed_d  <= std_logic_vector(unsigned(reg_seed_c) +
                                                    unsigned(reg_seed_c2));
                else
                    -- Compute 1812433253 * Mprev + reseed_cnt.
                    reg_seed_d  <= std_logic_vector(unsigned(reg_seed_c) +
                                                    unsigned(reg_reseed_cnt));
                end if;
            end if;

            -- Update internal RNG state.
            if reg_enable = '1' then

                if reg_reseeding = '1' then

                    -- Reseed state 1: Write next state element.
                    reg_a_wdata <= reg_seed_d;

                else

                    -- Normal operation.
                    -- Perform one step of the "twist" function.

                    y := reg_a_rdata_p(31 downto 31) &
                         reg_a_rdata(30 downto 0);

                    if y(0) = '1' then
                        y := "0" & y(31 downto 1);
                        y := y xor const_a;
                    else
                        y := "0" & y(31 downto 1);
                    end if;

                    reg_a_wdata <= reg_b_rdata xor y;

                end if;
            end if;

            -- Prepare output value.
            if reg_enable = '1' then

                y := reg_a_wdata;

                y(20 downto 0)  := y(20 downto 0) xor y(31 downto 11);
                y(31 downto 7)  := y(31 downto 7) xor
                                   (y(24 downto 0) and const_b(31 downto 7));
                y(31 downto 15) := y(31 downto 15) xor
                                   (y(16 downto 0) and const_c(31 downto 15));
                y(13 downto 0)  := y(13 downto 0) xor y(31 downto 18);

                reg_output_buf  <= y;

                -- Conditionally push to final output register.
                if out_ready = '1' or reg_valid = '0' then
                    reg_output      <= y;
                end if;

            end if;

            -- Use buffered value when restarting after pause.
            if out_ready = '1' and reg_enable = '0' then
                reg_output  <= reg_output_buf;
            end if;

            -- Indicate valid data at end of initialization.
            if reg_enable = '1' then
                reg_validwait   <= not reg_reseeding;
                reg_valid       <= reg_validwait and not reg_reseeding;
            end if;

            -- Start re-seeding.
            if reseed = '1' then
                reg_reseeding   <= '1';
                reg_reseedstate <= "0001";
                reg_reseed_cnt  <= std_logic_vector(to_unsigned(0, 10));
                reg_enable      <= '1';
                reg_seed_d      <= newseed;
                reg_valid       <= '0';
            end if;

            -- Synchronous reset.
            if rst = '1' then
                reg_a_addr      <= std_logic_vector(to_unsigned(0, 10));
                reg_b_addr      <= std_logic_vector(
                                     to_unsigned(addr_offset, 10));
                reg_reseeding   <= '1';
                reg_reseedstate <= "0001";
                reg_reseed_cnt  <= std_logic_vector(to_unsigned(0, 10));
                reg_enable      <= '1';
                reg_seed_d      <= init_seed;
                reg_valid       <= '0';
                reg_output      <= (others => '0');
            end if;

        end if;
    end process;

    --
    -- Synchronous process for block RAM.
    --
    process (clk) is
    begin
        if rising_edge(clk) then
            if reg_enable = '1' then

                -- Read from port A.
                reg_a_rdata <= mem(to_integer(unsigned(reg_a_addr)));

                -- Read from port B.
                reg_b_rdata <= mem(to_integer(unsigned(reg_b_addr)));

                -- Write to port A.
                mem(to_integer(unsigned(reg_a_addr))) <= reg_a_wdata;

            end if;
        end if;
    end process;

end architecture;






-- -----------------------------------------------------------------------------------------------------------------------

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY MT19937_TB IS
  PORT(
    Clk : IN STD_LOGIC;
    DataOut : OUT STD_LOGIC_VECTOR( 31 DOWNTO 0 )
  );
END MT19937_TB;


ARCHITECTURE rtl OF MT19937_TB IS
BEGIN

    rng_mt19937_inst : entity work.rng_mt19937
    generic map(
        init_seed => 32x"01234578",
        force_const_mul => True
    )
    port map(
        clk => Clk,
        rst => '0',
        reseed => '0',
        newseed => (OTHERS=>'0'),
        out_ready => '1',
        out_valid => OPEN,
        out_data => DataOut
    );

END rtl;

