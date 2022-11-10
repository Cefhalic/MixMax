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
  signal< uint64_t> W[ 13 ];
  signal< bool > flag[ 16 ];  
  signal< bool > run;
  signal< __uint128_t > PartialSumOverOld , PrePartialSumOverOld , SumOverNew , RotatedPreviousPartialSumOverOld , PreSum0 , PreSum1A , PreSum1Aclk , PreSum1Aclk2 , PreSum1B , PreW0;
  signal< __uint128_t > C , D , PreSumOverNew;

  //W{ 1,2,3,4,5,6,7,8,9,10,11,12,13 }
  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1,1 } , flag{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } , run( 0 ) , 
  PartialSumOverOld( 0 ) , PrePartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ), PreSum0(0) , PreSum1A( 0 ) , PreSum1Aclk( 0 ) , PreSum1Aclk2( 0 ) , PreSum1B( 0 )
   , PreW0( 0 ) , C( 0 ) , D( 0 ) , PreSumOverNew( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    for( int i(0); i!=12; ++i ) W[ i + 1 ] = W[ i ];
    for( int i(0); i!=16; ++i ) flag[ ( i + 1 ) % 16 ] = flag[ i ];
    run = *flag[4] | *run;

    // ===================================================================================
    // Four clock-cycles ahead
    PreSum0 = *W[10] + *W[11];
    // ===================================================================================

    // ===================================================================================
    // Three clock-cycles ahead
    if( *flag[1] )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = *W[12];
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
    // Two clock-cycles ahead
    PreSum1A = *PartialSumOverOld + *RotatedPreviousPartialSumOverOld;
    // ===================================================================================

    // ===================================================================================
    // One clock-cycle ahead
    PreSum1Aclk = MOD_MERSENNE( *PreSum1A );
    // PreSum1Aclk2 = *PreSum1Aclk;
    // PreSum1B = *PreSum1Aclk + *PreSum1A;

    C = *SumOverNew + *PreW0 + *PreSum1A;
    D = ( *SumOverNew << 1 ) + ( *PreW0 << 1 ) + *PreSum1A; 
    // ===================================================================================

    // ===================================================================================
    // Current clock
    if( *flag[4] )
    {
      // std::cout << "-----" << std::endl;

      W[0] = RetVal = MOD_MERSENNE( *C );
      PreW0         = *C + *PreSum1A;

      SumOverNew    = MOD_MERSENNE( *D );
      PreSumOverNew = *D + *C + *PreSum1A;

    }
    else if( *run )
    { 
      W[0] = RetVal = MOD_MERSENNE( *PreW0 );
      PreW0 = *W[0] + *PreSum1Aclk + *PreSum1A;

      SumOverNew    = MOD_MERSENNE( *PreSumOverNew );
      PreSumOverNew = *SumOverNew + *W[0] + *PreW0 + *PreSum1Aclk + *PreSum1A;
    }
    // ===================================================================================

    // std::cout << "C" << std::setw(26) << *C << " | D" << std::setw(26) << *D <<  " | X" << std::setw(26) << *X << " | PreSumOverNew" << std::setw(26) << *PreSumOverNew << " | SumOverNew" << std::setw(26) << *SumOverNew << " | Diff" << std::setw(26) << int64_t( *SumOverNew - *X ) << std::endl;
    // std::cout << "PreSum1A" << std::setw(16) << *PreSum1A << " PreSum1Aclk" << std::setw(16) << *PreSum1Aclk << " | W0" << std::setw(16) << *W[0] << " PreW0" << std::setw(16) << *PreW0 << " | X" << std::setw(16) << *X << " PreSumOverNew" << std::setw(16) << *PreSumOverNew << " | SumOverNew" << std::setw(16) << *SumOverNew << " | Diff" << std::setw(16) << int64_t( *SumOverNew - *X ) << std::endl;

    return RetVal;
  }

      
};



