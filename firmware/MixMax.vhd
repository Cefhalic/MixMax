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
  TYPE tData IS ARRAY( 0 TO 13 ) OF SIGNED( 63 DOWNTO 0 );
  signal W : tData := ( OTHERS => 64x"1" );
  signal flag : std_logic_vector( 0 TO 15 ) := ( 0 =>'1' , OTHERS=>'0' );
  signal run : std_logic := '0';

  signal PartialSumOverOld , RotatedPreviousPartialSumOverOld , PreSum : SIGNED( 63 DOWNTO 0 ) := ( OTHERS=>'0' );
  signal SumOverNew                                                    : SIGNED( 63 DOWNTO 0 ) := 64x"1";

  
  FUNCTION Rotate_61bit( aVal : SIGNED( 63 DOWNTO 0 ) ; aSize : INTEGER ) RETURN SIGNED IS
  BEGIN
    RETURN 3x"0" & ROTATE_LEFT( aVal( 60 DOWNTO 0 ) , aSize );
  END FUNCTION;

  FUNCTION MOD_MERSENNE( aVal : SIGNED( 63 DOWNTO 0 ) ) RETURN SIGNED IS
    VARIABLE Temp : SIGNED( 63 DOWNTO 0 ) := (OTHERS=>'0');
  BEGIN
    RETURN ( 3x"0" & aVal( 60 DOWNTO 0 ) ) + (61x"0" & aVal( 63 DOWNTO 61 ) );
  END FUNCTION;
  
  
BEGIN

  DataOut <= STD_LOGIC_VECTOR( W(0)( 60 DOWNTO 0 ) );

  PROCESS( Clk )
    variable Temp : SIGNED( 63 DOWNTO 0 ) := ( OTHERS=>'0' );
  BEGIN
    IF RISING_EDGE( Clk ) THEN

      W   ( 1 TO 13 ) <= W( 0 TO 12 );
      flag( 0 TO 15 ) <= flag( 15 ) & flag( 0 TO 14 );
      run             <= run or flag(1);

      -- ===================================================================================
      -- Two clock-cycles ahead
      if( flag(0) = '1' ) then
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
      if( flag(2) = '1' ) then
        W(0)       <= MOD_MERSENNE( Temp );
        SumOverNew <= MOD_MERSENNE( SumOverNew + Temp );
      elsif( run = '1' ) then
        W(0)       <= MOD_MERSENNE( W(0) + PreSum );
        SumOverNew <= MOD_MERSENNE( W(0) + Temp );
      end if;
      -- ===================================================================================

    END IF;
  END PROCESS;


END rtl;

