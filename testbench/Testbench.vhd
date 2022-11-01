-- ------------------------------------------------------------------------
entity MixMaxFli is
port(
    clk : in integer range 0 to 1 := 0;
    hi , lo : out natural := 0
);
end;

architecture fli of MixMaxFli is
    attribute foreign : string;
    attribute foreign of fli : architecture is "MixMaxFli ../testbench/mixmaxfli.so";
begin end;
-- ------------------------------------------------------------------------

-- ------------------------------------------------------------------------
ARCHITECTURE fli OF MixMax IS
  SIGNAL clock : integer range 0 to 1;
  SIGNAL hi , lo : natural := 0 ;
begin
    clock <= 1 when clk = '1' else 0;
    MixMaxFliInstance : entity work.MixMaxFli PORT MAP( clock , hi , lo );
    DataOut <= STD_LOGIC_VECTOR( TO_UNSIGNED( hi , 30 ) &  TO_UNSIGNED( lo , 31 ) ); -- slightly perverse, but it appears that the modelsim FLI cannot cope with integers that use the MSB of a word 
end;
-- ------------------------------------------------------------------------



-- ------------------------------------------------------------------------
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;

entity Testbench is end;

architecture Testbench of Testbench is
    SIGNAL clk : std_logic;
    SIGNAL data_vhd , data_fli : STD_LOGIC_VECTOR( 60 DOWNTO 0 ) := (OTHERS=>'0');
begin

    MixMaxVhdInstance : entity work.MixMax(rtl) PORT MAP( clk , data_vhd );
    MixMaxFliInstance : entity work.MixMax(fli) PORT MAP( clk , data_fli );

    process(clk)
        VARIABLE CNT: INTEGER := -2;
    begin
        if rising_edge(clk) then
            if CNT > 0 THEN
                assert data_vhd = data_fli report "RTL/FLI mismatch" severity failure;
            end if;
            CNT := CNT + 1;
        end if;
    end process;
end;
-- ------------------------------------------------------------------------
