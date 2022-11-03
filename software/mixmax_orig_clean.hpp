#include <stdint.h>
#include <iostream>
#include <iomanip>


namespace clean
{

  constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
  constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

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
    uint64_t PartialSumOverOld , SumOverNew; 
    uint64_t RetVal;

    rng_state_t() : V{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } , counter( 0 ) , PartialSumOverOld( 0 ) , SumOverNew( 1 )
    {}

    uint64_t get()
    {
      if( counter == 0 )
      {
        PartialSumOverOld = V[counter]; 
        RetVal = V[counter] = MOD_MERSENNE( SumOverNew + PartialSumOverOld );
      }
      else
      {
        uint64_t RotatedPreviousPartialSumOverOld( Rotate_61bit( PartialSumOverOld , 36 ) );
        PartialSumOverOld = MOD_MERSENNE( PartialSumOverOld + V[counter] ); 
        RetVal = V[counter] = MOD_MERSENNE( V[counter-1] + PartialSumOverOld + RotatedPreviousPartialSumOverOld );
      }

      SumOverNew = MOD_MERSENNE( SumOverNew + V[counter] ); 
      counter = (counter+1) % 16;
      return RetVal;
    }        
  };


}