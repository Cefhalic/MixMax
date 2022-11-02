#include <stdint.h>
#include <iostream>


namespace clean
{

  constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );

  inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
  {
    return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
  }

  inline constexpr uint64_t MOD_MERSENNE( const uint64_t& aVal )
  {
   return (aVal & M61) + (aVal >> 61); 
  }

  struct rng_state_t
  {
    uint64_t V[ 16 ];
    int counter;
    uint64_t PartialSumOverOld , SumOverNew , RotatedPreviousPartialSumOverOld , PreSum;
    
    rng_state_t() : V{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ,
    counter( -2 ) , PartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 )
    {}

    uint64_t get()
    {
      // ===================================================================================
      // Current clock
      auto Temp = SumOverNew + PreSum;

      if( counter == 0 )
      {
        V[counter] = MOD_MERSENNE( Temp );
        SumOverNew = MOD_MERSENNE( SumOverNew + Temp );
      }
      else if( counter > 0 )
      { 
        V[counter] = MOD_MERSENNE( V[counter-1] + PreSum );
        SumOverNew = MOD_MERSENNE( V[counter-1] + Temp );
      }
      // ===================================================================================
      
      // ===================================================================================
      // One clock-cycle ahead
      auto nextcounter = (counter+1) % 16;
      PreSum = MOD_MERSENNE( PartialSumOverOld + RotatedPreviousPartialSumOverOld );
      // ===================================================================================

      // ===================================================================================
      // Two clock-cycles ahead
      nextcounter = (counter+2) % 16;
      if( nextcounter == 0 )
      {
        RotatedPreviousPartialSumOverOld = 0;
        PartialSumOverOld = V[nextcounter];
      }
      else if( nextcounter > 0 )
      {
        RotatedPreviousPartialSumOverOld = Rotate_61bit( PartialSumOverOld , 36 );
        PartialSumOverOld = MOD_MERSENNE( PartialSumOverOld + V[nextcounter] ); 
      } 
      // ===================================================================================

      uint64_t RetVal = V[counter];
      counter = (counter+1) % 16;
      return RetVal;
    }
        
  };


}