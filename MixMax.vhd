LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY MixMax IS
  PORT(
    Clk : IN STD_LOGIC;
    DataOut : OUT STD_LOGIC_VECTOR( 60 DOWNTO 0 )
  );
END MixMax;


ARCHITECTURE rtl OF MixMax IS
  TYPE tData IS ARRAY( 0 TO 15 ) OF SIGNED( 63 DOWNTO 0 );
  signal W : tData := ( OTHERS => 64x"1" );
  signal PartialSumOverOld , RotatedPreviousPartialSumOverOld , PreSum : SIGNED( 63 DOWNTO 0 ) := ( OTHERS=>'0' );
  signal SumOverNew                                                    : SIGNED( 63 DOWNTO 0 ) := 64x"1";

  signal counter : integer range -2 to 15 := -2;
  
  
  FUNCTION Rotate_61bit( aVal : SIGNED( 63 DOWNTO 0 ) ; aSize : INTEGER ) RETURN SIGNED IS
  BEGIN
    RETURN 3x"0" & ROTATE_LEFT( aVal( 60 DOWNTO 0 ) , aSize );
  END FUNCTION;

  FUNCTION MOD_MERSENNE( aVal : SIGNED( 63 DOWNTO 0 ) ) RETURN SIGNED IS
    VARIABLE Temp : SIGNED( 63 DOWNTO 0 ) := (OTHERS=>'0');
  BEGIN
    Temp := aVal - 64x"1FFFFFFFFFFFFFFF";
    if Temp < 0 then
      return aVal;
    else
      return Temp;
    end if;
  END FUNCTION;
  
  
BEGIN

  DataOut <= STD_LOGIC_VECTOR( W(0)( 60 DOWNTO 0 ) );

  PROCESS( Clk )
    variable Temp : SIGNED( 63 DOWNTO 0 ) := ( OTHERS=>'0' );
    variable nextcounter : integer range -15 to 15 := -15;
  BEGIN
    IF RISING_EDGE( Clk ) THEN

    W( 1 TO 15 ) <= W( 0 TO 14 );

    -- ===================================================================================
    -- Two clock-cycles ahead
    nextcounter := (counter+2) mod 16;
    
    if( nextcounter = 0 ) then
      RotatedPreviousPartialSumOverOld <= 64x"0";
      PartialSumOverOld                <= W(13);
    else
      RotatedPreviousPartialSumOverOld <= Rotate_61bit( PartialSumOverOld , 36 );
      PartialSumOverOld                <= MOD_MERSENNE( PartialSumOverOld + W(13) ); 
    end if; 
    -- ===================================================================================

    -- ===================================================================================
    -- One clock-cycles ahead
    PreSum <= MOD_MERSENNE( PartialSumOverOld + RotatedPreviousPartialSumOverOld );
    -- ===================================================================================

    -- ===================================================================================
    -- Current clock
    Temp := SumOverNew + PreSum;
    if( counter = 0 ) then
      W(0)       <= MOD_MERSENNE( Temp );
      SumOverNew <= MOD_MERSENNE( SumOverNew + Temp );
    elsif( counter > 0 ) then
      W(0)       <= MOD_MERSENNE( W(0) + PreSum );
      SumOverNew <= MOD_MERSENNE( W(0) + Temp );
    end if;
    -- ===================================================================================

    if( counter < 0 ) then
      counter <= (counter+1);
    else
      counter <= (counter+1) mod 16;
    end if;

    END IF;
  END PROCESS;


END rtl;




-- ------------------------------------------------------------------------
entity MixMaxFli is
port(
    clk : in integer range 0 to 1;
    hi , lo : out integer
);
end;

architecture fli of MixMaxFli is
    attribute foreign : string;
    attribute foreign of fli : architecture is "MixMaxFli ../mixmaxfli.so";
begin end;
-- ------------------------------------------------------------------------




ARCHITECTURE fli OF MixMax IS
  SIGNAL clock : integer range 0 to 1;
  SIGNAL hi , lo : integer;
begin
  clock <= 1 when clk = '1' else 0;
  MixMaxFliInstance : entity work.MixMaxFli PORT MAP( clock , hi , lo );
  DataOut( 31 DOWNTO  0 ) <= STD_LOGIC_VECTOR( TO_SIGNED( lo , 32 ) );
  DataOut( 60 DOWNTO 32 ) <= STD_LOGIC_VECTOR( TO_SIGNED( hi , 29 ) );  
end;