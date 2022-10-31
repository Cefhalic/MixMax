LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;

entity test is end;

architecture test of test is
    SIGNAL clk : std_logic;
    SIGNAL data_vhd , data_fli : STD_LOGIC_VECTOR( 60 DOWNTO 0 ) := (OTHERS=>'0');
begin

    MixMaxVhdInstance : entity work.MixMax(rtl) PORT MAP( clk , data_vhd );
    MixMaxFliInstance : entity work.MixMax(fli) PORT MAP( clk , data_fli );

    process(clk)
    begin
        if rising_edge(clk) then
            assert data_vhd = data_fli report "RTL/FLI mismatch" severity failure;
        end if;
    end process;
end;
