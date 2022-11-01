#include <stdint.h>
#include "signal.hpp"

#include <iostream>
// #include <iomanip>

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
  signal< uint64_t> W[ 16 ];
  signal< bool > flag[ 19 ];
  signal< bool > run;
  signal< uint64_t> PartialSumOverOld , SumOverNew , RotatedPreviousPartialSumOverOld , PreSum;

  tRngState() : W{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } , flag{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } , run( 0 ) ,
  PartialSumOverOld( 0 ) , SumOverNew( 1 ) , RotatedPreviousPartialSumOverOld( 0 ) , PreSum( 0 )
  {}


  uint64_t get()
  {
    uint64_t RetVal(0xDEADBEEF);

    // std::cout << std::hex << std::setfill('0');
    //for( auto&i : W ) std::cout << " " << std::setw(16) << *i;
    // std::cout << " " << std::setw(16) << *W[0] << " " << std::setw(16) << *W[13] ;
    // std::cout << " " << std::setw(16) << *PartialSumOverOld << " " << std::setw(16) << *SumOverNew << " " << std::setw(16) << *RotatedPreviousPartialSumOverOld << " " << std::setw(16) << *PreSum << std::endl;

    for( int i(0); i!=15; ++i ) W[ i + 1 ] = W[ i ];

    flag[0] = 0;
    for( int i(0); i!=18; ++i ) flag[ i + 1 ] = flag[ i ];
    flag[3] = *flag[2] | *flag[18];
    if( *flag[2] ) run = 1;


    // ===================================================================================
    // Two clock-cycles ahead
    if( *flag[1] || *flag[17] )
    {
      RotatedPreviousPartialSumOverOld = 0;
      PartialSumOverOld = W[13];
    }
    else
    {
      RotatedPreviousPartialSumOverOld = Rotate_61bit( *PartialSumOverOld , 36 );
      PartialSumOverOld = MOD_MERSENNE( *PartialSumOverOld + *W[13] ); 
    } 
    // ===================================================================================

    // ===================================================================================
    // One clock-cycles ahead
    PreSum = MOD_MERSENNE( *PartialSumOverOld + *RotatedPreviousPartialSumOverOld );
    // ===================================================================================

    // ===================================================================================
    // Current clock
    auto Temp = *SumOverNew + *PreSum; // Variable

    if( *flag[3] )
    {
      W[0] = RetVal = MOD_MERSENNE( Temp );
      SumOverNew = MOD_MERSENNE( *SumOverNew + Temp );
    }
    else if( *run )
    { 
      W[0] = RetVal = MOD_MERSENNE( *W[0] + *PreSum );
      SumOverNew = MOD_MERSENNE( *W[0] + Temp );
    }
    // ===================================================================================

    return RetVal;
  }

      
};



