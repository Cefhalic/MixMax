// -----------------------------------------------------------------------------------------------------------------------
// Andrew W. Rose, 2022
// Imperial College London HEP group
// and
// Centre for High-throughput digital electronics and embedded machine learning
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
// PLEASE NOTE - THIS IS NOT AN OFFICIAL IMPLEMENTATION OF THE MIXMAX GENERATOR.
// THIS FILE ONLY EXISTS FOR THE VALIDATION OF REFACTORED CODE AND THE
// DEVELOPMENT OF THE VHDL IMPLEMENTATION.
// DO NOT USE THIS CODE IN YOUR OWN PROJECT: PLEASE USE THE VERSION FOUND,
// FOR EXAMPLE, IN THE BOOST C++ LIBRARY
// -----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include "signal.hpp"

#include <iostream>
#include <iomanip>

constexpr uint64_t M61( 0x1FFFFFFFFFFFFFFF );
constexpr uint64_t M64( 0xFFFFFFFFFFFFFFFF );

inline constexpr uint64_t Rotate_61bit( const __uint128_t& aVal , const std::size_t& aSize )
{
  return ((aVal << aSize) & M61) | ( aVal >> ( 61 - aSize ) ); 
}

inline uint64_t MOD_MERSENNE( const __uint128_t& aVal )
{
  return (aVal & M61) + (aVal >> 61); 
}


template < bool UseRun >
struct tRngState
{
  VhdlSignal< uint64_t > W[ 12 ];
  VhdlSignal< bool > flag[ 16 ];  
  VhdlSignal< bool > run;
  VhdlSignal< uint64_t > PartialSumOverOld , PrePartialSumOverOld;
  VhdlSignal< __uint128_t > SumOverNew , RotatedPreviousPartialSumOverOld , PreSum0 , PreSum1A , PreSum1Aclk , PreSum2A , PreSum2B , PreW0 , PreSumOverNew;
  VhdlSignal< __uint128_t > C ;

  //W{ 1,2,3,4,5,6,7,8,9,10,11,12,13 }
  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1 } , flag{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } , run( 0 ) , 
  PartialSumOverOld( 0 ) , PrePartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ), PreSum0(0) , PreSum1A( 0 ) , PreSum1Aclk( 0 ) , PreSum2A( 0 ) , PreSum2B( 0 )
   , PreW0( 0 ) , PreSumOverNew( 0 ) , C( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    for( int i(0); i!=11; ++i ) W[ i + 1 ] = W[ i ];
    for( int i(0); i!=16; ++i ) flag[ ( i + 1 ) % 16 ] = flag[ i ];
    run = *flag[4] | *run;

    // ===================================================================================
    // Four clock-cycles ahead
    PreSum0 = *W[9] + *W[10];
    // ===================================================================================

    // ===================================================================================
    // Three clock-cycles ahead
    if( *flag[1] )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = W[11];
      PrePartialSumOverOld = *PreSum0;
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

    if( (!UseRun) | *run ) PreSum2A = *PreSum1Aclk + *PreSum1A;
    PreSum2B = ( *PreSum1Aclk << 1 ) + *PreSum1A;


    if( *flag[4] )
    {
      PreSumOverNew = ( *PreSumOverNew << 1 ) + *PreSum1Aclk; 
    }
    else if( *flag[5] )
    {
      PreSumOverNew = ( 3 * *SumOverNew ) + *PreSum2B;
    }
    else 
    { 
      PreSumOverNew = *PreSumOverNew + *W[0] + *PreSum2A;
    }
  
    // C = *D + (*PreSum1A+ *PreSum1Aclk);

    if( *flag[4] )
    {
      // Could use "PreW0 = *PreW0 + *SumOverNew + *PreSum1A;" but then PreW0 accumulates unbounded.
      // Instead use W[0] and the additional sum, since W[0] has been mod-mersenne'd.
      PreW0 = *W[0] + *PreSum2A + *SumOverNew;
      // PreW0 = ( *W[0] << 2 ) + *C;
    }
    else
    { 
      PreW0 = *PreW0 + *PreSum1Aclk;
    }

    if( (!UseRun) | *run )
    {
      W[0] = RetVal = MOD_MERSENNE( *PreW0 );
      SumOverNew = MOD_MERSENNE( *PreSumOverNew );
    }

    return RetVal;
  }

      
};

// -----------------------------------------------------------------------------------------------------------------------
// PLEASE NOTE - THIS IS NOT AN OFFICIAL IMPLEMENTATION OF THE MIXMAX GENERATOR.
// THIS FILE ONLY EXISTS FOR THE VALIDATION OF REFACTORED CODE AND THE
// DEVELOPMENT OF THE VHDL IMPLEMENTATION.
// DO NOT USE THIS CODE IN YOUR OWN PROJECT: PLEASE USE THE VERSION FOUND,
// FOR EXAMPLE, IN THE BOOST C++ LIBRARY
// -----------------------------------------------------------------------------------------------------------------------


