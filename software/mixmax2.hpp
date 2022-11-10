// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include "signal.hpp"

#include <iostream>
#include <iomanip>

constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

inline constexpr uint64_t Rotate_61bit( const uint64_t& aVal , const std::size_t& aSize )
{
  return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
}

inline uint64_t MOD_MERSENNE( const __uint128_t& aVal )
{
  return (aVal & M61) + (aVal >> 61); 
}


struct tRngState
{
  signal< uint64_t> W[ 12 ]; // 61 bit
  signal< bool > flag[ 16 ];  
  signal< bool > run;
  signal< uint64_t > PreSum0 , //62 bit
                     PrePartialSumOverOld , //63 bit
                     PartialSumOverOld , RotatedPreviousPartialSumOverOld , //61 bit
                     PreSum1A , //62 bit
                     PreSum1Aclk , //61 bit 
                     PreSum1B , //63 bit                     
                     SumOverNew; //61 bit 
  signal< __uint128_t > PreW0, //65bit
                        W0ResetVal , //64 bit
                        SumOverNewResetVal , //65 bit
                        PreSumOverNew; //67 bit

  //W{ 1,2,3,4,5,6,7,8,9,10,11,12,13 }
  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1 } , flag{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } , run( 0 ) , 
  PreSum0(0) , PrePartialSumOverOld( 0 ) , PartialSumOverOld( 0 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum1A( 0 ) , PreSum1Aclk( 0 ) , PreSum1B( 0 ), SumOverNew( 1 ) ,  
  PreW0( 0 ) , W0ResetVal( 0 ) , SumOverNewResetVal( 0 ) , PreSumOverNew( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    for( int i(0); i!=11; ++i ) W[ i + 1 ] = W[ i ];
    for( int i(0); i!=16; ++i ) flag[ ( i + 1 ) % 16 ] = flag[ i ];
    run = *flag[5] | *run;

    // ===================================================================================
    // Five clock-cycles ahead
    PreSum0 = *W[9] + *W[10];
    // ===================================================================================

    // ===================================================================================
    // Four clock-cycles ahead
    if( *flag[1] )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = *W[11];
      PrePartialSumOverOld = PreSum0;
    }
    else
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
      PartialSumOverOld = MOD_MERSENNE( *PrePartialSumOverOld );
      PrePartialSumOverOld = *PartialSumOverOld + *PreSum0;
    } 
    // ===================================================================================

    // ===================================================================================
    // Three clock-cycles ahead
    PreSum1A = *PartialSumOverOld + *RotatedPreviousPartialSumOverOld;
    // ===================================================================================

    // ===================================================================================
    // Two clock-cycle ahead
    PreSum1Aclk = MOD_MERSENNE( *PreSum1A );
    // ===================================================================================

    // ===================================================================================
    // One clock-cycle ahead
    PreSum1B = *PreSum1Aclk + *PreSum1A;
    W0ResetVal = *SumOverNew + *PreW0 + *PreSum1Aclk;
    SumOverNewResetVal = ( *SumOverNew << 1 ) + ( *PreW0 << 1 ) + *PreSum1Aclk; 
    // ===================================================================================

    // ===================================================================================
    // Current clock
    if( *flag[5] )
    {
      W[0] = RetVal = MOD_MERSENNE( *W0ResetVal );
      PreW0         = *W0ResetVal + *PreSum1Aclk;

      SumOverNew    = MOD_MERSENNE( *SumOverNewResetVal );
      PreSumOverNew = *SumOverNewResetVal + *W0ResetVal + *PreSum1Aclk;
    }
    else if( *run )
    { 
      W[0] = RetVal = MOD_MERSENNE( *PreW0 );
      PreW0         = *W[0] + *PreSum1B;

      SumOverNew    = MOD_MERSENNE( *PreSumOverNew );
      PreSumOverNew = *SumOverNew + *W[0] + *PreW0 + *PreSum1B;
    }
    // ===================================================================================

    return RetVal;
  }

      
};



