// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

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
  return (aVal & M61) + ((aVal >> 61)&0x7); 
}


struct tRngState
{
  signal< uint64_t> W[ 13 ];
  signal< bool > flag[ 16 ];  
  signal< bool > run;
  signal< uint64_t> PartialSumOverOld , SumOverNew , RotatedPreviousPartialSumOverOld , PreSum , PreSum2;

  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1,1 } , flag{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } , run( 0 ) , 
  PartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 ) , PreSum2( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    for( int i(0); i!=12; ++i ) W[ i + 1 ] = W[ i ];
    for( int i(0); i!=16; ++i ) flag[ ( i + 1 ) % 16 ] = flag[ i ];
    run = *flag[2] | *run;

    // ===================================================================================
    // Three clock-cycles ahead
    if( *flag[0] )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = W[12];
    }
    else
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
      PartialSumOverOld = MOD_MERSENNE( *PartialSumOverOld + *W[12] ); 
    } 
    // ===================================================================================

    // ===================================================================================
    // Two clock-cycles ahead
    PreSum = *PartialSumOverOld + *RotatedPreviousPartialSumOverOld;
    // ===================================================================================

    // ===================================================================================
    // One clock-cycle ahead
    PreSum2 = MOD_MERSENNE( *PreSum );
    // ===================================================================================

    // ===================================================================================
    // Current clock
    auto Temp = *SumOverNew + *PreSum2; // Variable

    if( *flag[3] )
    {
      W[0] = RetVal = MOD_MERSENNE( Temp );
      SumOverNew = MOD_MERSENNE( *SumOverNew + Temp );
    }
    else if( *run )
    { 
      W[0] = RetVal = MOD_MERSENNE( *W[0] + *PreSum2 );
      SumOverNew = MOD_MERSENNE( *W[0] + Temp );
    }
    // ===================================================================================

    return RetVal;
  }

      
};



