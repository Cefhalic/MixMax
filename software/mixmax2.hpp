#include <stdint.h>
#include "signal.hpp"

constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
{
  return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
}

inline uint64_t MOD_MERSENNE( const uint64_t& aVal )
{
  // if ( aVal >= M61 ) return aVal - M61;
  return aVal % M61; 
 //return (aVal & M61) + ((aVal >> 61)&0x7); 
}


struct tRngState
{
  signal< uint64_t> W[ 16 ];
  int counter;
  signal< uint64_t> PartialSumOverOld , SumOverNew , RotatedPreviousPartialSumOverOld , PreSum;

  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } ,
  counter( -3 ) , PartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    for( int i(0); i!=15; ++i ) W[ i + 1 ] = W[ i ];

    // ===================================================================================
    // Two clock-cycles ahead
    auto nextcounter = (counter+2) % 16;
    if( nextcounter == 0 )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = W[13];
    }
    else if( nextcounter > 0 )
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
      PartialSumOverOld = *PartialSumOverOld + *W[13]; 
    } 
    // ===================================================================================

    // ===================================================================================
    // One clock-cycles ahead
    PreSum = MOD_MERSENNE( *PartialSumOverOld + *RotatedPreviousPartialSumOverOld );
    // ===================================================================================

    // ===================================================================================
    // Current clock
    auto Temp = *SumOverNew + *PreSum; // Variable

    if( counter == 0 )
    {
      W[0] = RetVal = MOD_MERSENNE( Temp );
      SumOverNew = MOD_MERSENNE( *SumOverNew + Temp );
    }
    else if( counter > 0 )
    { 
      W[0] = RetVal = MOD_MERSENNE( *W[0] + *PreSum );
      SumOverNew = MOD_MERSENNE( *W[0] + Temp );
    }
    // ===================================================================================

    counter = (counter+1) % 16;
    return RetVal;
  }

      
};



